#ifndef _REDE_H_
#define _REDE_H_

// Uma rede neural simples
//
// A rede é um grafo de nós (neurônios) interligados por arestas (sinapses).
// Alguns desses nós são nós de entrada, outros são nós de saída; os demais
//   são nós intermediários.
// Cada nó tem um valor associado, que pode ser calculado pela rede ou ser
//   um valor de entrada (no caso dos nós de entrada).
// O último nó de entrada é especial, e tem sempre o valor -1. Logo, N_ENTRADAS
//   deve ser declarado como um além do número de entradas fornecidas pelo modelo.
// Cada aresta tem um valor associado, chamado peso. As arestas são direcionais,
//   ligam um nó de origem a um nó de destino (e não vice-versa).
// O valor de um nó é calculado pelo somatório dos valores de seus nós antecessores
//   multiplicado pelo peso da aresta que o liga a esse antecessor. Para uniformizar
//   esses valores, é usada uma função "sigmoide", que limita o valor ao intervalo
//   (0,1).
// Os valores calculados nos nós de saída são os valores de saída da rede, que são
//   usados como valores de entrada no modelo.
//
// O cálculo do valor de um nó é dependente dos valores dos nós que o precedem, logo
//   necessitamos de uma ordem topológica para realizar esses cálculos.
// Para facilitar a identificação dos nós de entrada e saída do grafo, os nós de
//   entrada são numerados de 0 a N_ENTRADAS-1, os nós de saída entre N_ENTRADAS e 
//   N_ENTRADAS+N_SAIDAS-1 e os nós intermediários à partir de N_ENTRADAS+N_SAIDAS.

// número de nós de entrada e de saída na rede neural
// a última entrada é sempre -1
#define N_ENTRADAS 9
#define N_SAIDAS 2

#define PRIMEIRO_NO_ENTRADA 0
#define PRIMEIRO_NO_SAIDA (0+N_ENTRADAS)
#define PRIMEIRO_NO_INTERMEDIARIO (N_ENTRADAS+N_SAIDAS)

#include "grafo.h"
#include "fila.h"

typedef struct rede *Rede;

// as estruturas de rede, neurônio e sinapse estão abertas aqui para simplificar
//   a implementação do treinador de rede. Essas estruturas não devem ser
//   acessadas diretamente a não ser pelo treinador.

// Rede neural.
// Contém um grafo: nos vértices tem neurônios e nas arestas sinapses.
// Os primeiros N_ENTRADAS nós contêm os neurônios de entrada, os N_SAIDAS
//   seguintes contêm os de saída, os demais contêm os intermediários.
// Contém também uma fila, com os números dos nós na ordem em que devem ser
//   calculados.
// struct rede {
//   Grafo grafo;
//   Fila ordem;
// };

// Um neurônio tem uma identificação e um valor.
typedef struct {
  int id;
  float valor;
} neuronio;

// Uma sinapse contém um peso usado no cálculo do valor do neurônio de chegada,
//   e pode ser desabilitada
typedef struct {
  float peso;
  bool habilitada;
} sinapse;


// cria uma rede neural a partir de um arquivo
Rede rede_cria(char *nome);

// cria uma rede vazia
Rede rede_cria_vazia();

// libera a memória ocupada por uma rede neural
void rede_destroi(Rede self);

// retorna o número de neurônios na rede
int rede_nneuronios(Rede self);

// insere um novo neurônio na rede, retorno sua posição
int rede_insere_neuronio(Rede self, neuronio neuro);

// retorna ponteiro para o neurônio na posição pos
neuronio *rede_neuronio(Rede self, int pos);

// remove o neurônio na posição pos
void rede_remove_neuronio(Rede self, int pos);

// retorna ponteiro para a sinapse entre os neurônios nas posições pos_org e pos_dest
sinapse *rede_sinapse(Rede self, int pos_org, int pos_dest);

// insere a sinapse sina entre os neurônios em pos_origem e pos_destino
void rede_insere_sinapse(Rede self, int pos_origem, int pos_destino, sinapse sina);

// remove a sinapse entre os neurônios em pos_origem e pos_destino
void rede_remove_sinapse(Rede self, int pos_origem, int pos_destino);

// recalcula a ordem topológica de cálculo dos nós
void rede_calcula_ordem(Rede rede);

// recalcula os valores dos nós da rede, à partir dos valores fornecidos
//   para as entradas; copia os valores dos nós de saída para o vetor saidas
void rede_calcula(Rede rede, float entradas[N_ENTRADAS], float saidas[N_SAIDAS]);

// inicia um percurso pelas sinapses que partem do neurônio em pos_org
void rede_sinapses_que_partem(Rede self, int pos_org);

// retorna a próxima sinapse do percurso
sinapse *rede_proxima_sinapse(Rede self, int *pos_vizinho);

// funções que devem ser implementadas para calcular a pontuação de uma rede
// joga uma partida do jogo e retorna a pontuação
int joga_sem_tela(Rede rede);
int joga_com_tela(Rede rede);

#endif // _REDE_H_
