#include "bits.h"
#include "tela.h"
#include "lista.h"
#include "rede.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

// dados de configuração
#define MUNDO_LARGURA 600
#define MUNDO_ALTURA 150
#define FPS 30
#define GRAVIDADE 2100
#define GRAVIDADE_2 (2 * GRAVIDADE)
#define Y_CHAO 140

#define DINO_VEL_INI 250//150
#define DINO_VEL_MAX 500
#define DINO_ACEL 10
#define DINO_POS_X 30
#define DINO_VEL_INI_PULO -550
#define DINO_PX_POR_PASSO 10 // quantos pixels o dino anda a cada passo

// retorna um número aleatório positivo menor que 1
float aleat()
{
  return (double)rand() / ((double)RAND_MAX + 1);
}

// retorna um número aleatório entre a e b, inclusive
int aleat_entre(int a, int b)
{
  return a + aleat() * (b - a + 1); 
}

// retorna um bool que tem nch chances de ser true em cada segundo,
//   sendo chamada FPS vezes por segundo
bool chances_por_s(float nch)
{
  return aleat() < (nch / FPS) / 2;
}

// Imagens

enum {
  // cactus
  IM_C1, IM_C2, IM_C3,
  // dígitos
  IM_D0, IM_D1, IM_D2, IM_D3, IM_D4, IM_D5, IM_D6, IM_D7, IM_D8, IM_D9,
  // estrelas
  IM_E1, IM_E2, IM_E3,
  // cactus grandes
  IM_G1, IM_G2, IM_G3,
  // game over
  IM_GO,
  // horizonte
  IM_H1, IM_H2, IM_HI,
  // lua
  IM_L1, IM_L2, IM_L3, IM_L4, IM_L5, IM_L6, IM_L7,
  // nuvem
  IM_NU,
  // pterodáctilo
  IM_P1, IM_P2,
  // dino (rex)
  IM_R1, IM_R2, IM_R3, IM_R4, IM_R5, IM_R6, IM_R7, IM_R8, IM_R9,
  N_IMAGENS,
};

Bits imagens[N_IMAGENS];
bool imagens_lidas = false;
void le_imagens()
{
  if (imagens_lidas) return;
  imagens_lidas = true;
  imagens[IM_C1] = b_cria_arq("im/c1");
  imagens[IM_C2] = b_cria_arq("im/c2");
  imagens[IM_C3] = b_cria_arq("im/c3");
  imagens[IM_D0] = b_cria_arq("im/d0");
  imagens[IM_D1] = b_cria_arq("im/d1");
  imagens[IM_D2] = b_cria_arq("im/d2");
  imagens[IM_D3] = b_cria_arq("im/d3");
  imagens[IM_D4] = b_cria_arq("im/d4");
  imagens[IM_D5] = b_cria_arq("im/d5");
  imagens[IM_D6] = b_cria_arq("im/d6");
  imagens[IM_D7] = b_cria_arq("im/d7");
  imagens[IM_D8] = b_cria_arq("im/d8");
  imagens[IM_D9] = b_cria_arq("im/d9");
  imagens[IM_E1] = b_cria_arq("im/e1");
  imagens[IM_E2] = b_cria_arq("im/e2");
  imagens[IM_E3] = b_cria_arq("im/e3");
  imagens[IM_G1] = b_cria_arq("im/g1");
  imagens[IM_G2] = b_cria_arq("im/g2");
  imagens[IM_G3] = b_cria_arq("im/g3");
  imagens[IM_GO] = b_cria_arq("im/go");
  imagens[IM_H1] = b_cria_arq("im/h1");
  imagens[IM_H2] = b_cria_arq("im/h2");
  imagens[IM_HI] = b_cria_arq("im/hi");
  imagens[IM_L1] = b_cria_arq("im/l1");
  imagens[IM_L2] = b_cria_arq("im/l2");
  imagens[IM_L3] = b_cria_arq("im/l3");
  imagens[IM_L4] = b_cria_arq("im/l4");
  imagens[IM_L5] = b_cria_arq("im/l5");
  imagens[IM_L6] = b_cria_arq("im/l6");
  imagens[IM_L7] = b_cria_arq("im/l7");
  imagens[IM_NU] = b_cria_arq("im/nu");
  imagens[IM_P1] = b_cria_arq("im/p1");
  imagens[IM_P2] = b_cria_arq("im/p2");
  imagens[IM_R1] = b_cria_arq("im/r1");
  imagens[IM_R2] = b_cria_arq("im/r2");
  imagens[IM_R3] = b_cria_arq("im/r3");
  imagens[IM_R4] = b_cria_arq("im/r4");
  imagens[IM_R5] = b_cria_arq("im/r5");
  imagens[IM_R6] = b_cria_arq("im/r6");
  imagens[IM_R7] = b_cria_arq("im/r7");
  imagens[IM_R8] = b_cria_arq("im/r8");
  imagens[IM_R9] = b_cria_arq("im/r9");
}

typedef struct {
  float x;
  float y;
} vet2;

typedef struct {
  vet2 pos;
  vet2 vel;
  vet2 acel;
} pva;

void pva_atualiza(pva *p, float delta_t)
{
  p->pos.x += p->vel.x * delta_t;
  p->pos.y += p->vel.y * delta_t;
  p->vel.x += p->acel.x * delta_t;
  p->vel.y += p->acel.y * delta_t;
}

// desenhos

// desenha o valor de n no canvas, com ndig dígitos
// o final fica na posição x,y do canvas
void desenha_numero(Bits canvas, int x, int y, int n, int ndig)
{
  if (n < 0) { n = -0; b_desenha(canvas, x, y, imagens[IM_E1]); }
  for (int i = 0; i < ndig; i++) {
    b_desenha(canvas, x - 12*i, y, imagens[IM_D0+n%10]);
    n /= 10;
  }
}

// dino

typedef enum { esperando, correndo, pulando, descendo, abaixado, morto } dino_estado_t;
typedef struct {
  dino_estado_t estado;
  float dist_passo;
  Bits imagem;
  pva pva;
  bool piscando;
} dino, *Dino;

void dino_escolhe_imagem(Dino dino)
{
  int passo = dino->dist_passo / DINO_PX_POR_PASSO;
  switch (dino->estado) {
    case esperando:
      if (dino->piscando) dino->piscando = !chances_por_s(20);
      else                dino->piscando = chances_por_s(1/3.);
      dino->imagem = imagens[IM_R2 + (dino->piscando ? 1 : 0)];
      break;
    case correndo:
      dino->imagem = imagens[IM_R4 + passo];
      break;
    case pulando:
    case descendo:
      dino->imagem = imagens[IM_R2];
      break;
    case abaixado:
      dino->imagem = imagens[IM_R8 + passo];
      break;
    case morto:
      dino->imagem = imagens[IM_R6];
  }
}

void dino_muda_estado(Dino dino, dino_estado_t novo_estado)
{
  dino_estado_t estado_anterior = dino->estado;
  dino->estado = novo_estado;
  dino_escolhe_imagem(dino);
  switch (novo_estado) {
    case esperando:
      dino->pva = (pva){ .pos = { DINO_POS_X, Y_CHAO - b_altura(dino->imagem)}, .vel = {0, 0}, .acel = {0, 0} };
      dino->dist_passo = 0;
      break;
    case correndo:
      if (dino->pva.acel.x == 0) {
        dino->pva.vel.x = DINO_VEL_INI;
        dino->pva.acel.x = DINO_ACEL;
      }
      dino->pva.vel.y = 0;
      dino->pva.acel.y = 0;
      dino->pva.pos.y = Y_CHAO - b_altura(dino->imagem);
      break;
    case pulando:
      dino->pva.acel.y = GRAVIDADE;
      dino->pva.vel.y = DINO_VEL_INI_PULO;
      break;
    case descendo:
      if (dino->pva.vel.y < 0) dino->pva.vel.y = 0;
      dino->pva.acel.y = GRAVIDADE_2;
      break;
    case morto:
      dino->pva.vel = (vet2){0, 0};
      dino->pva.acel = (vet2){0, 0};
      if (estado_anterior == abaixado) {
        dino->pva.pos.y = Y_CHAO - b_altura(imagens[IM_R6]);
      }
      break;
    case abaixado:
      dino->pva.pos.y = Y_CHAO - b_altura(dino->imagem);
      break;
  }
}

void dino_reset(Dino dino)
{
  dino_muda_estado(dino, esperando);
}

void dino_processa_entradas(Dino dino, bool up, bool down)
{
  if (up && !down) {
    switch (dino->estado) {
      case abaixado:
        dino_muda_estado(dino, correndo);
        break;
      case correndo:
      case esperando:
        dino_muda_estado(dino, pulando);
        break;
      case pulando: case descendo: case morto:
        break;
    }
  }
  if (down && !up) {
    switch (dino->estado) {
      case correndo:
        dino_muda_estado(dino, abaixado);
        break;
      case pulando:
        dino_muda_estado(dino, descendo);
        break;
      case abaixado: case esperando: case descendo: case morto:
        break;
    }
  }
}

float dino_atualiza(Dino dino, float delta_t, bool up, bool down)
{
  dino_processa_entradas(dino, up, down);
  pva_atualiza(&dino->pva, delta_t);
  float delta_x = dino->pva.pos.x - DINO_POS_X;
  dino->pva.pos.x = DINO_POS_X;
  dino->dist_passo += delta_x;
  while (dino->dist_passo > 2 * DINO_PX_POR_PASSO) dino->dist_passo -= 2 * DINO_PX_POR_PASSO;
  if (dino->pva.vel.x > DINO_VEL_MAX) dino->pva.vel.x = DINO_VEL_MAX;
  //if (dino->pva.vel.y > 0) dino->pva.pos.y += dino->pva.vel.y * delta;
  if ((dino->estado == pulando || dino->estado == descendo) && dino->pva.pos.y + b_altura(dino->imagem) > Y_CHAO) {
    dino_muda_estado(dino, correndo);
  }
  dino_escolhe_imagem(dino);
  return delta_x;
}


// objetos

typedef enum { chao, cactus_p, cactus_g, pterodactilo, estrela, lua, nuvem } tipo_de_objeto;
#define NTIPOS (nuvem + 1)
typedef struct {
  tipo_de_objeto tipo;
  Bits imagem;
  vet2 posicao;
  bool move_com_distancia;
  bool move_com_tempo;
  bool colide;
  vet2 velocidade;  // caso mova com tempo
} objeto, *Objeto;

void obj_init(Objeto obj, tipo_de_objeto tipo, int pos_x)
{
  int pos_ptero[] = { 20, 35, 70 };
  obj->tipo = tipo;
  switch (tipo) {
    case cactus_p:
      obj->imagem = imagens[IM_C1 + aleat_entre(0, 2)];
      obj->posicao = (vet2){ pos_x, Y_CHAO - b_altura(obj->imagem)};
      obj->move_com_distancia = true;
      obj->move_com_tempo = false;
      obj->colide = true;
      break;
    case cactus_g:
      obj->imagem = imagens[IM_G1 + aleat_entre(0, 2)];
      obj->posicao = (vet2){ pos_x, Y_CHAO - b_altura(obj->imagem)};
      obj->move_com_distancia = true;
      obj->move_com_tempo = false;
      obj->colide = true;
      break;
    case pterodactilo:
      obj->imagem = imagens[IM_P1];
      obj->posicao = (vet2){ pos_x, Y_CHAO - b_altura(obj->imagem) - pos_ptero[aleat_entre(0,2)] };
      obj->move_com_distancia = true;
      obj->move_com_tempo = true;
      obj->colide = true;
      obj->velocidade = (vet2){ aleat_entre(-20,-10), 0 };
      break;
    case chao:
      obj->imagem = imagens[IM_H1 + (aleat() < .8)];
      obj->posicao = (vet2){pos_x, Y_CHAO - 8};
      obj->move_com_distancia = true;
      obj->move_com_tempo = false;
      obj->colide = false;
      break;
    default:
      break;
  }
}

void obj_move(Objeto obj, float delta_x, float delta_t, float dist_ref)
{
  if (obj->move_com_distancia) {
    obj->posicao.x -= delta_x;
  }
  if (obj->move_com_tempo) {
    obj->posicao.x += obj->velocidade.x * delta_t;
    obj->posicao.y += obj->velocidade.y * delta_t;
    if (obj->tipo == pterodactilo) {
      // faz bater asas de acordo com a distância ao solo
      obj->imagem = imagens[IM_P1 + (int)(obj->posicao.x - dist_ref) / 5 % 2];
    }
  }
}

// jogo

typedef struct {
  Bits canvas;
  double data_da_morte;
  double tempo;
  float delta_t;
  float distancia;
  float delta_d;
  int pontos;
  int recorde;
  dino rex;
  Lista objetos;
  int dist_ultimo_obstaculo;
  bool tem_tela;
  bool tem_rede_neural;
  // entradas
  bool up;
  bool down;
  bool quit;
  Rede rede;
  // entradas para a rede
  float entradas[N_ENTRADAS];
} jogo, *Jogo;

void jogo_calcula_delta_t(Jogo j)
{
  if (j->tem_tela) {
    static double ultimo = 0;
    double agora = tela_relogio();
    j->delta_t = agora - ultimo;
    ultimo = agora;
    //j->delta_t *= 1.5;
  } else {
    j->delta_t = 1. / FPS;
  }
  j->tempo += j->delta_t;
}

void jogo_calcula_entradas(Jogo j)
{
  if (!j->tem_rede_neural) {
    tecla tec = tela_le_tecla();
    j->up = tec == t_up || tec == ' ';
    j->down = tec == t_down;
    j->quit = tec == 'q';
  } else {
    // envia o estado do jogo para a entrada da rede neural, executa a rede,
    //   pega a saída e coloca em up e down
    float *entradas = j->entradas;
    for (int i = 0; i < N_ENTRADAS; i++) entradas[i] = 600;
    float saidas[N_SAIDAS];
    int di = j->rex.pva.pos.y;
    int df = di + b_altura(j->rex.imagem);
    for (l_inicio(j->objetos); l_avanca(j->objetos);) {
      Objeto obj = l_item_ptr(j->objetos);
      if (!obj->colide) continue;
      int yi = obj->posicao.y;
      int yf = yi + b_altura(obj->imagem);
      int xi = obj->posicao.x;
      int xf = xi + b_largura(obj->imagem);
      if (xf < DINO_POS_X) continue;
      if (xi < 0) xi = 0;
      if (yf > di && yi < df && xi < entradas[0]) { entradas[0] = xi; entradas[1] = xf; }
      if (yf > Y_CHAO - 27) {
        if (xi < entradas[2]) { entradas[5] = entradas[2]; entradas[2] = xi; if (xf < entradas[1]) entradas[1] = xf; } else if (xi < entradas[5]) entradas[5] = xi;
      }
      if (yf > Y_CHAO - 42 && yi < Y_CHAO - 27) {
        if (xi < entradas[3]) { entradas[6] = entradas[3]; entradas[3] = xi; if (xf < entradas[1]) entradas[1] = xf; } else if (xi < entradas[6]) entradas[6] = xi;
      }
      if (yi < Y_CHAO - 45) {
        if (xi < entradas[4]) { entradas[7] = entradas[4]; entradas[4] = xi; if (xf < entradas[1]) entradas[1] = xf; } else if (xi < entradas[7]) entradas[7] = xi;
      }
    }
    for(int i = 0; i < N_ENTRADAS - 1; i++) entradas[i] /= 600;

    rede_calcula(j->rede, entradas, saidas);

    j->up = (saidas[0] > 0.5);
    j->down = (saidas[1] > 0.5);
  }
}

void jogo_init(Jogo j)
{
  l_posiciona(j->objetos, 0);
  while (!l_vazia(j->objetos)) l_remove(j->objetos, NULL);
  objeto obj;
  obj_init(&obj, chao, 0);
  l_insere_depois(j->objetos, &obj);
  obj_init(&obj, chao, MUNDO_LARGURA);
  l_insere_depois(j->objetos, &obj);
  dino_reset(&j->rex);
  jogo_calcula_delta_t(j);
  j->tempo = 0;
  j->distancia = 0;
  j->dist_ultimo_obstaculo = 0;
  j->pontos = 0;
  if (j->tem_rede_neural) dino_muda_estado(&j->rex, correndo);
}

bool jogo_termina(Jogo j)
{
  if (j->tem_rede_neural) {
    if (j->rex.estado == morto) return true;
    // limita o tempo máximo de jogo
    if (j->distancia > 90000) return true;
  } else {
    if (j->rex.estado == morto && j->quit) return true;
    if (j->rex.estado == morto && j->tempo - j->data_da_morte > 1 && j->up) jogo_init(j);
  }
  return false;
}

Jogo jogo_cria(bool tem_tela, Rede rede)
{
  Jogo j = malloc(sizeof(*j));
  assert(j != NULL);
  j->rede = rede;
  j->tem_tela = tem_tela;
  j->tem_rede_neural = rede != NULL;
  if (j->tem_tela) {
    tela_cria(FPS);
    tela_seleciona_cursor(invisivel);
  }
  le_imagens();
  j->canvas = b_cria(MUNDO_LARGURA, MUNDO_ALTURA);
  j->objetos = l_cria(sizeof(objeto));
  j->recorde = 0;
  jogo_init(j);
  return j;
}

void jogo_destroi(Jogo j)
{
  b_destroi(j->canvas);
  l_destroi(j->objetos);
  if (j->tem_tela) tela_destroi();
  free(j);
}

void jogo_verifica_colisao(Jogo j)
{
  if (j->rex.estado == morto) return;
  for (l_inicio(j->objetos); l_avanca(j->objetos);) {
    Objeto obj = l_item_ptr(j->objetos);
    if (obj->colide) {
      if (b_colide(j->rex.imagem, obj->posicao.x - j->rex.pva.pos.x, obj->posicao.y - j->rex.pva.pos.y, obj->imagem)) {
        dino_muda_estado(&j->rex, morto);
        if (j->rex.estado == morto) {
          j->data_da_morte = j->tempo;
          if (j->pontos > j->recorde) j->recorde = j->pontos;
        }
      }
    }
  }
}

void jogo_remove_objetos(Jogo j)
{
  for (l_inicio(j->objetos); l_avanca(j->objetos);) {
    Objeto obj = l_item_ptr(j->objetos);
    if (obj->posicao.x + b_largura(obj->imagem) < 0) {
      switch (obj->tipo) {
        case chao:
          obj_init(obj, chao, obj->posicao.x + 2 * MUNDO_LARGURA);
          break;
        case cactus_p:
        case cactus_g:
        case pterodactilo:
          l_remove(j->objetos, NULL);
          l_recua(j->objetos);
          break;
        default:
          break;
      }
    }
  }
}

void jogo_cria_objetos(Jogo j)
{
  int dist_min = 600 - 200 * (j->distancia > 3000) - 200 * (j->distancia > 10000);
  int dist_var = 300 - 100 * (j->distancia > 5000);
  int dist = j->distancia + MUNDO_LARGURA - j->dist_ultimo_obstaculo;
  if (dist < dist_min) return;

  tipo_de_objeto tipo;
  /*
  if (j->distancia > 7000 && aleat() < .4 || j->distancia > 3000 && aleat() < .1) tipo = pterodactilo;
  else if (j->distancia > 5000 && aleat() < .6 || j->distancia > 1000 && aleat() < .5) tipo = cactus_g;
  */
  float a = aleat();
  if (a < .3) tipo = pterodactilo;
  else if (a < .6) tipo = cactus_g;
  else tipo = cactus_p;

  objeto obj;
  j->dist_ultimo_obstaculo += dist + aleat_entre(0, dist_var);
  obj_init(&obj, tipo, j->dist_ultimo_obstaculo - j->distancia);
  l_insere_depois(j->objetos, &obj);
}

void jogo_move_objetos(Jogo j)
{
  // pterodáctilos precisam de uma referência no solo para bater asas
  int ref = -MUNDO_LARGURA - (int)(j->distancia + MUNDO_LARGURA) % MUNDO_LARGURA;
  for (l_inicio(j->objetos); l_avanca(j->objetos);) {
    obj_move(l_item_ptr(j->objetos), j->delta_d, j->delta_t, ref);
  }
}

void jogo_move_dino(Jogo j)
{
  j->delta_d = dino_atualiza(&j->rex, j->delta_t, j->up, j->down);
  j->distancia += j->delta_d;
  j->pontos = j->distancia / 4;
}

void jogo_desenha(Jogo j)
{
  if (!j->tem_tela) return;
  b_limpa(j->canvas);
  b_desenha(j->canvas, j->rex.pva.pos.x, j->rex.pva.pos.y, j->rex.imagem);
  for (l_inicio(j->objetos); l_avanca(j->objetos);) {
    Objeto obj = l_item_ptr(j->objetos);
    b_desenha(j->canvas, obj->posicao.x, obj->posicao.y, obj->imagem);
  }
  desenha_numero(j->canvas, 585, 5, j->pontos, 5);
  desenha_numero(j->canvas, 510, 5, j->recorde, 5);
  b_desenha(j->canvas, 430, 5, imagens[IM_HI]);
  if (j->tem_rede_neural) {
    desenha_numero(j->canvas, 585, 20, j->distancia, 5);
    desenha_numero(j->canvas, 585, 35, j->rex.pva.vel.x, 3);
    b_desenha(j->canvas, j->entradas[0]*600-5, 2, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[1]*600-8, 2, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[2]*600-5, 11, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[3]*600-5, 8, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[4]*600-5, 5, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[5]*600-8, 11, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[6]*600-8, 8, imagens[IM_E1]);
    b_desenha(j->canvas, j->entradas[7]*600-8, 5, imagens[IM_E1]);
    desenha_numero(j->canvas, 545, 50, j->entradas[0]*600,3);
    desenha_numero(j->canvas, 585, 50, j->entradas[1]*600,3);
    desenha_numero(j->canvas, 545, 95, j->entradas[2]*600,3);
    desenha_numero(j->canvas, 545, 80, j->entradas[3]*600,3);
    desenha_numero(j->canvas, 545, 65, j->entradas[4]*600,3);
    desenha_numero(j->canvas, 585, 95, j->entradas[5]*600,3);
    desenha_numero(j->canvas, 585, 80, j->entradas[6]*600,3);
    desenha_numero(j->canvas, 585, 65, j->entradas[7]*600,3);
  }

  tela_lincol(1, 1);
  tela_cor_fundo(50,50,50);
  tela_cor_letra(255,125,0);
  b_imprime_uni_2x3(j->canvas);
  //b_imprime_ascii(j->canvas);

  tela_mostra();
}

void jogo_avanca_um_frame(Jogo j)
{
  jogo_calcula_delta_t(j);
  jogo_calcula_entradas(j);
  jogo_move_dino(j);
  jogo_move_objetos(j);
  jogo_remove_objetos(j);
  jogo_cria_objetos(j);
  jogo_verifica_colisao(j);
  jogo_desenha(j);
}

int joga_sem_tela(Rede rede)
{
  Jogo j = jogo_cria(false, rede);
  //Jogo j = jogo_cria(true, rede);
  while (!jogo_termina(j)) {
    jogo_avanca_um_frame(j);
  }
  int pontos = j->pontos;

  jogo_destroi(j);
  return pontos;
}

int joga_com_tela(Rede rede)
{
  Jogo j = jogo_cria(true, rede);
  while (!jogo_termina(j)) {
    jogo_avanca_um_frame(j);
  }
  int pontos = j->pontos;

  jogo_destroi(j);
  return pontos;
}
