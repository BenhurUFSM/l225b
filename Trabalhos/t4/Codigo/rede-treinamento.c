#include "grafo.h"
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
  return grafo_nnos(self->rede->grafo);
}
int individuo_insere_neuronio(Individuo *self, Neuronio neuronio)
{
  return grafo_insere_no(self->rede->grafo, &neuronio);
}
void individuo_remove_neuronio(Individuo *self, int pos)
{
  return grafo_remove_no(self->rede->grafo, pos);
}
void individuo_neuronio(Individuo *self, int pos, Neuronio *pneuro)
{
  grafo_valor_no(self->rede->grafo, pos, pneuro);
}
void individuo_altera_neuronio(Individuo *self, int pos, Neuronio neuronio)
{
  grafo_altera_valor_no(self->rede->grafo, pos, &neuronio);
}
bool individuo_sinapse(Individuo *self, int pos_org, int pos_dest, Sinapse *psina)
{
  return grafo_valor_aresta(self->rede->grafo, pos_org, pos_dest, psina);
}
void individuo_altera_sinapse(Individuo *self, int pos_org, int pos_dest, Sinapse *psina)
{
  grafo_altera_valor_aresta(self->rede->grafo, pos_org, pos_dest, psina);
}
void individuo_remove_sinapse(Individuo *self, int pos_org, int pos_dest)
{
  grafo_altera_valor_aresta(self->rede->grafo, pos_org, pos_dest, NULL);
}
void individuo_sinapses_que_partem(Individuo *self, int pos_org)
{
  grafo_arestas_que_partem(self->rede->grafo, pos_org);
}
void individuo_sinapses_que_chegam(Individuo *self, int pos_dest)
{
  grafo_arestas_que_chegam(self->rede->grafo, pos_dest);
}
bool individuo_proxima_sinapse(Individuo *self, int *pos_vizinho, Sinapse *psina)
{
  return grafo_proxima_aresta(self->rede->grafo, pos_vizinho, psina);
}

// grava uma indivíduo em um arquivo, em formato que pode ser lido como Rede
void individuo_grava(Individuo *self, char *nome)
{
  FILE *arq = fopen(nome, "w");
  if (arq == NULL) return;
  int nneuronios = individuo_nneuronios(self);
  fprintf(arq, "%d %d %d\n", nneuronios, N_ENTRADAS, N_SAIDAS);
  for (int pos = 0; pos < nneuronios; pos++) {
    Neuronio neuronio;
    individuo_neuronio(self, pos, &neuronio);
    fprintf(arq, "%d\n", neuronio.id);
  }
  for (int pos_origem = 0; pos_origem < nneuronios; pos_origem++) {
    individuo_sinapses_que_partem(self, pos_origem);
    int pos_destino;
    Sinapse sinapse;
    while (individuo_proxima_sinapse(self, &pos_destino, &sinapse)) {
      fprintf(arq, "%d %d %f %d\n", pos_origem, pos_destino, sinapse.peso, sinapse.habilitada ? 1 : 0);
    }
  }
  fprintf(arq, "-1 -1 -1 -1\n");
  fprintf(arq, "%d\n", self->geracao);
  fprintf(arq, "%f\n", self->pontuacao);
  int no;
  fila_inicia_percurso(self->rede->ordem, 0);
  while (fila_proximo(self->rede->ordem, &no)) {
    fprintf(arq, " %d", no);
  }
  fprintf(arq, "\n");
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

static bool individuo_neuronio_com_id(Individuo *self, int neuro_id, Neuronio *pneuro)
{
  Neuronio neuro;
  int nneuronios = individuo_nneuronios(self);
  for (int pos = 0; pos < nneuronios; pos++) {
    individuo_neuronio(self, pos, &neuro);
    if (neuro.id == neuro_id) {
      if (pneuro != NULL) *pneuro = neuro;
      return true;
    }
  }
  return false;
}

static bool individuo_sinapse_com_ids(Individuo *self, int id_neuro_origem, int id_neuro_destino, Sinapse *psina)
{
  int pos_origem = -1, pos_destino = -1;
  int nneuronios = individuo_nneuronios(self);
  int pos;
  Neuronio neuro;
  for (pos = 0; pos < nneuronios; pos++) {
    individuo_neuronio(self, pos, &neuro);
    if (neuro.id == id_neuro_origem) {
      pos_origem = pos;
      if (pos_destino != -1) break;
    }
    else if (neuro.id == id_neuro_destino) {
      pos_destino = pos;
      if (pos_origem != -1) break;
    }
  }
  if (pos == nneuronios) return false;
  return individuo_sinapse(self, pos_origem, pos_destino, psina);
}


// Produção de sinapses novas ou mutadas

// produz uma nova sinapse misturando outras 2
// escolhe aleatoriamente de o peso vem de um ou outro antepassado
Sinapse mistura_sinapses(Sinapse s1, Sinapse s2)
{
  Sinapse mistura = {
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
  int nnos = grafo_nnos(dominante->rede->grafo);
  for (int no = 0; no < nnos; no++) {
    Neuronio neuro;
    individuo_neuronio(dominante, no, &neuro);
    grafo_insere_no(filhote->rede->grafo, &neuro);
  }

  // mistura as sinapses
  for (int no_origem = 0; no_origem < nnos; no_origem++) {
    int no_destino;
    Sinapse sinadom, sinarec, sinafilho;
    Neuronio neuroorg, neurodest;
    individuo_neuronio(dominante, no_origem, &neuroorg);
    individuo_sinapses_que_partem(dominante, no_origem);
    while (individuo_proxima_sinapse(dominante, &no_destino, &sinadom)) {
      individuo_neuronio(dominante, no_destino, &neurodest);
      if (individuo_sinapse_com_ids(recessivo, neuroorg.id, neurodest.id, &sinarec)) {
        sinafilho = mistura_sinapses(sinadom, sinarec);
      } else {
        sinafilho = sinadom;
      }
      individuo_altera_sinapse(filhote, no_origem, no_destino, &sinafilho);
    }
  }

  filhote->rede->ordem = grafo_ordem_topologica(filhote->rede->grafo);
  return filhote;
}

// Mutações

// cria uma nova sinapse entre 2 neurônios, sem criar ciclo na rede
// retorna true se criar sinapse
bool mutacao_cria_sinapse(Individuo *self)
{
  int pos_origem = pos_neuronio_nao_saida(self);
  int pos_destino = pos_neuronio_nao_entrada(self);
  Sinapse sina;
  if (individuo_sinapse(self, pos_origem, pos_destino, &sina)) {
    // já existe a sinapse -- ativa se estiver inativa
    if (!sina.habilitada) {
      sina.habilitada = true;
      individuo_altera_sinapse(self, pos_origem, pos_destino, &sina);
    }
  } else {
    // sinapse não existe - cria, mas descria se formar ciclo
    sina.peso = valor_aleatorio();
    sina.habilitada = true;
    individuo_altera_sinapse(self, pos_origem, pos_destino, &sina);
    if (!grafo_tem_ciclo(self->rede->grafo)) return true;
    individuo_remove_sinapse(self, pos_origem, pos_destino);
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
    if (individuo_sinapse(self, pos_origem, pos_destino, NULL)) {
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
    Sinapse sinapse;
    if (individuo_sinapse(self, pos_origem, pos_destino, &sinapse)) {
      Neuronio neuro_origem, neuro_destino;
      individuo_neuronio(self, pos_origem, &neuro_origem);
      individuo_neuronio(self, pos_destino, &neuro_destino);
      int id_do_meio = novo_id_neuronio(neuro_origem.id, neuro_destino.id);
      if (individuo_neuronio_com_id(self, id_do_meio, NULL)) {
        // já tem essa divisão, desiste
        continue;
      }
      // desativa a sinapse original
      if (sinapse.habilitada) {
        sinapse.habilitada = false;
        individuo_altera_sinapse(self, pos_origem, pos_destino, &sinapse);
      }
      // insere uma sinapse, um neurônio e outra sinapse interligando os 2 neurônios
      Neuronio neuro_meio = { .id = id_do_meio, .valor = 0 };
      Sinapse sina1 = { .peso = 1, .habilitada = true };
      Sinapse sina2 = { .peso = sinapse.peso, .habilitada = true };
      int pos_meio = individuo_insere_neuronio(self, neuro_meio);
      individuo_altera_sinapse(self, pos_origem, pos_meio, &sina1);
      individuo_altera_sinapse(self, pos_meio, pos_destino, &sina2);
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
    Sinapse sinapse;
    individuo_sinapses_que_partem(self, pos_origem);
    while (individuo_proxima_sinapse(self, &pos_destino, &sinapse)) {
      int aleat = aleatorio(100);
      if (aleat >= prob_absoluto) continue;
      if (individuo_sinapse(self, pos_origem, pos_destino, &sinapse)) {
        if (aleat < prob_relativo) {
          sinapse.peso = altera_valor(sinapse.peso);
        } else {
          sinapse.peso = valor_aleatorio();
        }
        individuo_altera_sinapse(self, pos_origem, pos_destino, &sinapse);
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
    Sinapse sinapse;
    if (individuo_sinapse(self, pos_origem, pos_destino, &sinapse)) {
      if (sinapse.habilitada) {
        // não desativa se for a única sinapse ativa saindo do neurônio
        bool achou_vizinho = false;
        int pos_vizinho;
        Sinapse sina_vizinho;
        individuo_sinapses_que_partem(self, pos_origem);
        while (individuo_proxima_sinapse(self, &pos_vizinho, &sina_vizinho)) {
          if (pos_vizinho != pos_destino && sina_vizinho.habilitada) {
            achou_vizinho = true;
            break;
          }
        }
        if (!achou_vizinho) break;
      }
      sinapse.habilitada = !sinapse.habilitada;
      individuo_altera_sinapse(self, pos_origem, pos_destino, &sinapse);
      break;
    }
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
    if (self->rede->ordem != NULL) fila_destroi(self->rede->ordem);
    self->rede->ordem = grafo_ordem_topologica(self->rede->grafo);
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
    Neuronio neuronio = { .id = pos, .valor = 0 };
    individuo_insere_neuronio(self, neuronio);
  }
  /*
  for (int pos_origem = 0; pos_origem < N_ENTRADAS; pos_origem++) {
    for (int pos_destino = N_ENTRADAS; pos_destino < N_ENTRADAS + N_SAIDAS; pos_destino++) {
      Sinapse sinapse = { .peso = valor_aleatorio(), .habilitada = true };
      individuo_altera_sinapse(self, pos_origem, pos_destino, &sinapse);
    }
  }
*/
  self->rede->ordem = grafo_ordem_topologica(self->rede->grafo);
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
  Neuronio neuro_origem, neuro_destino;
  Sinapse sinapse_a, sinapse_b;
  for (pos_origem = 0; pos_origem < nneuronios; pos_origem++) {
    individuo_neuronio(a, pos_origem, &neuro_origem);
    individuo_sinapses_que_partem(a, pos_origem);
    while (individuo_proxima_sinapse(a, &pos_destino, &sinapse_a)) {
      individuo_neuronio(a, pos_destino, &neuro_destino);
      if (individuo_sinapse_com_ids(b, neuro_origem.id, neuro_destino.id, &sinapse_b)) {
        soma_diffs_pesos += fabsf(sinapse_a.peso - sinapse_b.peso);
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
    int pmax = 0;
    int pmin = 10000;
    int psom = 0;
    int p[5];
    for (int ip = 0; ip < 5; ip++) {
      p[ip] = joga_sem_tela(jogador->rede);
      psom += p[ip];
      if (p[ip] > pmax) pmax = p[ip];
      if (p[ip] < pmin) pmin = p[ip];
    }
    printf("[%d %d %d]", i, pmin, pmax);
    fflush(stdout);
    jogador->pontuacao = psom;
    jogador->pontuacao_original = jogador->pontuacao;
    //fprintf(stderr, "I%03d %f\n", i, jogador->pontuacao);
  }
  printf("\n");
}

// calcula os valores globais da especie, a partir das pontuacoes de seus individuos
// altera as pontuacoes dos individuos para das mais chance a especies novas, menos a quem nao evolui,
// e reduz as pontuacoes em relacao ao tamanho da especie
// retorna a soma total das pontuacoes dos individuos
float especie_totaliza(Especie *especie, int id_geracao)
{
  // ordena indivíduos por pontuação
  //fprintf(stderr, "\ntotaliza esp%d ger_cria%d ger_mel%d ger_at%d\n", especie->id, especie->geracao_criacao, especie->geracao_ultima_melhora, id_geracao);
  qsort(especie->individuos, especie->n_individuos, sizeof(Rede), compara_individuos_por_pontuacao);
  // vê se a espécie evoluiu
  if (especie->individuos[0]->pontuacao > especie->pontuacao_maxima) {
    //printf("especie %d evoluiu ind%d %.0f>%.0f\n", especie->id, especie->individuos[0]->id, especie->pontuacao_maxima, especie->individuos[0]->pontuacao);
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
    //fprintf(stderr, "i%dp%.1f>%.1f ", individuo->id, individuo->pontuacao_original, individuo->pontuacao);
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
  //   mas não perder muitos que a espécie tem direito por arredendamentos.
  // calcula de tras pra diante no vetor para dar as sobras pra quem tem
  //   maior pontuação
  //fprintf(stderr, "desc %d:", especie->id);
  for (int i = especie->n_individuos - 1; i >= 0; i--) {
    Individuo *individuo = especie->individuos[i];
    total_descendentes += individuo->pontuacao / pontuacao_media;
    int n_descendentes_int = total_descendentes - total_descendentes_int;
    //fprintf(stderr, " %d", individuo->n_descendentes);
    total_descendentes_int += n_descendentes_int;
  }
  //fprintf(stderr, "tot %d\n", total_descendentes_int);
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
    //fprintf(stderr, "calc e%d nd%d ", geracao->especies[e].id, n_descendentes);
    n_descendentes += especie_calcula_descendentes(&geracao->especies[e], geracao->pontuacao_media);
  }
  // entrega as sobras para a espécie campeã (pode ter erro de arredondamento nos cálculos das espécies)
  //fprintf(stderr, "sobra %d\n", N_INDIVIDUOS - n_descendentes);
  geracao->especies[0].n_descendentes += N_INDIVIDUOS - n_descendentes;
}

// cria uma geração inicial, totalmente aleatória
void povoa_geracao(Geracao *geracao)
{
  geracao->id = 0;
  for (int i = 0; i < N_INDIVIDUOS; i++) {
    geracao->individuos[i] = individuo_cria_aleatorio();
    //rede_faz_mutacao(geracao->individuos[i]);
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

