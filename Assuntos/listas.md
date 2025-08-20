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

### Implementação de listas

Uma lista pode ser implementada de diversas formas. É comum classificar essas formas de acordo como a memória é alocada e como a ordem dos dados é implementada. A alocação pode ser estática ou dinâmica, a ordem dos dados pode ser mantida por contiguidade ou por encadeamento.

#### Alocação contígua

Os dados são mantidos em uma região contígua de memória, em um vetor ou em uma região alocada dinamicamente (que pode ser tratada como um vetor).
O primeiro elemento da lista é colocado na primeira posição desse vetor, o segundo na segunda posição etc.
O número de elementos que se pode colocar na lista corresponde ao tamanho do vetor.
Para se saber quantos elementos existem na lista (que geralmente é inferior ao tamanho do vetor), em geral se usa ou um elemento demarcador de fim (como em uma string em C) ou uma variável auxiliar que contém o número de elementos (dentro de uma struct que descreve a lista).
Na maioria dos casos se usa a segunda opção, por ser mais econômica (é necessário um inteiro e não o espaço de um item) e mais rápida (a operação de se obter o número de elementos na lista é mais simples).

A implementação das operações básicas:
- o número de elementos é obtido diretamente, já que tem uma variável com essa informação
- a obtenção do dado em uma posição da lista é realizada com a operação a acesso a elemento do vetor nessa mesma posição
- a remoção de um dado em determinada posição implica no deslocamento físico de todos os dados da lista, desde a posição seguinte até o final da lista; a remoção do último elemento não necessita movimentação alguma, mas a do primeiro necessita movimentação de todos os dados restantes na lista
- a inserção de um elemento em uma posição da lista necessita da movimentação de todos os dados da lista a partir dessa posição até o final, para gerar espaço para o elemento inserido; a inserção no final da lista não necessita de movimentação, mas no início implica na movimentação da lista inteira

Esse tipo de lista pode também ser implementado com alocação dinâmica, sem se limitar o número máximo de itens que a lista pode conter.
Nesse caso, em uma inserção em uma lista que já ocupa toda a memória disponível, aloca-se uma região maior, copia-se o conteúdo da lista para a nova região e libera-se a região anterior. Deve ser mantida uma nova variável com a capacidade atual da memória alocada para a lista.
No caso de ser ter uma lista em que o número de elementos pode diminuir bastante depois de ter sido grande, para não se manter o consumo de memória muito alto, a operação de remoção pode também realocar a memória para uma capacidade inferior quando a capacidade e a utilização forem muito diferentes.

Uma pilha pode ser implementada dessa forma a um custo baixo, porque as operações principais, de remoção e inserção podem ser realizadas sem movimentação de dados, se não houver necessidade de realocar o vetor.

Já a implementação de uma fila terá uma das operações principais (inserção ou remoção) com baixo custo, mas a outra necessitará a movimentação de todos os dados presentes na fila.

##### Implementação circular

Uma outra forma semelhante de se implementar uma lista contígua é em uma implementação **circular**, onde o primeiro elemento da lista não necessariamente é colocado na primeira posição do vetor (mas os elementos continuam contíguos em memória, o segundo elemento está logo depois do primeiro etc).
Uma nova variável é necessária, para informar em que posição do vetor está o primeiro elemento da lista.
Uma remoção do primeiro elemento da lista implica somente na alteração da variável que diz onde está o primeiro elemento, não necessita deslocar fisicamente os elementos dentro do vetor (claro, tem que alterar também a informação sobre o número de elementos, como antes).
O espaço não usado no início do vetor é usado quando a posição final do vetor já está usada e há uma nova inserção -- o elemento após o que está na última posição do vetor é colocado na primeira posição.
Por exemplo, as variáveis representadas abaixo armazenam uma lista com os valores `3 8 9`, os valores 1 e 2 no vetor não estão na lista. `cap` é a capacidade (quantos elementos cabem no vetor), `pri` é o índice no vetor onde está o primeiro elemento da lista, `num` é o número de elementos na lista, `vet` é o vetor.
```
   cap 5
   pri 3
   num 3
   vet 9 1 2 3 8
```
A inserção de um valor no início dessa lista pode ser colocado sobre o valor `2`, sem necessitar deslocar nenhum dado; a inserção no final da lista pode ser colocado sobre o `1`, sem necessitar deslocar. 
A inserção do valor 4 no início da lista acima (que passa a conter os valores `4 3 8 9`) resultaria em:
```
   cap 5
   pri 2
   num 4
   vet 9 1 4 3 8
```
A inserção do valor 0 no final da lista original (que passa a conter os valores `3 8 9 0`) resultaria em:
```
   cap 5
   pri 3
   num 4
   vet 9 0 2 3 8
```
A remoção do valor inicial dessa última lista (que passa a conter os valores `8 9 0`) resultaria em:
```
   cap 5
   pri 4
   num 3
   vet 9 0 2 3 8
```

As 4 operações de inserção e remoção nas extremidades da lista são implementadas de forma simples, e seu tempo de execução independe do número de elementos contidos na lista.
A inserção em alguma outra posição irá implicar em deslocamento de dados na memória, mas pode-se escolher deslocar o início da lista para posições anteriores do vetor ou o final da lista para posições posteriores, o que for menor.

Para encontrar a posição `pv` no vetor onde está o item na posição `pl` da lista, faz-se `pv = (pri+pl)%cap`.

A alocação dinâmica de memória para um vetor circular é um pouco mais trabalhosa, porque tem que deslocar as duas porções da lista para regiões diferentes do novo espaço.

Por exemplo, se se duplicasse a memória da última versão da lista acima, para que ela continuasse corretamente possuindo os mesmos elementos, poderia ficar assim: (`x` é usado para indicar posições alocadas e não inicializadas)
```
   cap 10
   pri 9
   num 3
   vet 9 0 2 3 8 x x x x 8
```
outra possibilidade:
```
   cap 10
   pri 4
   num 3
   vet 9 0 2 3 8 9 0 x x x
```

Em outro exemplo de realocação, considere a lista abaixo, que está cheia, com os valores [4 5 1 2 3].
```
   cap 5
   pri 3
   num 5
   vet 1 2 3 4 5
       2 3 4 0 1 (índices na lista)
```
Após uma realocação que aumenta a capacidade para 10, ele pode ficar em uma das configurações abaixo, ainda com os mesmos valores [4 5 1 2 3]. A segunda configuração é melhor, porque envolve a cópia de 2 dados (iniciais, 4 e 5) em vez de 3 (finais, 1, 2 e 3) da primeira.
```
   cap 10
   pri 3
   num 5
   vet 1 2 3 4 5 1 2 3 x x
             0 1 2 3 4     (índices na lista)
```
```
   cap 10
   pri 8
   num 5
   vet 1 2 3 4 5 x x x 4 5
       2 3 4           0 1 (índices na lista)
```

