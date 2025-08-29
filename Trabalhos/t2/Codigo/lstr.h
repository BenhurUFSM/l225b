#ifndef _LSTR_H_
#define _LSTR_H_

// Lista de strings (lstr)
//
// TAD que implementa uma lista de strings (do tipo str)
//

// declarações {{{1

// lstr é o tipo de dados para nossas listas
// a estrutura lstr é opaca (definida em lstr.c, usuários não têm acesso)
// todas operações são realizadas por referências a lista (por isso o tipo
//   Lstr é um ponteiro para a estrutura da lista)
typedef struct lstr *Lstr;

// todas as operações de lista são implementadas por funções prefixadas por
// `ls_`. O primeiro argumento dessas funções (exceto `ls_cria`) é um ponteiro
// para a lista objeto dessa operação, declarado como `Lstr self`, que foi
// obtido por uma chamada a `ls_cria` e que ainda não foi utilizado em uma
// chamada a `ls_destroi`.
//
// a lista tem o conceito de "posição corrente", que representa a posição do item
// que pode ser acessado (com `ls_item`).
// as operações `ls_avanca` e `ls_recua` alteram essa posição.
// existem duas posições especiais que não correspondem a um item, que são o início
// e o final da lista. Nessas posições não se pode chamar `ls_item`.
// elas são atingidas quando se avança após o último item, quando se recua antes
// do primeiro, quando a lista está vazia, quando se remove o item final da lista.
//
// o percurso de uma lista é tipicamente feito assim:
//   for (ls_inicio(l); ls_avanca(l); ) {
//     dado = ls_item(l);
//     // ...
//   }
//
// quando um item é inserido na lista, a posição corrente passa a ser a posição do
//   item inserido.
// quando um item é removido da lista, a posição corrente passa a ser a posição do
//   item que estava após o item removido.
// a posição corrente pode também ser alterada diretamente por `ls_inicio`, 
//   `ls_final` e `ls_posiciona`.

#include "str.h"

// operações de criação e destruição {{{1

// cria e retorna uma lista vazia
// o ponteiro retornado aponta para uma estrutura recém alocada, que deve
//   ser destruída (com ls_destroi) quando a lista não for mais necessária
Lstr ls_cria();

// destrói uma lista
// essa lista não deve ser utilizada após essa chamada
// esta função destrói a lista, não as strings que ela contém
void ls_destroi(Lstr self);


// operações de acesso {{{1

// retorna true se não houver nenhum elemento na lista
bool ls_vazia(Lstr self);

// retorna o tamanho (número de elementos) da lista
int ls_tam(Lstr self);

// retorna true se existe um item na posição corrente
bool ls_item_valido(Lstr self);

// retorna a string na posição corrente da lista
// essa função não deve ser chamada se a posição corrente estiver antes do
//   início ou depois do final da lista
str ls_item(Lstr self);

// operações de percurso {{{1

// posiciona antes do início da lista
void ls_inicio(Lstr self);

// posiciona após o final da lista
void ls_final(Lstr self);

// altera a posição corrente para a posição do pos-ésimo elemento da lista
// pos pode ser negativo, conta a partir do final da lista
// pos pode representar uma posição após o último ou antes do primeiro item,
//   caso em que altera a posição corrente para uma das posições especiais
void ls_posiciona(Lstr self, int pos);

// avança para a próxima posição da lista
// se a posição corrente for anterior ao início, passa para a posição do
//   primeiro item da lista
// passa para a posição após o final se a posição corrente for a posição
//   do último elemento da lista ou se a lista estiver vazia
// continua na posição após o final se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for após o final da lista)
bool ls_avanca(Lstr self);

// recua para a posição anterior da lista
// se a posição corrente for após o final, passa para a posição do último
//   item da lista 
// passa para a posição antes do início se a posição corrente for a do
//   primeiro elemento da lista ou se a lista estiver vazia
// continua na posição antes do início se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for antes do início da lista)
bool ls_recua(Lstr self);

// operações de alteração da lista {{{1

// insere a string cad antes da posição corrente da lista
// caso a posição corrente seja antes do início, insere no início; caso seja
//   após o fim, insere no fim
// a posição corrente passa a ser a do item inserido
void ls_insere_antes(Lstr self, str cad);

// insere a string cad após a posição corrente da lista
// caso a posição corrente seja antes do início, insere no início; caso seja
//   após o fim, insere no fim
// a posição corrente passa a ser a do item inserido
void ls_insere_depois(Lstr self, str cad);

// remove e retorna a string da posição corrente, que deve ser válida
// a posição corrente passa a ser a do item seguinte ao removido ou após o
//   final se foi removido o item no final da lista
str ls_remove(Lstr self);


// outras operações {{{1

// retorna uma nova lista que é uma cópia dos tam itens de self iniciando na posição
//   corrente
// se tam for 0 ou negativo, ou se a posição corrente da lista for após o final,
//   retorna uma lista vazia
// se não houverem tam itens a partir da posição corrente, a lista retornada
//   conterá menos de tam itens (os tantos que existem a partir da posição corrente)
// a posição corrente de self é alterada para após o último item copiado
// a posição corrente da nova lista é antes do primeiro item
Lstr ls_sublista(Lstr self, int tam);

// retorna uma string (nova, alterável, que deve ser destruída) contendo
//   a concatenação de todas as strings em lista, separadas pela string
//   em separador
// junta(["abacaxi", "abóbora", "abacate"], ", ") -> "abacaxi, abóbora, abacate"
str ls_junta(Lstr self, str separador);

// imprime todos os itens da lista
// após a impressão, a posição corrente pode ser qualquer
void ls_imprime(Lstr self);

#endif // _LSTR_H_
// vim: foldmethod=marker shiftwidth=2
