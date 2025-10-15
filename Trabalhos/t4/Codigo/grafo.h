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

// cria um grafo vazio que suporta dados do tamanho fornecido (em bytes)
//   nos nós e nas arestas
Grafo grafo_cria(int tam_no, int tam_aresta);

// libera a memória ocupada por um grafo
void grafo_destroi(Grafo self);

// Nós

// insere um nó no grafo, com o dado apontado por pdado
// retorna o número do novo nó
int grafo_insere_no(Grafo self, void *pdado);

// remove um nó do grafo e as arestas incidentes nesse nó
// a identificação dos nós remanescentes é alterada, como se esse nó
//   nunca tivesse existido
void grafo_remove_no(Grafo self, int no);

// altera o valor associado a um nó (copia o valor apontado por pdado
//   para o nó)
void grafo_altera_valor_no(Grafo self, int no, void *pdado);

// coloca em pdado o valor associado a um nó
void grafo_valor_no(Grafo self, int no, void *pdado);

// retorna o número de nós do grafo
int grafo_nnos(Grafo self);

// Arestas

// altera o valor da aresta que interliga o nó origem ao nó destino
//   (copia de *pdado)
// caso a aresta não exista, deve ser criada
// caso pdado seja NULL, a aresta deve ser removida
void grafo_altera_valor_aresta(Grafo self, int origem, int destino, void *pdado);

// coloca em pdado (se não for NULL) o valor associado à aresta, se existir
// retorna true se a aresta entre os nós origem e destino existir, e false
//   se não existir
bool grafo_valor_aresta(Grafo self, int origem, int destino, void *pdado);

// inicia uma consulta a arestas que partem do nó origem
// as próximas chamadas a 'grafo_proxima_aresta' devem retornar os valores
//   correspondentes à cada aresta que parte desse nó
void grafo_arestas_que_partem(Grafo self, int origem);

// inicia uma consulta a arestas que chegam ao nó destino
// as próximas chamadas a 'grafo_proxima_aresta' devem retornar os valores
//   correspondentes à cada aresta que chega nesse nó
void grafo_arestas_que_chegam(Grafo self, int destino);

// retorna a próxima aresta, de acordo com a última consulta iniciada por 
//   'grafo_arestas_que_partem' ou 'grafo_arestas_que_chegam'
// o valor do nó vizinho ao nó da consulta deve ser colocado em 'vizinho'
//   (se não for NULL); o valor associado à aresta deve ser colocado em
//   '*pdado' (se não for NULL) e a função deve retornar true.
// Caso não exista mais aresta que satisfaça a consulta, retorna false.
bool grafo_proxima_aresta(Grafo self, int *vizinho, void *pdado);

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
