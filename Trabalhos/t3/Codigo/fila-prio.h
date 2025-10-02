#ifndef _FILA_PRIO_H_
#define _FILA_PRIO_H_

// fila-prio
// =========
// Fila de prioridade
//
// Uma fila em que os dados têm prioridade entre si, e são removidos
//   de acordo com essa prioridade. Sempre na remoção, é removido o
//   mais prioritário entre os dados contidos na fila.
//
// Esta é uma implementação de fila genérica, a fila não conhece o
//   tipo dos dados que armazena. Na criação da fila, é informado o
//   tamanho em bytes de cada dado, e na inserção é passado um ponteiro
//   para o dado a inserir, que a fila copia para sua memória, de
//   acordo com o número de bytes com que foi configurada.
//   Na remoção também, a fila recebe um ponteiro para onde o dado
//   removido é copiado.
// Como a fila não conhece o tipo de dado, não tem como comparar os
//   dados para saber quem tem mais prioridade. Essa comparação é
//   realizada por uma função, que também é informada para a fila
//   quando ela é criada. Essa função recebe ponteiror para dois dados
//   e retorna um número que diz qual deles tem maior prioridade.
//   A forma de se informar uma função é através de um ponteiro,
//   do tipo "ponteiro para função". No caso, um "ponteiro para função
//   que recebe dois ponteiros para void e retorna um int".
//
// Na implementação fornecida, os dados da fila são colocados em um
//   vetor alocado conforme a necessidade. Na inserção, o dado é colocado
//   no final do vetor. Na remoção, o dado com mais prioridade é
//   buscado linearmente entre todos os dados, é removido e o último
//   dado da fila é colocado no seu lugar.
//
// Seu trabalho é reimplementar a fila como um "heap", mantendo a
//   interface (fila-prio.h não deve ser alterado).

// Tipo de dados para para função de comparação.
// A função recebe ponteiros para dois dados, e retorna um inteiro
//   com o resultado da comparação, que deve ser:
//   - positivo se o primeiro item tem prioridade maior que o segundo,
//   - negativo se o primeiro item tem prioridade menor que o segundo,
//   - zero se os itens têm a mesma prioridade.
typedef int compara_t(void *item1, void *item2);

// tipo de dados da fila, a ser declarado no .c
typedef struct fila_prio *FilaPrio;


// Aloca e retorna uma nova fila, vazia.
// Os dados a serem colocados na fila terão o tamanho "bytes_por_item",
//   e podem ser comparados pela função apontada por "comparador".
FilaPrio fp_cria(int bytes_por_item, compara_t *comparador);

// Libera a memória ocupada pela fila.
void fp_destroi(FilaPrio self);

// Retorna o número de itens atualmente na fila.
int fp_tam(FilaPrio self);

// Insere o item apontado por "dado" na fila "self".
void fp_insere(FilaPrio self, void *dado);

// Remove o próximo item da fila, copiando seu conteúdo para a 
//   região apontada por "dado".
void fp_remove(FilaPrio self, void *dado);

#endif  // _FILA_PRIO_H_
