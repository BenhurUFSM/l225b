#ifndef _LISTA_H_
#define _LISTA_H_

// Lista genérica
//
// TAD que implementa uma lista
//

// declarações {{{1

#include <stdbool.h>

// Lista é o tipo de dados para nossas listas
// a estrutura lista é opaca (definida em lista.c, usuários não têm acesso)
// todas operações são realizadas por referências a lista (por isso o tipo
//   Lista é um ponteiro para a estrutura da lista)
typedef struct lista *Lista;

// todas as operações de lista são implementadas por funções prefixadas por
// `l_`. O primeiro argumento dessas funções (exceto `l_cria`) é um ponteiro
// para a lista objeto dessa operação, declarado como `Lista self`, que foi
// obtido por uma chamada a `l_cria` e que ainda não foi utilizado em uma
// chamada a `l_destroi`.
//
// a lista tem o conceito de "posição corrente", que representa a posição do item
// que pode ser acessado (com `l_item` e `l_item_ptr`).
// as operações `l_avanca` e `l_recua` alteram essa posição.
// existem duas posições especiais que não correspondem a um item, que são o início
// e o final da lista. Nessas posições não se pode chamar `l_item`.
// elas são atingidas quando se avança após o último item, quando se recua antes
// do primeiro, quando a lista está vazia, quando se remove o item final da lista.
//
// o percurso de uma lista é tipicamente feito assim:
//   for (l_inicio(l); l_avanca(l); ) {
//     l_item(l, &dado);
//     // ...
//   }
//
// quando um item é inserido na lista, a posição corrente passa a ser a posição do
//   item inserido.
// quando um item é removido da lista, a posição corrente passa a ser a posição do
//   item que estava após o item removido.
// a posição corrente pode também ser alterada diretamente por `l_inicio`, 
//   `l_final` e `l_posiciona`.


// operações gerais {{{1

// cria e retorna uma lista vazia, que irá armazenar dados do tamanho informado
// o ponteiro retornado aponta para uma estrutura recém alocada, que deve
//   ser destruída (com l_destroi) quando a lista não for mais necessária
Lista l_cria(int tamanho_de_um_item);

// destrói uma lista
// essa lista não deve ser utilizada após essa chamada
// se a lista não estiver vazia, a memória com os dados que ela contém é liberada,
//   e os dados são perdidos
void l_destroi(Lista self);

// retorna true se não houver nenhum elemento na lista
bool l_vazia(Lista self);

// retorna o tamanho (número de elementos) da lista
int l_tam(Lista self);


// operações de acesso ao item corrente {{{1

// retorna true se existe um item na posição corrente
bool l_item_valido(Lista self);

// copia o item na posição corrente da lista para o endereço item
// retorna false e não copia nada se a posição corrente não corresponder
//   a um item válido (se estiver antes do início ou após o final)
bool l_item(Lista self, void *item);

// retorna um ponteiro para o item que está na posição corrente da lista
// retorna NULL se a posição corrente não corresponder a um item válido
//   (se estiver antes do início ou após o final)
void *l_item_ptr(Lista self);

// operações de percurso {{{1

// posiciona antes do início da lista
void l_inicio(Lista self);

// posiciona após o final da lista
void l_final(Lista self);

// altera a posição corrente para a posição do pos-ésimo elemento da lista
// pos pode ser negativo, conta a partir do final da lista
// pos pode representar uma posição após o último ou antes do primeiro item,
//   caso em que altera a posição corrente para uma das posições especiais
// retorna true se a posição resultante corresponder a um item válido ou 
//   false caso seja uma das posições especiais
bool l_posiciona(Lista self, int pos);

// avança para a próxima posição da lista
// se a posição corrente for anterior ao início, passa para a posição do
//   primeiro item da lista
// passa para a posição após o final se a posição corrente for a posição
//   do último elemento da lista ou se a lista estiver vazia
// continua na posição após o final se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for após o final da lista)
bool l_avanca(Lista self);

// recua para a posição anterior da lista
// se a posição corrente for após o final, passa para a posição do último
//   item da lista 
// passa para a posição antes do início se a posição corrente for a do
//   primeiro elemento da lista ou se a lista estiver vazia
// continua na posição antes do início se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for antes do início da lista)
bool l_recua(Lista self);

// operações de alteração da lista {{{1

// insere o dado apontado por item antes da posição corrente da lista
// caso a posição corrente seja antes do início, o item inserido será
//   o novo primeiro elemento da lista
// caso a posição corrente seja após o fim, o item inserido será o novo
//   último elemento da lista
// a posição corrente passa a ser a do item inserido
void l_insere_antes(Lista self, void *item);

// insere o dado apontado por item após a posição corrente da lista
// caso a posição corrente seja antes do início, o item inserido será
//   o novo primeiro elemento da lista
// caso a posição corrente seja após o fim, o item inserido será o novo
//   último elemento da lista
// a posição corrente passa a ser a do item inserido
void l_insere_depois(Lista self, void *item);

// se a posição corrente da lista contiver um item válido, copia o conteúdo
//   desse item para a região apontada por item, remove o item da lista e
//   retorna true
// senão, retorna false
bool l_remove(Lista self, void *item);

#endif // _LISTA_H_
// vim: foldmethod=marker shiftwidth=2
