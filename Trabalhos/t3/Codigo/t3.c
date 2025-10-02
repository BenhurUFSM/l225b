#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "str.h"
#include "lstr.h"
#include "fila-prio.h"
#include "tela.h"
#include "utf8.h"

// coloca em buf a conversão de uni para utf8 e retorna uma str com isso
str s_uni(byte *buf, unichar uni)
{
  int nbytes = u8_converte_pra_utf8(uni, buf);
  return s_cria_buf(buf, nbytes, 1);
}

// concatena um unichar ao final da string
void s_cat_uni(str *ps, unichar uni)
{
  byte buf[4];
  s_cat(ps, s_uni(buf, uni));
}

// remove o último char da string
void s_del(str *ps)
{
  s_subst(ps, -1, 1, s_(""), s_(""));
}

// esvazia a string apontada por ps
void s_esvazia(str *ps) {
  s_subst(ps, 0, s_tam(*ps), s_(""), s_(""));
}

// que horas são?
double agora()
{
  return tela_relogio();
}

// chuta um número
int aleat_entre(int ini, int fim) {
  return rand() % (fim - ini + 1) + ini;
}

// chuta um número entre 1 e 1000, com mais chances de ser pequeno que grande
int chuta_pontos()
{
  return aleat_entre(1, 10) * aleat_entre(1, 10) * aleat_entre(1, 10);
}

// dado para colocar na fila, com uma palavra e o número de pontos que ela vale
typedef struct {
  str palavra;
  int pontos;
} palpon_t;

// função de comparação para prioridade entre palpons
// considera que quem vale menos pontos tem maior prioridade
int compara_palpon(void *p1, void *p2)
{
  palpon_t *pp1 = p1;
  palpon_t *pp2 = p2;
  return pp2->pontos - pp1->pontos;
}

// estrutura para manter o estado do jogo
typedef struct {
  str conteudo_do_arquivo;
  Lstr palavras;
  FilaPrio fila_de_palavras;
  palpon_t palavra_escolhida;
  str palavra_sendo_digitada;
  int bonus;
  int pontos;
  bool terminou;
  double t_ini_palavra;
  double t_fim_palavra;
  float fracao_tempo_palavra;
  double t_ini_partida;
  double t_fim_partida;
  int badabum;
} *Jogo;

// aloca e inicializa o estado do jogo (mas não de uma partida)
Jogo j_cria(str nome_do_arquivo)
{
  Jogo j = malloc(sizeof(*j));
  assert(j != NULL);
  j->conteudo_do_arquivo = s_le_arquivo(nome_do_arquivo);
  j->palavras = s_separa(j->conteudo_do_arquivo, s_("\n"));
  j->fila_de_palavras = NULL;
  j->palavra_sendo_digitada = s_copia(s_(""));
  return j;
}

// libera a memória ocupada pelo estado do jogo
void j_destroi(Jogo j)
{
  s_destroi(j->conteudo_do_arquivo);
  ls_destroi(j->palavras);
  fp_destroi(j->fila_de_palavras);
  s_destroi(j->palavra_sendo_digitada);
  free(j);
}

// pega a próxima palavra da fila (ou termina se não tem mais)
void j_escolhe_palavra(Jogo j)
{
  if (fp_tam(j->fila_de_palavras) == 0) {
    j->terminou = true;
    return;
  }
  fp_remove(j->fila_de_palavras, &j->palavra_escolhida);
  j->t_ini_palavra = agora();
  j->t_fim_palavra = agora() + s_tam(j->palavra_escolhida.palavra) * 2.0;
  s_esvazia(&j->palavra_sendo_digitada);
}

// insere mais uma palavra na fila (aleatória)
void j_sorteia_palavra(Jogo j)
{
  int p = aleat_entre(0, ls_tam(j->palavras) - 1);
  palpon_t sorteada;
  ls_posiciona(j->palavras, p);
  sorteada.palavra = ls_item(j->palavras);
  sorteada.pontos = chuta_pontos() * s_tam(sorteada.palavra);
  fp_insere(j->fila_de_palavras, &sorteada);
}


// inicia uma partida
void j_inicia(Jogo j)
{
  fp_destroi(j->fila_de_palavras);
  j->fila_de_palavras = fp_cria(sizeof(palpon_t), compara_palpon);
  j->pontos = 0;
  j->terminou = false;
  j->t_ini_partida = agora();
  for (int i = 0; i < 10; i++) {
    j_sorteia_palavra(j);
  }
  j_escolhe_palavra(j);
  j->badabum = 0;
  j->t_fim_partida = agora() + 30;
}

// dependendo do tempo de digitação, pode ganhar bônus (ou malus)
void j_calc_bonus(Jogo j)
{
  int pontos = j->palavra_escolhida.pontos;
  if (j->fracao_tempo_palavra < 0.25) {
    j->bonus = (0.25 - j->fracao_tempo_palavra) * 2 * pontos;
  } else if (j->fracao_tempo_palavra > 0.5) {
    j->bonus = (0.5 - j->fracao_tempo_palavra) * pontos;
  } else {
    j->bonus = 0;
  }

}

// processa coisas relacionadas à passagem do tempo
void j_verifica_tempo(Jogo j)
{
  if (agora() >= j->t_fim_partida) {
    j->terminou = true;
    return;
  }
  if (agora() >= j->t_fim_palavra) {
    j->badabum = 2;
    j_escolhe_palavra(j);
  }
  if (aleat_entre(0, 200) == 0) j_sorteia_palavra(j);
  j->fracao_tempo_palavra = (agora() - j->t_ini_palavra)
                          / (j->t_fim_palavra - j->t_ini_palavra);
  j_calc_bonus(j);
}

// imprime a palavra, com as letras já digitadas em verde ou vermelho
void j_imprime_palavra(Jogo j)
{
  int iguais = 0;
  str esc = j->palavra_escolhida.palavra;
  str dig = j->palavra_sendo_digitada;
  while (iguais < s_tam(esc) && s_igual(s_sub(esc, iguais, 1), s_sub(dig, iguais, 1))) {
    iguais++;
  }
  tela_cor_letra(20, 200, 20);
  s_imprime(s_sub(dig, 0, iguais));
  tela_cor_letra(200, 20, 20);
  s_imprime(s_sub(dig, iguais, s_tam(dig)));
  tela_cor_normal();
  s_imprime(s_sub(esc, s_tam(dig), s_tam(esc)));
}

// desenha a tela do jogo
void j_desenha(Jogo j)
{
  // limpa a tela, colorida se badabum
  if (j->badabum != 0) {
    int c = j->badabum == 2 ? 0 : 255;
    tela_cor_fundo(255, c, c);
    j->badabum--;
  } else {
    tela_cor_normal();
  }
  tela_limpa();

  // imprime a palavra, em uma linha relacionada ao tempo disponível
  tela_lincol(j->fracao_tempo_palavra * (tela_nlin() - 1) + 1, 10);
  j_imprime_palavra(j);

  // imprime a linha com o estado
  tela_lincol(tela_nlin(), 1);
  printf("%d", j->palavra_escolhida.pontos);
  printf("  pontos=%d", j->pontos);
  printf("  bonus=%d", j->bonus);
  printf("  palavras=%d", fp_tam(j->fila_de_palavras));
  double tempo = j->t_fim_partida - agora();
  if (tempo < 5) {
    tela_cor_letra(200, 10, 10);
    printf("  tempo=%.1f", tempo);
  } else {
    printf("  tempo=%.0f", tempo);
  }
}

// lê e processa uma tecla do teclado
void j_teclado(Jogo j)
{
  tecla tec = tela_le_tecla();
  switch (tec) {
    case t_enter:
      if (s_igual(j->palavra_sendo_digitada, j->palavra_escolhida.palavra)) {
        int pontos = j->palavra_escolhida.pontos + j->bonus;
        j->pontos += pontos;
        j->t_fim_partida += pontos / 200.0;
      } else {
        j->badabum = 1;
      }
      j_escolhe_palavra(j);
      break;
    case t_back:
      s_del(&j->palavra_sendo_digitada);
      break;
    default:
      if (tec >= ' ' && tec <= t_ultimo_unicode) {
        s_cat_uni(&j->palavra_sendo_digitada, tec);
      }
  }
}

int main()
{
  srand(time(0));
  tela_cria();
  Jogo jogo = j_cria(s_("palavras"));
  j_inicia(jogo);

  while (!jogo->terminou) {
    j_verifica_tempo(jogo);
    j_desenha(jogo);
    j_teclado(jogo);
  }

  tela_lincol(tela_nlin() - 1, 0);
  printf("Fim do jogo.   Total de pontos:%d   Tecle ESC ", jogo->pontos);
  while (tela_le_tecla() != t_esc)
    ;

  j_destroi(jogo);
  tela_destroi();
}
