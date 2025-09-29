#include "fila-prio.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// definição da estrutura que implementa a fila
struct fila_prio {
  int bytes_por_item;
  compara_t comparador;
  int capacidade;
  int num_itens;
  void *dados;
};

FilaPrio fp_cria(int bytes_por_item, compara_t comparador)
{
  FilaPrio self = malloc(sizeof(*self));
  assert(self != NULL);
  self->bytes_por_item = bytes_por_item;
  self->comparador = comparador;
  self->capacidade = 0;
  self->dados = NULL;
  self->num_itens = 0;
  return self;
}

void fp_destroi(FilaPrio self)
{
  if (self != NULL) {
    free(self->dados);
    free(self);
  }
}

int fp_tam(FilaPrio self)
{
  return self->num_itens;
}

// garante que tenha espaço para num_itens na fila
// aumenta ou diminui o espaço alocado, para não faltar nem sobrar demais
static void fp__ajusta_capacidade(FilaPrio self)
{
  if (self->capacidade < self->num_itens) {
    self->capacidade = self->num_itens + self->num_itens / 2;
  } else if (self->capacidade > self->num_itens * 3) {
    self->capacidade = self->num_itens + self->num_itens / 4;
  } else {
    return;
  }
  // aloca um tamanho mínimo
  if (self->capacidade < 4) self->capacidade = 4;
  self->dados = realloc(self->dados,
                        self->capacidade * self->bytes_por_item);
  assert(self->dados != NULL);
}

// retorna ponteiro para o item na posição 'indice'
static void *fp__ponteiro_para_item(FilaPrio self, int indice)
{
  char *ptr = self->dados; // com ponteiro para char, dá para somar em bytes
  ptr += indice * self->bytes_por_item;
  return (void *)ptr;
}

// retorna um ponteiro para o item mais prioritário na fila,
//   de acordo com a função de comparação
static void *fp__acha_item_a_remover(FilaPrio self)
{
  if (self->num_itens == 0) return NULL;
  // chuta que mais prioritário é o primeiro
  void *maior = fp__ponteiro_para_item(self, 0);
  // procura em todos da fila, cada vez que achar um com prioridade
  //   maior, substitui o maior
  for (int i = 1; i < self->num_itens; i++) {
    void *candidato = fp__ponteiro_para_item(self, i);
    if (self->comparador(maior, candidato) < 0) {
      maior = candidato;
    }
  }
  return maior;
}

void fp_insere(FilaPrio self, void *dado)
{
  // aumenta o número de itens
  self->num_itens++;
  // faz caber
  fp__ajusta_capacidade(self);
  void *ptr = fp__ponteiro_para_item(self, self->num_itens - 1);
  // copia o dado fornecido
  memcpy(ptr, dado, self->bytes_por_item);
}

void fp_remove(FilaPrio self, void *dado)
{
  // acha o mais prioritário da fila
  void *item_a_remover = fp__acha_item_a_remover(self);
  if (item_a_remover != NULL) {
    // copia o maior para o usuário
    if (dado != NULL) {
      memcpy(dado, item_a_remover, self->bytes_por_item);
    }
    // move o último da fila para o local que ficou vago
    void *ultimo_item = fp__ponteiro_para_item(self, self->num_itens - 1);
    if (ultimo_item != item_a_remover) {
      memcpy(item_a_remover, ultimo_item, self->bytes_por_item);
    }
    // tem um item a menos, talvez tenha que realocar
    self->num_itens--;
    fp__ajusta_capacidade(self);
  }
}
