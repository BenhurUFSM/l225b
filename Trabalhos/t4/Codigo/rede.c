#include "rede.h"
#include "grafo.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct rede {
  Grafo grafo;
  Fila ordem;
};

// função auxiliar para colocar o valor de um neurônio no intervalo (0,1)
static inline float sigmoid(float x)
{
  return 1 / (1 + expf(-x));
}

static void rede_calcula_valores(Rede rede)
{
  // calcula o valor de cada neurônio, na ordem da fila
  // o valor de um neurônio é o somatório do valor de cada neurônio conectado
  //   às suas entradas multiplicado pelo peso da sinapse que faz essa conexão.
  // esse valor depois é limitado pela função sigmoide.
  int no_origem, no_destino;
  neuronio *neuronio_origem, *neuronio_destino;
  sinapse *sina;
  fila_inicia_percurso(rede->ordem, 0);
  while (fila_proximo(rede->ordem, &no_destino)) {
    // o valor das entradas é fornecido, não é calculado
    if (no_destino < N_ENTRADAS) continue;
    neuronio_destino = grafo_no(rede->grafo, no_destino);
    float soma = 0;
    grafo_arestas_que_chegam(rede->grafo, no_destino);
    while ((sina = grafo_proxima_aresta(rede->grafo, &no_origem)) != NULL) {
      if (!sina->habilitada) continue;
      neuronio_origem = grafo_no(rede->grafo, no_origem);
      soma += neuronio_origem->valor * sina->peso;
    }
    neuronio_destino->valor = sigmoid(soma);
  }

}

void rede_calcula_ordem(Rede rede)
{
  if (rede->ordem != NULL) fila_destroi(rede->ordem);
  rede->ordem = grafo_ordem_topologica(rede->grafo);
}

// Cálculo dos valores da rede neural (produz as saídas à partir das entradas)
void rede_calcula(Rede rede, float entradas[N_ENTRADAS], float saidas[N_SAIDAS])
{
  if (rede == NULL) return;
  // copia os valores de entrada para os neurônios de entrada da rede
  // não copia o último, coloca -1
  for (int entrada = 0; entrada < N_ENTRADAS; entrada++) {
    neuronio *neuro;
    neuro = grafo_no(rede->grafo, PRIMEIRO_NO_ENTRADA + entrada);
    neuro->valor = entrada < N_ENTRADAS - 1 ? entradas[entrada] : -1;
  }
  // zera os valores de saída da rede (dessa forma, se algum neurônio de
  //   saída estiver desconectado, não terá um valor não inicializado)
  for (int saida = 0; saida < N_SAIDAS; saida++) {
    neuronio *neuro;
    neuro = grafo_no(rede->grafo, PRIMEIRO_NO_SAIDA + saida);
    neuro->valor = 0;
  }

  rede_calcula_valores(rede);

  // copia os valores dos neurônios de saída para o vetor de saída
  for (int saida = 0; saida < N_SAIDAS; saida++) {
    neuronio *neuro;
    neuro = grafo_no(rede->grafo, PRIMEIRO_NO_SAIDA + saida);
    saidas[saida] = neuro->valor;
  }
}

// cria uma rede vazia
Rede rede_cria_vazia()
{
  Rede self = malloc(sizeof(*self));
  if (self == NULL) return NULL;
  self->grafo = grafo_cria(sizeof(neuronio), sizeof(sinapse));
  if (self->grafo == NULL) {
    free(self);
    return NULL;
  }
  self->ordem = NULL;
  return self;
}

// cria uma rede neural a partir de um arquivo
Rede rede_cria(char *nome)
{
  FILE *arq = fopen(nome, "r");
  if (arq == NULL) return NULL;
  Rede self = rede_cria_vazia();
  if (self == NULL) {
    fclose(arq);
    return NULL;
  }
  int nnos, nentradas, nsaidas;
  if (fscanf(arq, "%d%d%d", &nnos, &nentradas, &nsaidas) != 3) goto err_arq;
  if (nentradas != N_ENTRADAS || nsaidas != N_SAIDAS) goto err_arq;
  for (int i = 0; i < nnos; i++) {
    neuronio neuronio;
    if (fscanf(arq, "%d", &neuronio.id) != 1) goto err_arq;
    neuronio.valor = 0;
    grafo_insere_no(self->grafo, &neuronio);
  }
  for (;;) {
    int no_origem, no_destino, habilitada;
    float peso;
    if (fscanf(arq, "%d%d%f%d", &no_origem, &no_destino, &peso, &habilitada) != 4) goto err_arq;
    if (no_origem == -1) break;
    sinapse sinapse = { peso, habilitada == 1 };
    grafo_altera_aresta(self->grafo, no_origem, no_destino, &sinapse);
  }
  rede_calcula_ordem(self);
  fclose(arq);
  return self;

err_arq:
  fprintf(stderr, "erro na leitura do grafo de '%s'\n", nome);
  fclose(arq);
  grafo_destroi(self->grafo);
  free(self);
  return NULL;
}

// libera a memória ocupada por uma rede neural
void rede_destroi(Rede self)
{
  if (self == NULL) return;
  grafo_destroi(self->grafo);
  if (self->ordem != NULL) fila_destroi(self->ordem);
  free(self);
}

int rede_nneuronios(Rede self)
{
  return grafo_nnos(self->grafo);
}

int rede_insere_neuronio(Rede self, neuronio neuro)
{
  return grafo_insere_no(self->grafo, &neuro);
}

void rede_remove_neuronio(Rede self, int pos)
{
  grafo_remove_no(self->grafo, pos);
}

neuronio *rede_neuronio(Rede self, int pos)
{
  return grafo_no(self->grafo, pos);
}

void rede_remove_sinapse(Rede self, int pos_origem, int pos_destino)
{
  grafo_altera_aresta(self->grafo, pos_origem, pos_destino, NULL);
}

void rede_insere_sinapse(Rede self, int pos_origem, int pos_destino, sinapse sina)
{
  grafo_altera_aresta(self->grafo, pos_origem, pos_destino, &sina);
  if (grafo_tem_ciclo(self->grafo))
    rede_remove_sinapse(self, pos_origem, pos_destino);
}

sinapse *rede_sinapse(Rede self, int pos_org, int pos_dest)
{
  return grafo_aresta(self->grafo, pos_org, pos_dest);
}

void rede_sinapses_que_partem(Rede self, int pos_org)
{
  grafo_arestas_que_partem(self->grafo, pos_org);
}

sinapse *rede_proxima_sinapse(Rede self, int *pos_vizinho)
{
  return grafo_proxima_aresta(self->grafo, pos_vizinho);
}
