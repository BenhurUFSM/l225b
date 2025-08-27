// Lista de strings (lstr)
//
// TAD que implementa uma lista de strings (do tipo str)
//

// declarações {{{1

#include "lstr.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct lstr {
  int tam;
  int cap;
  str *vet;
  int cur;
};

// testes simples de integridade da lista
static void ls_ok(Lstr self)
{
  assert(self != NULL);
  assert(self->tam <= self->cap);
  assert(self->cur >= -1);
  assert(self->cur <= self->tam);
  assert(self->cap == 0 || self->vet != NULL);
}

Lstr ls_cria()
{
  Lstr self = malloc(sizeof(struct lstr));
  if (self == NULL) return NULL;

  self->tam = 0;
  self->cap = 100;
  self->cur = -1;
  self->vet = malloc(self->cap * sizeof(str));
  if (self->vet == NULL) {
    free(self);
    return NULL;
  }
  ls_ok(self);
  return self;
}

// destrói uma lista
// essa cadeia não deve ser utilizada após essa chamada
// essa função deve liberar a memória em cadeias alteráveis
// essa função não faz nada com cadeias não alteráveis (não precisa destruir
//   cadeias não alteráveis)
void ls_destroi(Lstr self)
{
  ls_ok(self);
  free(self->vet);
  free(self);
}

// operações de acesso {{{1

bool ls_vazia(Lstr self)
{
  ls_ok(self);
  return ls_tam(self) == 0;
}

int ls_tam(Lstr self)
{
  ls_ok(self);
  return self->tam;
}

bool ls_item_valido(Lstr self)
{
  ls_ok(self);
  return self->cur >= 0 && self->cur < self->tam;
}

str ls_item(Lstr self)
{
  ls_ok(self);
  assert(ls_item_valido(self));
  return self->vet[self->cur];
}

// operações de percurso {{{1

void ls_inicio(Lstr self)
{
  ls_ok(self);
  self->cur = -1;
}

void ls_final(Lstr self)
{
  ls_ok(self);
  self->cur = self->tam;
}

void ls_posiciona(Lstr self, int pos)
{
  ls_ok(self);
  if (pos < 0) pos += self->tam;
  if (pos < 0) pos = -1;
  else if (pos > self->tam) pos = self->tam;

  self->cur = pos;
}

bool ls_avanca(Lstr self)
{
  ls_ok(self);
  if (self->cur < self->tam) self->cur++;
  return ls_item_valido(self);
}

bool ls_recua(Lstr self)
{
  ls_ok(self);
  if (self->cur >= 0) self->cur--;
  return ls_item_valido(self);
}

// operações de alteração da lista {{{1

static void ls_insere_pos_sm(Lstr self, int pos, str cad)
{
  memmove(&self->vet[pos+1], &self->vet[pos], sizeof(self->vet[0]) * (self->tam - pos));
  self->vet[pos] = cad;
  self->tam++;
  self->cur = pos;
}

void ls_insere_antes(Lstr self, str cad)
{
  ls_ok(self);
  int pos = self->cur;
  if (pos < 0) pos = 0;
  if (self->cap <= self->tam) {
    self->cap += self->cap / 2;
    self->vet = realloc(self->vet, self->cap * sizeof(str));
    printf("cap:%d %ld %ld\n", self->cap, self->cap*sizeof(str), (char *)self->vet - (char *)self);
  }
  assert(pos < self->cap);
  ls_insere_pos_sm(self, pos, cad);
}

void ls_insere_depois(Lstr self, str cad)
{
  ls_ok(self);
  int pos = self->cur + 1;
  if (pos > self->tam) pos = self->tam;
  if (self->cap <= self->tam) {
    self->cap += self->cap / 2;
    self->vet = realloc(self->vet, self->cap * sizeof(str));
    printf("cap:%d %ld %ld\n", self->cap, self->cap*sizeof(str), (char *)self->vet - (char *)self);
  }
  assert(pos < self->cap);
  ls_insere_pos_sm(self, pos, cad);
}

str ls_remove(Lstr self)
{
  str item = ls_item(self);
  int pos = self->cur;
  self->tam--;
  memmove(&self->vet[pos], &self->vet[pos+1], sizeof(self->vet[0]) * (self->tam - pos));
  return item;
}

// vim: foldmethod=marker shiftwidth=2
// outras operações {{{1

Lstr ls_sublista(Lstr self, int tam)
{
  Lstr nova = ls_cria();
  assert(nova != NULL);
  int pos = self->cur;
  if (!ls_item_valido(self)) ls_avanca(self);
  for (int i = 0; i < tam; i++) {
    if (!ls_item_valido(self)) break;
    ls_insere_depois(nova, ls_item(self));
    ls_avanca(self);
  }
  self->cur = pos;
  return nova;
}

// retorna uma string (nova, alterável, que deve ser destruída) contendo
//   a concatenação de todas as strings em lista, separadas pela string
//   em separador
// junta(["abacaxi", "abóbora", "abacate"], ", ") -> "abacaxi, abóbora, abacate"
str ls_junta(Lstr self, str separador)
{
  str s = s_copia(s_(""));
  bool primeiro = true;
  for (ls_inicio(self); ls_avanca(self); ) {
    if (!primeiro) s_cat(&s, separador);
    primeiro = false;
    s_cat(&s, ls_item(self));
  }
  return s;
}

// imprime todos os itens da lista
// após a impressão, o item corrente pode ser qualquer
void ls_imprime(Lstr self)
{
  printf("[");
  bool primeiro = true;
  for (ls_inicio(self); ls_avanca(self); ) {
    if (!primeiro) printf(", ");
    primeiro = false;
    s_imprime(ls_item(self));
  }
  printf("]");
}
// vim: foldmethod=marker shiftwidth=2
