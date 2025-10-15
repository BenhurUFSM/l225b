#ifndef _FILA_H_
#define _FILA_H_
#include <stdbool.h>

typedef struct fila *Fila;

// funções que implementam as operações básicas de uma fila

// cria uma fila vazia que suporta dados do tamanho fornecido (em bytes)
Fila fila_cria(int tam_do_dado);

// libera a memória ocupada por uma fila
void fila_destroi(Fila self);

// diz se a fila está vazia
bool fila_vazia(Fila self);

// remove o dado no início da fila e, se pdado não for NULL, copia o
//   dado removido para *pdado
void fila_remove(Fila self, void *pdado);

// insere o dado apontado por pdado no final da fila
void fila_insere(Fila self, void *pdado);

// funções que implementam operações complementares, que permitem acesso
//   a todos os elementos da fila. Durante um percurso, a fila não pode ser
//   alterada.

// inicia um percurso aos elementos da fila, a partir de uma posição inicial.
// cada dado da fila, a partir dessa posição será acessado por chamadas a
//   fila_proximo().
// se a posição for positiva, o percurso vai desde essa posição até o fim da fila.
// se a posição for negativa, o percurso vai desde essa posição até o início.
//   0 é a posição do primeiro dado (aquele que está na fila há mais tempo)
//   1 é a posição do segundo dado, etc
//   além disso,
//   -1 é a posição do último dado (o que está na fila há menos tempo)
//   -2 é a posição do penúltimo dado, etc
// se a posição for positiva além do final da fila ou negativa antes
//   do início, o percurso não percorrerá nenhum dado
void fila_inicia_percurso(Fila self, int pos_inicial);

// coloca o próximo dado do percurso em *pdado (se pdado não for NULL),
//   avança o percurso para o dado seguinte e retorna true
// ou, caso o percurso tenha terminado, retorna false
bool fila_proximo(Fila self, void *pdado);

#endif //_FILA_H_
