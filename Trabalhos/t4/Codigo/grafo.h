#ifndef _GRAFO_H_
#define _GRAFO_H_
#include <stdbool.h>

#include "fila.h"

typedef struct grafo *Grafo;

// funções que implementam as operações básicas de um grafo
//
// um grafo é constituido por nós e arestas que os interligam.
// existe um valor associado a cada nó e a cada aresta. Esses valores são
//   armazenados pelo grafo, mas o grafo desconhece seu tipo, só conhece 
//   o tamanho.
// os nós são identificados por um inteiro que corresponde à ordem em que
//   são inseridos, com 0 correspondendo ao primeiro.
// as arestas são direcionadas, e são identificadas pelo nó de origem e
//   de destino.

// cria um grafo vazio que suporta dados dos tamanhos fornecidos (em bytes)
//   nos nós e nas arestas
Grafo grafo_cria(int tam_no, int tam_aresta);

// libera a memória ocupada por um grafo
void grafo_destroi(Grafo self);

// Nós

// insere um nó no grafo, com o dado apontado por pdado (copia o conteúdo
//   de *pdado).
// retorna o número do novo nó
int grafo_insere_no(Grafo self, void *pdado);

// remove um nó do grafo e as arestas incidentes nesse nó.
// a identificação dos nós remanescentes é alterada, como se esse nó
//   nunca tivesse existido.
void grafo_remove_no(Grafo self, int no);

// retorna um ponteiro para o valor associado ao nó "no"
void *grafo_no(Grafo self, int no);

// retorna o número de nós do grafo
int grafo_nnos(Grafo self);

// Arestas

// altera o valor da aresta que interliga o nó origem ao nó destino
//   (copia o valor de *pdado)
// caso a aresta não exista, deve ser criada
// caso pdado seja NULL, a aresta deve ser removida
void grafo_altera_aresta(Grafo self, int origem, int destino, void *pdado);

// retorna um ponteiro para o valor associado à aresta entre os nós origem e
//   destino existir, ou NULL se tal aresta não existir
void *grafo_aresta(Grafo self, int origem, int destino);

// inicia uma consulta a arestas que partem do nó origem
// as próximas chamadas a 'grafo_proxima_aresta' devem retornar os valores
//   correspondentes à cada aresta que parte desse nó
void grafo_arestas_que_partem(Grafo self, int origem);

// inicia uma consulta a arestas que chegam ao nó destino
// as próximas chamadas a 'grafo_proxima_aresta' devem retornar os valores
//   correspondentes à cada aresta que chega nesse nó
void grafo_arestas_que_chegam(Grafo self, int destino);

// retorna um ponteiro para o valor associado à próxima aresta.
// a próxima aresta é definida de acordo com a última consulta iniciada por 
//   'grafo_arestas_que_partem' ou 'grafo_arestas_que_chegam'.
// se vizinho não for NULL, recebe o valor do nó vizinho ao nó da consulta.
// caso não exista mais aresta que satisfaça a consulta, retorna NULL.
void *grafo_proxima_aresta(Grafo self, int *vizinho);

// Algoritmos

// retorna true se grafo é cíclico, false caso contrário
bool grafo_tem_ciclo(Grafo self);

// retorna uma fila contendo os números dos nós do grafo em uma ordem em
//   que, se o nó 'a' antecede 'b' na lista, não existe um caminho de 'b'
//   para 'a' no grafo
// deve retornar uma fila vazia caso tal ordem não exista
// quem chama esta função é responsável por destruir a fila.
Fila grafo_ordem_topologica(Grafo self);

#endif //_GRAFO_H_
