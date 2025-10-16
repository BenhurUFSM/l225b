#include "rede.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// configuração
#define N_GERACOES 5000 // numero de geracoes a executar
#define N_INDIVIDUOS 200 // número de individuos em uma geração
#define PERC_SOBREVIVENTES 30 // percentual de sobreviventes de uma geração para outra
#define MAX_ESPECIES N_INDIVIDUOS // número máximo de espécies em uma geração
#define MAX_IND_ESP (N_INDIVIDUOS / 1) // número máximo de indivíduos em uma espécie
#define MAX_PASSOS 2000 // número máximo de passos em uma simulação
#define N_PARTIDAS 3 // número de partidas que cada indivíduo joga

// probabilidades na mutação
#define PROB_CRIAR_NEURONIO 50//2
#define PROB_CRIAR_SINAPSE 50//5
#define PROB_MUTAR_PESOS 85
#define PROB_MUTAR_PESOS_BAGUALMENTE 50
#define PROB_INV_HABILITACAO 2
#define PROB_MESMA_ESPECIE 50//99

// Um indivíduo contém uma rede neural e mais informações necessárias para executar o 
// algoritno de evolução.
typedef struct {
  Rede rede;                 // a rede neural da criatura
  int geracao;               // em que geração ele foi criado
  int especie;               // a que espécie ele pertence
  float pontuacao_original;  // quantos pontos ele fez no jogo
  float pontuacao;           // pontuação alterada considerando a espécie
} Individuo;

Individuo *individuo_cria()
{
  Individuo *self = malloc(sizeof(*self));
  if (self == NULL) return self;
  self->rede = rede_cria_vazia();
  if (self->rede == NULL) {
    free(self);
    return NULL;
  }
  return self;
}
void individuo_destroi(Individuo *self)
{
  rede_destroi(self->rede);
  free(self);
}

int individuo_nneuronios(Individuo *self)
{
  return rede_nneuronios(self->rede);
}
int individuo_insere_neuronio(Individuo *self, neuronio neuronio)
{
  return rede_insere_neuronio(self->rede, neuronio);
}
void individuo_remove_neuronio(Individuo *self, int pos)
{
  rede_remove_neuronio(self->rede, pos);
}
neuronio *individuo_neuronio(Individuo *self, int pos)
{
  return rede_neuronio(self->rede, pos);
}
sinapse *individuo_sinapse(Individuo *self, int pos_org, int pos_dest)
{
  return rede_sinapse(self->rede, pos_org, pos_dest);
}
void individuo_remove_sinapse(Individuo *self, int pos_org, int pos_dest)
{
  rede_remove_sinapse(self->rede, pos_org, pos_dest);
}
void individuo_sinapses_que_partem(Individuo *self, int pos_org)
{
  rede_sinapses_que_partem(self->rede, pos_org);
}
sinapse *individuo_proxima_sinapse(Individuo *self, int *pos_vizinho)
{
  return rede_proxima_sinapse(self->rede, pos_vizinho);
}
void individuo_insere_sinapse(Individuo *self, int pos_origem, int pos_destino, sinapse sina)
{
  rede_insere_sinapse(self->rede, pos_origem, pos_destino, sina);
}

// grava uma indivíduo em um arquivo, em formato que pode ser lido como Rede
void individuo_grava(Individuo *self, char *nome)
{
  FILE *arq = fopen(nome, "w");
  if (arq == NULL) return;
  int nneuronios = individuo_nneuronios(self);
  fprintf(arq, "%d %d %d\n", nneuronios, N_ENTRADAS, N_SAIDAS);
  for (int pos = 0; pos < nneuronios; pos++) {
    neuronio *neuro;
    neuro = individuo_neuronio(self, pos);
    fprintf(arq, "%d\n", neuro->id);
  }
  for (int pos_origem = 0; pos_origem < nneuronios; pos_origem++) {
    individuo_sinapses_que_partem(self, pos_origem);
    int pos_destino;
    sinapse *sina;
    while ((sina = individuo_proxima_sinapse(self, &pos_destino)) != NULL) {
      fprintf(arq, "%d %d %f %d\n", pos_origem, pos_destino, sina->peso, sina->habilitada ? 1 : 0);
    }
  }
  fprintf(arq, "-1 -1 -1 -1\n");
  fprintf(arq, "%d\n", self->geracao);
  fprintf(arq, "%f\n", self->pontuacao);
  fclose(arq);
}

// Treino da rede neural (método próximo ao NEAT)

// os individuos são divididos em espécies, de acordo com a semelhança de suas
// redes neurais
typedef struct {
  int id;
  Individuo *individuos[MAX_IND_ESP];
  int n_individuos;
  int geracao_criacao;        // em que geração foi criada esta espécie
  int geracao_ultima_melhora; // em que geração foi o último aumento da pontuação máxima
  float pontuacao_media;      // a média da pontuação dos indivíduos atuais da espécie
  float pontuacao_maxima;     // a maior pontuação já obtida por um membro desta espécie
  int n_descendentes;
} Especie;

// a evolução acontece em gerações.
// depois de calculados, os resultados individuais são comparados e os melhores são
//   reproduzidos, misturados, sofrem mutações para compor a geração seguinte
typedef struct {
  int id;
  Individuo *individuos[N_INDIVIDUOS];
  int n_individuos;
  Especie especies[MAX_ESPECIES];
  int n_especies;
  float pontuacao_media;
} Geracao;


// os neurônios recebem uma identificação única global, que permite comparar redes
//   que evoluiram independentemente. No NEAT original, isso é feito para as sinapses,
//   mas é meio caro fazer dessa forma sem complicar o grafo (ou usar outra estrutura)
// esta função serve para numerar os neurônios (um neurônio é criado no meio de dois
//   outros, as id desses dois outros definem a id do novo)
#define MAX_INOVACOES 10000
static int proxima_id_neuronio = PRIMEIRO_NO_INTERMEDIARIO;
struct inov {
  int id_origem;
  int id_destino;
  int id_meio;
} inovacoes[MAX_INOVACOES];
int n_inovacoes = 0;
static int novo_id_neuronio(int id_origem, int id_destino)
{
  // procura a identificação de um neurônio com a mesma origem e destino
  for (int i = 0; i < n_inovacoes; i++) {
    if (inovacoes[i].id_origem == id_origem
        && inovacoes[i].id_destino == id_destino) {
      return inovacoes[i].id_meio;
    }
  }
  // se não achou, cria uma nova id
  if (n_inovacoes < MAX_INOVACOES) {
    inovacoes[n_inovacoes++] = (struct inov){id_origem, id_destino, proxima_id_neuronio};
  } else {
    fprintf(stderr, "Número máximo de inovações excedido\n");
  }
  fprintf(stderr, "inovacoes: %d\n", n_inovacoes);
  return proxima_id_neuronio++;
}

// funções auxiliares para aleatórios

// retorna um aleatório >= 0 e < 1
float al01()
{
  // random() tem 31 bits
  return (random()>>4) / (float)0x8000000;
}

// retorna um número aleatório com distribuição normal, com média e desvio padrão dados
float gauss(float media, float desvio)
{
  float x, y, z;
  x = al01();
  y = al01();
  z = sqrtf(-2 * logf(x)) * cosf(2 * M_PI * y);
  return media + desvio * z;
}

// limita um valor (peso de sinapse ou viés de neurônio) a valores razoáveis
float limita_valor(float valor)
{
  if (valor < -20) return -20;
  if (valor >  20) return  20;
  return valor;
}

// retorna um valor aleatório para ser usado como viés ou peso
float valor_aleatorio()
{
  return limita_valor(gauss(0, 3));//1
}

// altera um viés ou peso somando um valor aleatório
float altera_valor(float valor)
{
  return limita_valor(valor + gauss(0, 1.2));
}

// retorna um inteiro aleatório, que pode ser 0, mas não chega a ser limite
static int aleatorio(int limite) {
  return random() % limite;
}

// retorna um inteiro aleatório, que pode ser 0, mas não chega a ser limite
// quanto mais próximo de 0, maior a chance
// quanto menor o valor da média padrao na chamada a gauss, maior a chance de ser pequeno
static int aleatorio_p(int limite) {
  if (limite < 2) return 0;
  float alp01;
  do { alp01 = fabsf(gauss(0, 1)); } while (alp01 >= 1);
  return alp01 * limite;
}

// retorna um inteiro aleatório, que pode ser min, mas não chega a ser max
static int aleatorio_entre(int min, int max)
{
  return min + aleatorio(max - min);
}

// retorna o maior
int max(int a, int b)
{
  if (a > b) return a;
  return b;
}

// retorna verdadeiro com uma certa probabilidade
static bool probabilidade(int percentual) {
  return aleatorio(100) < percentual;
}

// 50% de chance de ser true
static bool caracoroa() { return probabilidade(50); }

// retorna o número de posição no grafo de um neurônio qualquer da rede
static int pos_neuronio_aleatorio(Individuo *self) {
  return aleatorio(individuo_nneuronios(self));
}

// retorna o número de um nó qualquer da rede, que não contenha um neurônio de saída
static int pos_neuronio_nao_saida(Individuo *self) {
  int pos;
  do {
    pos = pos_neuronio_aleatorio(self);
  } while (pos >= N_ENTRADAS && pos < N_ENTRADAS + N_SAIDAS);
  return pos;
}

// retorna o número de um nó qualquer da rede, que não contenha um neurônio de entrada
static int pos_neuronio_nao_entrada(Individuo *self) {
  return aleatorio_entre(N_ENTRADAS, individuo_nneuronios(self));
}

// retorna o número de um nó qualquer da rede, que contenha um neurônio intermediário
//   (ou -1 se não tem neurônio intermediário nessa rede)
static int pos_neuronio_intermediario(Individuo *self) {
  int nneuronios = individuo_nneuronios(self);
  if (nneuronios <= PRIMEIRO_NO_INTERMEDIARIO) return -1;
  return aleatorio_entre(PRIMEIRO_NO_INTERMEDIARIO, nneuronios);
}



// funções auxiliares para busca por id

static neuronio *individuo_neuronio_com_id(Individuo *self, int neuro_id)
{
  neuronio *neuro;
  int nneuronios = individuo_nneuronios(self);
  for (int pos = 0; pos < nneuronios; pos++) {
    neuro = individuo_neuronio(self, pos);
    if (neuro->id == neuro_id) {
      return neuro;
    }
  }
  return NULL;
}

static sinapse *individuo_sinapse_com_ids(Individuo *self, int id_neuro_origem, int id_neuro_destino)
{
  int pos_origem = -1, pos_destino = -1;
  int nneuronios = individuo_nneuronios(self);
  int pos;
  neuronio *neuro;
  for (pos = 0; pos < nneuronios; pos++) {
    neuro = individuo_neuronio(self, pos);
    if (neuro->id == id_neuro_origem) {
      pos_origem = pos;
      if (pos_destino != -1) break;
    }
    else if (neuro->id == id_neuro_destino) {
      pos_destino = pos;
      if (pos_origem != -1) break;
    }
  }
  if (pos == nneuronios) return NULL;
  return individuo_sinapse(self, pos_origem, pos_destino);
}


// Produção de sinapses novas ou mutadas

// produz uma nova sinapse misturando outras 2
// escolhe aleatoriamente de o peso vem de um ou outro antepassado
sinapse mistura_sinapses(sinapse s1, sinapse s2)
{
  sinapse mistura = {
    .peso = caracoroa() ? s1.peso : s2.peso,
    .habilitada = true
  };
  // se algum dos antepassados é inativo, tem 75% de chance de ser inativo
  if (!s1.habilitada || !s2.habilitada) {
    mistura.habilitada = probabilidade(25);
  }
  if (probabilidade(40)) mistura.peso = (s1.peso + s2.peso) / 2;
  return mistura;
}

// cria um individuo que é resultado da mistura de dois outros
Individuo *mistura_individuos(Individuo *ind1, Individuo *ind2)
{
  // para simplificar a mistura, uma delas é dominante: os neurônios e sinapses
  //   da rede recessiva que não existam na dominante não são herdados
  // a dominante é a de maior pontuação
  Individuo *dominante, *recessivo;
  if (ind1->pontuacao_original < ind2->pontuacao_original) {
    dominante = ind2;
    recessivo = ind1;
  } else {
    dominante = ind1;
    recessivo = ind2;
  }

  Individuo *filhote = individuo_cria();
  assert(filhote != NULL);
  filhote->geracao = max(dominante->geracao, recessivo->geracao) + 1;

  // mistura os neurônios
  int nnos = individuo_nneuronios(dominante);
  for (int no = 0; no < nnos; no++) {
    individuo_insere_neuronio(filhote, *individuo_neuronio(dominante, no));
  }

  // mistura as sinapses
  for (int no_origem = 0; no_origem < nnos; no_origem++) {
    int no_destino;
    sinapse *sinadom, *sinarec, sinafilho;
    neuronio *neuroorg, *neurodest;
    neuroorg = individuo_neuronio(dominante, no_origem);
    individuo_sinapses_que_partem(dominante, no_origem);
    while ((sinadom = individuo_proxima_sinapse(dominante, &no_destino)) != NULL) {
      neurodest = individuo_neuronio(dominante, no_destino);
      sinarec = individuo_sinapse_com_ids(recessivo, neuroorg->id, neurodest->id);
      if (sinarec != NULL) {
        sinafilho = mistura_sinapses(*sinadom, *sinarec);
      } else {
        sinafilho = *sinadom;
      }
      individuo_insere_sinapse(filhote, no_origem, no_destino, sinafilho);
    }
  }

  rede_calcula_ordem(filhote->rede);
  return filhote;
}

// Mutações

// cria uma nova sinapse entre 2 neurônios, sem criar ciclo na rede
// retorna true se criar sinapse
bool mutacao_cria_sinapse(Individuo *self)
{
  int pos_origem = pos_neuronio_nao_saida(self);
  int pos_destino = pos_neuronio_nao_entrada(self);
  sinapse *sina = individuo_sinapse(self, pos_origem, pos_destino);
  if (sina != NULL) {
    // já existe a sinapse -- ativa se estiver inativa
    if (!sina->habilitada) {
      sina->habilitada = true;
    }
  } else {
    // sinapse não existe - cria (mas não vai ser inserida se formar ciclo)
    sinapse nova = { .peso = valor_aleatorio(), .habilitada = true };
    individuo_insere_sinapse(self, pos_origem, pos_destino, nova);
  }
  return false;
}

// remove uma sinapse qualquer
void mutacao_remove_sinapse(Individuo *self)
{
  // tenta achar uma aresta aleatoriamente e remove
  // se não achar uma depois de algumas tentativas, desiste
  for (int tentativa = 0; tentativa < 50; tentativa++) {
    int pos_origem = pos_neuronio_nao_saida(self);
    int pos_destino = pos_neuronio_nao_entrada(self);
    if (individuo_sinapse(self, pos_origem, pos_destino) != NULL) {
      individuo_remove_sinapse(self, pos_origem, pos_destino);
      return;
    }
  }
}

// insere um novo neurônio no meio de uma sinapse
// a sinapse antiga continua existindo, mas é desativada
bool mutacao_divide_sinapse(Individuo *self)
{
  // tenta achar uma aresta aleatoriamente para dividir
  // se não achar uma depois de algumas tentativas, desiste
  for (int tentativa = 0; tentativa < 50; tentativa++) {
    int pos_origem = pos_neuronio_nao_saida(self);
    int pos_destino = pos_neuronio_nao_entrada(self);
    sinapse *sina = individuo_sinapse(self, pos_origem, pos_destino);
    if (sina != NULL) {
      neuronio *neuro_origem, *neuro_destino;
      neuro_origem = individuo_neuronio(self, pos_origem);
      neuro_destino = individuo_neuronio(self, pos_destino);
      int id_do_meio = novo_id_neuronio(neuro_origem->id, neuro_destino->id);
      if (individuo_neuronio_com_id(self, id_do_meio) != NULL) {
        // já tem essa divisão, desiste
        continue;
      }
      // desativa a sinapse original
      if (sina->habilitada) {
        sina->habilitada = false;
      }
      // insere uma sinapse, um neurônio e outra sinapse interligando os 2 neurônios
      neuronio neuro_meio = { .id = id_do_meio, .valor = 0 };
      sinapse sina1 = { .peso = 1, .habilitada = true };
      sinapse sina2 = { .peso = sina->peso, .habilitada = true };
      int pos_meio = individuo_insere_neuronio(self, neuro_meio);
      individuo_insere_sinapse(self, pos_origem, pos_meio, sina1);
      individuo_insere_sinapse(self, pos_meio, pos_destino, sina2);
      return true;
    }
  }
  return false;
}

// remove um neurônio intermediario aleatório
void mutacao_remove_neuronio(Individuo *self)
{
  int pos = pos_neuronio_intermediario(self);
  if (pos != -1) individuo_remove_neuronio(self, pos);
}

// altera os pesos das sinapses da rede
void mutacao_valor_sinapses(Individuo *self)
{
  bool bagual = probabilidade(PROB_MUTAR_PESOS_BAGUALMENTE);
  int prob_relativo = 50, prob_absoluto = 55;
  if (bagual) {
    prob_relativo = 70;
    prob_absoluto = 90;
  }
  int nneuronios = individuo_nneuronios(self);
  for (int pos_origem = 0; pos_origem < nneuronios; pos_origem++) {
    int pos_destino;
    sinapse *sina;
    individuo_sinapses_que_partem(self, pos_origem);
    while ((sina = individuo_proxima_sinapse(self, &pos_destino)) != NULL) {
      int aleat = aleatorio(100);
      if (aleat >= prob_absoluto) continue;
      if (aleat < prob_relativo) {
        sina->peso = altera_valor(sina->peso);
      } else {
        sina->peso = valor_aleatorio();
      }
    }
  }
}


// desativa alguma sinapse ativa ou ativa uma inativa
void mutacao_inverte_habilitacao_sinapse(Individuo *self)
{
  for (int tentativa = 0; tentativa < 50; tentativa++) {
    int pos_origem = pos_neuronio_nao_saida(self);
    int pos_destino = pos_neuronio_nao_entrada(self);
    sinapse *sina = individuo_sinapse(self, pos_origem, pos_destino);
    if (sina == NULL) continue;
    if (sina->habilitada) {
      // não desativa se for a única sinapse ativa saindo do neurônio
      bool achou_vizinho = false;
      int pos_vizinho;
      sinapse *sina_vizinho;
      individuo_sinapses_que_partem(self, pos_origem);
      while ((sina_vizinho = individuo_proxima_sinapse(self, &pos_vizinho)) != NULL) {
        if (pos_vizinho != pos_destino && sina_vizinho->habilitada) {
          achou_vizinho = true;
          break;
        }
      }
      if (!achou_vizinho) continue;
    }
    sina->habilitada = !sina->habilitada;
    break;
  }
}

// faz mutações variadas em um indivíduo
void realiza_mutacoes(Individuo *self)
{
  bool mutacao_estrutural = false;
  if (probabilidade(PROB_CRIAR_NEURONIO)) {
    mutacao_estrutural = mutacao_divide_sinapse(self);
  } else if (probabilidade(PROB_CRIAR_SINAPSE)) {
    mutacao_estrutural = mutacao_cria_sinapse(self);
  } else {
    if (probabilidade(PROB_MUTAR_PESOS)) {
      mutacao_valor_sinapses(self);
    }
    if (probabilidade(PROB_INV_HABILITACAO)) mutacao_inverte_habilitacao_sinapse(self);
  }
  if (mutacao_estrutural) {
    rede_calcula_ordem(self->rede);
  }
}

// cria um indivíduo só com entradas e saídas, conectadas com valores aleatórios
Individuo *individuo_cria_aleatorio()
{
  Individuo *self = individuo_cria();
  if (self == NULL) {
    return NULL;
  }
  self->geracao = 0;
  int nneuronios = N_ENTRADAS + N_SAIDAS;
  for (int pos = 0; pos < nneuronios; pos++) {
    neuronio neuronio = { .id = pos, .valor = 0 };
    individuo_insere_neuronio(self, neuronio);
  }
  rede_calcula_ordem(self->rede);
  return self;
}

// funcoes para comparacao de dados para ordenar com qsort
// qsort ordena dados em um vetor, e envia 2 ponteiros para posicoes nesse vetor, para a funcao
//   dizer se o primeiro deve estar antes (<0) ou depois (>0)

// compara dois indivíduos para ordenação por pontuação, com qsort
int compara_individuos_por_pontuacao(const void *p1, const void *p2)
{
  // p1 e p2 sao ponteiros para posições em um vetor de ponteiros para Individuo
  Individuo *ind1 = *(Individuo **)p1;
  Individuo *ind2 = *(Individuo **)p2;
  return ind2->pontuacao - ind1->pontuacao;
}

// compara duas especies para ordenação por maior pontuação, com qsort
int compara_especies_por_pontuacao(const void *p1, const void *p2)
{
  // p1 e p2 sao ponteiros para posições em um vetor de Especie
  Especie *esp1 = (Especie *)p1;
  Especie *esp2 = (Especie *)p2;
  // compara a pontuacao do primeiro individuo (deve ser o de maior pontuacao da especie)
  return esp2->individuos[0]->pontuacao - esp1->individuos[0]->pontuacao;
}

// compara duas especies para ordenação por maior pontuação original, com qsort
int compara_especies_por_pontuacao_original(const void *p1, const void *p2)
{
  // p1 e p2 sao ponteiros para posições em um vetor de Especie
  Especie *esp1 = (Especie *)p1;
  Especie *esp2 = (Especie *)p2;
  // compara a pontuacao do primeiro individuo (deve ser o de maior pontuacao da especie)
  return esp2->individuos[0]->pontuacao_original - esp1->individuos[0]->pontuacao_original;
}

void poe_na_especie(Especie *especie, Individuo *individuo)
{
  especie->individuos[especie->n_individuos++] = individuo;
  individuo->especie = especie->id;
}

void cria_especie(Geracao *geracao, Individuo *individuo)
{
  static int prox_id = 0;
  if (geracao->n_especies > MAX_ESPECIES) {
    printf("ESPECIES DEMAIS!!\n");
    return;
  }
  int indice = geracao->n_especies++;
  Especie *especie = &geracao->especies[indice];
  especie->id = prox_id++;
  especie->n_individuos = 0;
  especie->pontuacao_maxima = 0;
  especie->geracao_criacao = geracao->id;
  especie->geracao_ultima_melhora = geracao->id;
  //printf("cria especie %d G%d\n", especie->id, geracao->id);
  poe_na_especie(especie, individuo);
}

// compara as redes neuronais de dois indivíduos e diz se são parecidas o
// suficiente para serem considerados da mesma espécie
// verifica as sinapses de a, procura em b sinapses com mesmos neurônios
//   de origem e destino.
// conta quantas não encontra e soma a diferença dos pesos das que encontra.
// calcula o grau de compatibilidade a partir desses dois valores.
float compatibilidade(Individuo *a, Individuo *b)
{
  int nneuronios = individuo_nneuronios(a);
  float soma_diffs_pesos = 0;
  int npesos = 0;
  int ndisjuntos = 0;
  int pos_origem, pos_destino;
  neuronio *neuro_origem, *neuro_destino;
  sinapse *sinapse_a, *sinapse_b;
  for (pos_origem = 0; pos_origem < nneuronios; pos_origem++) {
    neuro_origem = individuo_neuronio(a, pos_origem);
    individuo_sinapses_que_partem(a, pos_origem);
    while ((sinapse_a = individuo_proxima_sinapse(a, &pos_destino)) != NULL) {
      neuro_destino = individuo_neuronio(a, pos_destino);
      sinapse_b = individuo_sinapse_com_ids(b, neuro_origem->id, neuro_destino->id);
      if (sinapse_b != NULL) {
        soma_diffs_pesos += fabsf(sinapse_a->peso - sinapse_b->peso);
        npesos++;
      } else {
        ndisjuntos++;
      }
    }
  }
  // deveria fazer de b para a também...
  float compatibilidade = ndisjuntos + 2 * soma_diffs_pesos / npesos;
  return compatibilidade;
}

bool mesma_especie(Individuo *a, Individuo *b)
{
  return (compatibilidade(a, b) < 7);
}

void geracao_especia_individuo(Geracao *geracao, Individuo *individuo)
{
  bool achou = false;
  for (int e = 0; e < geracao->n_especies; e++) {
    Especie *especie = &geracao->especies[e];
    if (mesma_especie(especie->individuos[0], individuo)) {
      poe_na_especie(especie, individuo);
      achou = true;
      break;
    }
  }
  if (!achou) {
    cria_especie(geracao, individuo);
  }
}

void geracao_insere_individuo_sem_especiar(Geracao *geracao, Individuo *individuo)
{
  geracao->individuos[geracao->n_individuos] = individuo;
  geracao->n_individuos++;
}

void geracao_insere_individuo(Geracao *geracao, Individuo *individuo)
{
  geracao_insere_individuo_sem_especiar(geracao, individuo);
  geracao_especia_individuo(geracao, individuo);
}

// classifica todos os individuos da geracao em especies.
// um individuo pertence a uma especie se for considerado da mesma especie que o primeiro dessa especie
// se nao encontrar especie compativel, cria uma nova especie para esse diferentao
void separa_especies(Geracao *geracao)
{
  qsort(geracao->individuos, N_INDIVIDUOS, sizeof(Rede), compara_individuos_por_pontuacao);
  geracao->n_especies = 0;
  for (int i = 0; i < N_INDIVIDUOS; i++) {
    Individuo *individuo = geracao->individuos[i];
    geracao_especia_individuo(geracao, individuo);
  }
  //printf("geracao %d n_especies %d\n", geracao->id, geracao->n_especies);
}

// copia o primeiro indivíduo de esp_org para uma nova espécie para uma nova geração
void copia_melhor_da_especie(Geracao *geracao, Especie *esp_org)
{
  if (geracao->n_especies > MAX_ESPECIES) {
    printf("ESPECIES DEMAIS!!\n");
    return;
  }
  Individuo *campeao = esp_org->individuos[0];
  //assert(campeao->n_descendentes > 0);
  Especie *especie = &geracao->especies[geracao->n_especies++];
  especie->id = esp_org->id;
  especie->n_individuos = 0;
  especie->pontuacao_maxima = esp_org->pontuacao_maxima;
  especie->geracao_criacao = esp_org->geracao_criacao;
  especie->geracao_ultima_melhora = esp_org->geracao_ultima_melhora;
  Individuo *copia_do_campeao = mistura_individuos(campeao, campeao);
  copia_do_campeao->geracao--;
  poe_na_especie(especie, copia_do_campeao);
  geracao_insere_individuo_sem_especiar(geracao, copia_do_campeao);
  esp_org->n_descendentes--;
}


// cria a nova geração inteira a partir do melhor indivíduo das duas melhores espécies
void reproduz_geracao_estagnada(Geracao *velhos, Geracao *novos)
{
  printf("Estagnou\n");
  Individuo *campeao1, *campeao2;
  campeao1 = velhos->especies[0].individuos[0];
  if (velhos->n_especies > 1 && velhos->especies[1].n_descendentes > 0) {
    campeao2 = velhos->especies[1].individuos[0];
  } else {
    campeao2 = campeao1;
  }
  geracao_insere_individuo(novos, mistura_individuos(campeao1, campeao1));
  geracao_insere_individuo(novos, mistura_individuos(campeao1, campeao2));
  geracao_insere_individuo(novos, mistura_individuos(campeao2, campeao2));
  while (novos->n_individuos < N_INDIVIDUOS) {
    Individuo *filhote;
    if (probabilidade(5)) {
      filhote = mistura_individuos(campeao2, campeao2);
    } else if (probabilidade(10)) {
      filhote = mistura_individuos(campeao1, campeao2);
    } else {
      filhote = mistura_individuos(campeao1, campeao1);
    }
    realiza_mutacoes(filhote);
    realiza_mutacoes(filhote);
    realiza_mutacoes(filhote);
    geracao_insere_individuo(novos, filhote);
  }
}

// cria os individuos de uma nova geracao a partir da geracao precedente
void reproduz_geracao(Geracao *velhos, Geracao *novos)
{
  //fprintf(stderr, "Reproduz G%d ", velhos->id);
  // ordena as espécies por pontuação original
  qsort(velhos->especies, velhos->n_especies, sizeof(Especie),
        compara_especies_por_pontuacao_original);
  novos->n_especies = 0;
  novos->n_individuos = 0;
  novos->id = velhos->id + 1;
  if ((velhos->id - velhos->especies[0].geracao_ultima_melhora) > 100) {
    reproduz_geracao_estagnada(velhos, novos);
    return;
  }
  // mata os piores e copia os campeões de todas as espécies que reproduzem
  for (int e = 0; e < velhos->n_especies; e++) {
    Especie *especie_velha = &velhos->especies[e];
    //fprintf(stderr, "repr. especie %d GC%d desc %d\n", especie_velha->id, especie_velha->geracao_criacao, especie_velha->n_descendentes);
    if (especie_velha->n_descendentes == 0) {
       especie_velha->n_individuos = 0;
       continue;
    }
    // oh, mundo cruel!
    int n_sobreviventes = especie_velha->n_individuos * PERC_SOBREVIVENTES / 100;
    if (n_sobreviventes < 1) n_sobreviventes = 1;
    especie_velha->n_individuos = n_sobreviventes;
    // copia o campeão
    copia_melhor_da_especie(novos, especie_velha);
  }
  // gera o número de descendentes de cada espécie
  for (int e = 0; e < velhos->n_especies; e++) {
    Especie *especie_velha = &velhos->especies[e];
    //fprintf(stderr, "repr2 especie %d desc %d\n", especie_velha->id, especie_velha->n_descendentes);
    for (int i = 0; i < especie_velha->n_descendentes; i++) {
      Individuo *mamae = especie_velha->individuos[aleatorio_p(especie_velha->n_individuos)];
      Individuo *filhote;
      if (especie_velha->n_individuos == 1 || probabilidade(25)) {
        filhote = mistura_individuos(mamae, mamae);
        realiza_mutacoes(filhote);
      } else {
        Individuo *papai;
        if (probabilidade(PROB_MESMA_ESPECIE)) {
          papai = especie_velha->individuos[aleatorio_p(especie_velha->n_individuos)];
        } else {
          Especie *especie_aleatoria;
          for (int tentativas = 0; tentativas < 5; tentativas++) {
            especie_aleatoria = &velhos->especies[aleatorio_p(velhos->n_especies)];
            if (especie_aleatoria == especie_velha || especie_aleatoria->n_individuos == 0) continue;
          }
          if (especie_aleatoria->n_individuos == 0) especie_aleatoria = especie_velha;
          papai = especie_aleatoria->individuos[0];
        }
        filhote = mistura_individuos(mamae, papai);
        if (compatibilidade(mamae, papai) == 0 || probabilidade(80)) {
          realiza_mutacoes(filhote);
        }
      }
      geracao_insere_individuo(novos, filhote);
      //fprintf(stderr, " filho %d foi para esp %d\n", filhote->id, filhote->especie);
    }
  }
}

// calcula a pontuacao de cada individuo de uma geracao, jogando com ele.
void pontua_geracao(Geracao *geracao)
{
  for (int i = 0; i < N_INDIVIDUOS; i++) {
    srand(0);
    Individuo *jogador = geracao->individuos[i];
    float soma = 0;
    for (int ip = 0; ip < N_PARTIDAS; ip++) {
      soma += joga_sem_tela(jogador->rede);
    }
    jogador->pontuacao = soma / N_PARTIDAS;
    jogador->pontuacao_original = jogador->pontuacao;
  }
}

// calcula os valores globais da especie, a partir das pontuacoes de seus individuos
// altera as pontuacoes dos individuos para das mais chance a especies novas, menos a quem nao evolui,
// e reduz as pontuacoes em relacao ao tamanho da especie
// retorna a soma total das pontuacoes dos individuos
float especie_totaliza(Especie *especie, int id_geracao)
{
  // ordena indivíduos por pontuação
  qsort(especie->individuos, especie->n_individuos, sizeof(Rede), compara_individuos_por_pontuacao);
  // vê se a espécie evoluiu
  if (especie->individuos[0]->pontuacao > especie->pontuacao_maxima) {
    especie->pontuacao_maxima = especie->individuos[0]->pontuacao;
    especie->geracao_ultima_melhora = id_geracao;
  }
  // classifica a especie
  bool novinha = (id_geracao - especie->geracao_criacao) < 5;
  bool estagnada = (id_geracao - especie->geracao_ultima_melhora) > 25;
  // recalcula a pontuação de cada indivíduo
  float pontuacao_total = 0;
  for (int i = 0; i < especie->n_individuos; i++) {
    Individuo *individuo = especie->individuos[i];
    individuo->pontuacao_original = individuo->pontuacao;
    // dá uma turbinada nas espécies novas
    if (novinha) individuo->pontuacao *= 2;
    // se a espécie não evolui, detona
    if (estagnada) individuo->pontuacao *= 0.1;
    // divide a pontuação entre os indivíduos
    // (não tenho compreensão total sobre o motivo, mas faz parte do NEAT)
    individuo->pontuacao /= especie->n_individuos;
    pontuacao_total += individuo->pontuacao;
  }
  especie->pontuacao_media = pontuacao_total / especie->n_individuos;
  return pontuacao_total;
}

// calcula o numero de descendentes que se espera que cada individuo tenha,
// retorna o total de descendentes esperados para a especie
int especie_calcula_descendentes(Especie *especie, float pontuacao_media)
{
  float total_descendentes = 0;
  int total_descendentes_int = 0;
  // distribui descendentes entre os indivíduos
  // faz um baile entre int e float para dar números inteiros de descendentes,
  //   mas não perder muitos que a espécie tem direito por arredondamentos.
  // calcula de tras pra diante no vetor para dar as sobras pra quem tem
  //   maior pontuação
  for (int i = especie->n_individuos - 1; i >= 0; i--) {
    Individuo *individuo = especie->individuos[i];
    total_descendentes += individuo->pontuacao / pontuacao_media;
    int n_descendentes_int = total_descendentes - total_descendentes_int;
    total_descendentes_int += n_descendentes_int;
  }
  especie->n_descendentes = total_descendentes_int;
  return total_descendentes_int;
}

// calcula o número de descendentes para cada indivíduo da geração
void calcula_descendentes_por_especie(Geracao *geracao)
{
  float pontuacao_total = 0;
  for (int e = 0; e < geracao->n_especies; e++) {
    pontuacao_total += especie_totaliza(&geracao->especies[e], geracao->id);
  }
  geracao->pontuacao_media = pontuacao_total / N_INDIVIDUOS;
  // ordena as espécies, escolhe o campeão
  qsort(geracao->especies, geracao->n_especies, sizeof(Especie), compara_especies_por_pontuacao);
  // calcular o número de descendentes para cada indivíduo/espécie, e o total
  int n_descendentes = 0;
  for (int e = 0; e < geracao->n_especies; e++) {
    n_descendentes += especie_calcula_descendentes(&geracao->especies[e], geracao->pontuacao_media);
  }
  // entrega as sobras para a espécie campeã (pode ter erro de arredondamento nos cálculos das espécies)
  geracao->especies[0].n_descendentes += N_INDIVIDUOS - n_descendentes;
}

// cria uma geração inicial, totalmente aleatória
void povoa_geracao(Geracao *geracao)
{
  geracao->id = 0;
  for (int i = 0; i < N_INDIVIDUOS; i++) {
    geracao->individuos[i] = individuo_cria_aleatorio();
    realiza_mutacoes(geracao->individuos[i]);
  }
  geracao->n_individuos = N_INDIVIDUOS;
  separa_especies(geracao);
}

void destroi_geracao(Geracao *geracao)
{
  for (int i = 0; i < geracao->n_individuos; i++) {
    individuo_destroi(geracao->individuos[i]);
    geracao->individuos[i] = NULL;
  }
  geracao->n_individuos = 0;
}

// grava todos os individuos da geração em disco
void grava_geracao(Geracao *geracao)
{
  char nome[30];
  for (int i = 0; i < N_INDIVIDUOS / 10; i++) {
    sprintf(nome, "G/%03d-%03d", geracao->id, i);
    individuo_grava(geracao->individuos[i], nome);
  }
}

// roda o algoritmo de evolução baseado no NEAT
void evolui(int n_geracoes)
{
  Geracao geracao[2]; // uma é a geração atual, outra é a próxima
  int atual = 0, prox = 1; // qual é a geração atual, qual é a próxima
  // começa com um povo aleatório
  povoa_geracao(&geracao[atual]);
  for (int i = 0; i < n_geracoes; i++) {
    //fprintf(stderr, "Geracao %03d\n", i);
    pontua_geracao(&geracao[atual]);
    qsort(geracao[atual].individuos, N_INDIVIDUOS, sizeof(Rede), compara_individuos_por_pontuacao);
    grava_geracao(&geracao[atual]);
    printf("geracao %d melhor=%.0f pior=%.0f media=%.0f nespecies=%d\n", i, geracao[atual].individuos[0]->pontuacao, geracao[atual].individuos[N_INDIVIDUOS-1]->pontuacao, geracao[atual].pontuacao_media, geracao[atual].n_especies);
    calcula_descendentes_por_especie(&geracao[atual]);
    reproduz_geracao(&geracao[atual], &geracao[prox]);
    destroi_geracao(&geracao[atual]);
    atual = prox;
    prox = 1 - atual;
  }
  destroi_geracao(&geracao[atual]);
}

int main(int argc, char *argv[])
{
  srandom(time(0));
  if (argc > 1) {
    Rede rede = rede_cria(argv[1]);
    if (rede == NULL) {
      return 1;
    }
    int pontos = joga_com_tela(rede);
    printf("pontos: %d\n", pontos);
  } else {
    evolui(N_GERACOES);
  }
}

