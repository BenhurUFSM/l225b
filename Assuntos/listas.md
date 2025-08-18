## Listas

Uma lista é um tipo abstrato de dados que mantém um conjunto de dados, além de manter a ordem relativa entre esses dados.
Um dos dados é o primeiro dado da lista, outro é o segundo etc até o último dado da lista.

Exemplos de listas:
- lista de sentenças em uma função
- lista de strings que formam um texto
- lista de compras
- uma string é uma lista de caracteres
- lista de palavras chutadas em uma partida de wordle
- lista de partidas em um campeonato de xadrez
- lista de jogadores que vão chutar os pênaltis
- lista de alunos aprovados no vestibular (mais conhecida como listão)

### Operações em uma lista

O conjunto de operações abaixo é típico de uma lista:
- obter o número de dados na lista
- obter o dado em determinada posição da lista
- remover o dado em determinada posição da lista
- inserir um dado em determinada posição da lista

Exemplo de interface para um TAD que implementa uma lista de inteiros com as operações acima:
```c
// lista.h
#ifndef _LISTA_H_
#define _LISTA_H_

typedef struct lista *Lista;

Lista lista_cria();
void lista_destroi(Lista self);

int lista_tamanho(Lista self);
int lista_acessa_pos(Lista self, int pos);
int lista_remove_pos(Lista self, int pos); // retorna o dado removido
void lista_insere_pos(Lista self, int pos, int dado);

#endif // _LISTA_H_
```

Pode ficar um pouco mais claro com a definição de um tipo para representar o tipo dos dados com os quais a lista trabalha (além de ficar mais fácil de alterar):
```c
// lista.h
#ifndef _LISTA_H_
#define _LISTA_H_

typedef struct lista *Lista;
typedef int dado_t;

Lista lista_cria();
void lista_destroi(Lista self);

int lista_tamanho(Lista self);
dado_t lista_acessa_pos(Lista self, int pos);
dado_t lista_remove_pos(Lista self, int pos); // retorna o dado removido
void lista_insere_pos(Lista self, int pos, dado_t dado);

#endif // _LISTA_H_
```
Se a lista tem uma capacidade máxima, é comum incluir isso na interface. Por exemplo:
```c
// lista.h
#ifndef _LISTA_H_
#define _LISTA_H_

typedef struct lista *Lista;
typedef int dado_t;

Lista lista_cria(int capacidade);
void lista_destroi(Lista self);

int lista_tamanho(Lista self);
// o bool de retorno serve para informar se a operação foi bem sucedida
bool lista_acessa_pos(Lista self, int pos, dado_t *pdado);
bool lista_remove_pos(Lista self, int pos);
bool lista_insere_pos(Lista self, int pos, dado_t dado);

#endif // _LISTA_H_
```

### Listas restritas (fila, pilha, deque)

Existem listas com restrições nas operações que são permitidas. Essas restrições permitem otimizar a implementação.
Algumas dessas listas restritas são tão comuns que recebem nomes especiais. As restrições limitam as operações de inserção e remoção, de forma que só seja possível inserir ou remover o primeiro ou o último elemento.
Caso só se permita inserir em uma ponta e remover na outra, a lista é chamada de **fila**.
Caso só se permita inserir e remover em uma mesma ponta, a lista é chamada de **pilha**.
Caso só se permita inserir e remover em qualquer das pontas, a lista é chamada de **deque** (de *double ended queue*).

Um exemplo de interface para uma pilha:
```c
// pilha.h
#ifndef _PILHA_H_
#define _PILHA_H_

typedef struct pilha *Pilha;
typedef int dado_t;

Pilha pilha_cria();
void pilha_destroi(Pilha self);

bool pilha_vazia(Pilha self);
dado_t pilha_topo(Pilha self);  // é comum chamar essa operação de "top"
dado_t pilha_remove(Pilha self);  // é comum chamar essa operação de "pop"
void pilha_insere(Pilha self, dado_t dado);  // é comum chamar essa operação de "push"
#endif _PILHA_H_
```
O programa abaixo usa uma pilha como essa para inverter a ordem dos números digitados:
```c
#include <stdio.h>
#include "pilha.h"   // o include usa " para incluir arquivos locais, <> para arquivos do sistema

int main()
{
  Pilha p = pilha_cria();

  printf("Digite uma sequência de números, termine com -1\n");
  while (true) {
    int n;
    scanf("%d", &n);
    if (n == -1) break;
    pilha_insere(p, n);
  }

  while (!pilha_vazia(p)) {
    printf("%d\n", pilha_remove(p));
  }

  pilha_destroi(p);
}
```
Outro exemplo similar, a função abaixo inverte todos os elementos de uma fila recebida como argumento, usando uma pilha auxiliar:
```c
void fila_inverte(Fila f)
{
  Pilha p = pilha_cria();
  while (!fila_vazia(f)) {
    pilha_insere(p, fila_remove(f));
  }
  while (!pilha_vazia(p)) {
    fila_insere(f, pilha_remove(p));
  }
  pilha_destroi(p);
}
```
Se fosse trocado fila por pilha e pilha por fila no exemplo acima, em que ordem ficariam os elementos da pilha recebida?

