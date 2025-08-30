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

### Listas encadeadas

#### Listas encadeadas com alocação contígua

Nas implementações contíguas, a ordem física em que os dados da lista são mantidos em memória corresponde à ordem lógica desses dados na lista.
Sabendo a posição de um dos dados, com um cálculo simples se obtém a posição de qualquer outro.
Em compensação, se um dado muda sua posição lógica na lista (devido a inclusões ou remoções), os dados devem ser fisicamente movimentados na memória para que a correspondência seja mantida.

Uma outra forma de se organizar uma lista em memória é quebrando essa correspondência, e usando uma outra forma de se encontrar os dados da lista em vez de sua posição relativa na memória.
A forma mais comum de se fazer isso é manter, para cada dado na lista, uma informação que permite chegar ao dado que o sucede na lista.
Por exemplo, se os dados da lista são colocados em um vetor, a informação que permite chegar a um dado é a posição no vetor.
Uma forma de se manter essa posição é colocando a lista em dois vetores de mesmo tamanho, um contendo espaço para os dados e outro contendo a posição do sucessor de cada dado. A posição onde está o dado que é o sucessor do dado na posição 3 do vetor de dados é colocada na posição 3 do vetor de posições. 

Cada dado da lista pode estar em qualquer posição do vetor, então necessitamos saber a posição do primeiro dado da lista.
Quando uma lista é organizada dessa forma, com um dado levando à informação do seguinte e assim por diante, dizemos que a lista é "encadeada".

Por exemplo, nesse esquema, a lista com os dados `X Y Z` poderia ser armazenada como abaixo:
```
   cap 5
   prim 2
   num 3
   dados       Z ? X Y ?
   sucessores -1 ? 3 0 ?
```
Os sinais `?` marcam valores nos vetores que não estão sendo ocupadas pela lista.
O primeiro dado da lista está na posição 2, informação dada por "prim". Nessa posição está o dado `X`.
No vetor de sucessores, na mesma posição do dado (2), tem o valor 3, que é a posição do dado seguinte (o segundo dado da lista).
Na posição 3, está o valor `Y` e o sucessor é 0.
Na posição 0 está o terceiro dado da lista, `Z` e o sucessor, -1. O valor -1, que não representa um índice válido nos vetores é um valor especial usado para representar que esse dado não tem sucessor, é o último da lista.

Se dessa lista fosse removido o segundo dado (na posição 1 da lista e 3 do vetor), a lista ficaria assim:
```
   cap 5
   prim 2
   num 2
   dados       Z ? X Y ?
   sucessores -1 ? 0 0 ?
```

Se na lista original fosse inserido o dado W na segunda posição da lista (entre o X e o Y), a lista poderia ficar assim:
```
   cap 5
   prim 2
   num 4
   dados       Z W X Y ?
   sucessores -1 3 1 0 ?
```
ou assim:
```
   cap 5
   prim 2
   num 4
   dados       Z ? X Y W
   sucessores -1 ? 4 0 3
```
Tanto na inserção quanto na remoção, nenhum dado presente na lista precisou ser movimentado, o que é um dos motivos para se usar o encadeamento.

Claro, tem algumas desvantagens nessa implementação:
- é necessário mais memória, para colocar informação sobre os sucessores
- é necessário encontrar os espaços livres quando se tem uma inclusão. Uma forma de se fazer isso é organizar uma segunda lista no mesmo vetor, contendo os elementos livres, e retirar dela nas inserções, colocar nela nas remoções
- para implementar inclusão e remoção, tem que alterar a informação de sucessor do dado na posição anterior ao ponto de inserção/remoção; dependendo da posição, encontrar esse dado pode levar a percorrer a lista desde o início.
- caso se use realocação de memória que libera espaço quando a área ocupada pela lista é muito inferior à capacidade, pode ser necessário deslocar dados que estejam na área que será liberada (já que um dado pode estar em qualquer lugar, não se tem garantia de que não tenha dado no final da área alocada), além de rearranjar a lista de livres.

#### Código exemplo

Abaixo está uma possível implementação de operações de inicialização e inserção em uma lista encadeada com dois vetores, nos quais são mantidas duas listas, a lista propriamente dita e a lista de locais livres.

```c
// no .h tem typedef struct lista *Lista;
struct lista {
  int prim;         // índice do vetor onde está o primeiro dado da lista
  int livre;        // índice do vetor onde está o primeira pos. livre
  dado_t dados[N];  // vetor que contém os dados da lista
  int prox[N];      // vetor que contém os encadeamentos
};

// prox[i] contém o índice do vetor onde está o dado que segue o
//         dado que está no índice i do vetor

// inicializa a lista.
// a memória para a lista já foi alocada.
// a lista fica vazia (prim é -1, dizendo que não tem um índice válido
//   que armazene o primeiro dado da lista)
// todos as posições do vetor são colocadas na lista de livres
void l_init(Lista l)
{
  l->prim = -1;

  l->livre = 0;
  for (int i=0; i<N-1; i++) l->prox[i] = i+1;
  l->prox[N-1] = -1;
}

// encontra um índice livre, e retira da lista de livres
static int ind_livre(Lista l)
{
  if (l_cheia(l)) erro(); // poderia realocar...
  // pega o índice do primeiro da lista de livres
  int ind = l->livre;
  // o primeiro livre passa a ser o seguinte
  l->livre = l->prox[ind];
  return ind;
}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d)
{
  // acha um índice livre
  int novo = ind_livre(l);
  // coloca o dado no vetor
  l->dados[novo] = d;
  // o dado que segue o novo primeiro dado é o antigo primeiro
  // se a lista estava vazia, l->prim é -1, que é o que queremos como prox
  l->prox[novo] = l->prim;
  // atualiza a nova primeira posição
  l->prim = novo;
}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d)
{
  // se a lista estiver vazia, é mais fácil inserir no início...
  if (vazia(l)) { l_insere_inicio(l, d); return; }
  // acha um índice livre
  int novo = ind_livre(l);
  // coloca o dado no vetor
  l->dados[novo] = d;
  // ele é o novo último
  l->prox[novo] = -1;
  // acha a posição do último (vai virar penúltimo)
  // sabemos que a lista não está vazia, então l->prim é um índice válido
  // poderia poupar essa busca colocando a posição do último no descritor
  int ult;
  for (ult = l->prim; l->prox[ult] != -1; ult = l->prox[ult])
    /*nada*/;
  // o dado que segue o antigo último é o novo último
  l->prox[ult] = novo;
}

// insere o dado d na lista l, de forma que ele fique na posição p
void l_insere_pos(Lista l, dado_t d, int p)
{
  // se a inserção for no início, temos uma função pronta...
  if (p == 0 || l_vazia(l)) { insere_inicio(l, d); return; }
  // acha um índice livre
  int novo = ind_livre(l);
  // coloca o dado no vetor
  l->dados[novo] = d;
  // acha a posição do dado que deve anteceder o novo dado
  // caso a lista não tenha elementos suficientes, acha o último
  int ant = l->prim;  // índice de um dado no vetor
  int pos_ant = 0;    // posição na lista correspondente a esse dado
  while (pos_ant < p-1) {
    if (l->prox[ant] == -1) {
      // chegamos no último da lista!
      break;
    }
    ant = l->prox[ant];
    pos_ant++;
  }
  // o índice do dado seguinte
  int seguinte = l->prox[ant];
  // o dado novo fica após o anterior
  l->prox[ant] = novo;
  // depois do novo fica o que estava depois do anterior
  l->prox[novo] = seguinte;
}
```

#### Exercícios

Faça as funções de remoção, teste a lista.

### Listas encadeadas com alocação individual

Quando a lista é encadeada, não necessitamos movimentar fisicamente na memória os dados que já estão na lista, mesmo na implementação de operações que alterem a posição desses dados na lista.
Com a alocação contígua dos dados, entretanto, ainda resta o problema de se manter alocada memória nos vetores além das necessidades da lista.
É muito caro realocar os vetores para terem o tamanho exato necessário cada vez que o número de elementos na lista é alterado. Geralmente se aloca os vetores com folga para evitar excesso de operações de realocação e cópia de dados.

Uma outra possibilidade de alocação de memória é alocar cada dado individualmente, e só manter alocados os que estão efetivamente em uso pela lista. Quando um dado é removido, a memória ocupada por ele é liberada. O gerenciamento de memória fica com o sistema de alocação de memória e não mais com a lista. Nesse caso, um dado pode estar em qualquer posição da memória, e sua posição é mantida por um ponteiro e não por um índice.
Para cada posição deve-se guardar, além do dado, a informação de sucessor (um ponteiro para o dado seguinte).
Essas duas informações (dado e ponteiro) devem ser mantidas juntas, o que geralmente é feito declarando uma estrutura que contém um dado e um ponteiro para outra dessas estruturas. É usual chamar essa estrutura de "nó" ou "nodo".

Nossa lista passa a ter então dois tipos de registros: uma estrutura que mantém informações gerais da lista (em geral se chama de descritor da lista) e uma estrutura para cada dado (um nó).

Nessa implementação, não vamos ter memória desperdiçada com espaço não usado pela lista, mas em compensação tem memória extra gasta pelo sistema de gerenciamento de memória, que vai gerenciar um número maior de alocações.

Abaixo está um exemplo de implementação parcial de lista encadeada com alocação individual. Nesse exemplo, o descritor da lista contém somente um ponteiro para o nó que contém o primeiro elemento da lista (`NULL` quando vazia), e um nó contém o dado (representado pelo tipo `dado_t`) e um ponteiro para o nó que contém o dado seguinte.

```c
   /// lista.c
   #include "lista.h"  // contém  "typedef struct lista *Lista;"

   // a estrutura para conter um nó -- contém um dado e um ponteiro para o próximo nó
   // essa estrutura é interna à implementação da lista
   typedef struct nodo nodo;
   struct nodo {
     dado_t dado;
     nodo *prox;
   };

   // a estrutura para o descritor da lista -- contém um ponteiro para o nó que contém o primeiro dado da lista (poderia conter mais informação, se necessário)
   typedef struct _lista {
     nodo *prim;
   };
      
   Lista lista_cria(void)
   {
     // cria uma lista vazia
     Lista l = malloc(sizeof *l);
     assert(l != NULL);
     // a lista está vazia, o primeiro elemento é representado por NULL
     l->prim = NULL;
     return l;
   }
   
   void lista_destroi(Lista l)
   {
     // precisa liberar a memória de cada nó, e do descritor
     nodo *no = l->prim;
     while (no != NULL) {
       // salva o ponteiro que está no nó, não pode acessar o nó depois do free
       nodo *prox = no->prox;
       free(no);
       no = prox;
     }
     free(l);
   }

   bool lista_vazia(Lista l)
   {
     return l->prim == NULL;
   }
   
   // função auxiliar para alocar e inicializar um nó
   static nodo *cria_nodo(dado_t d, nodo *prox)
   {
     nodo *no = malloc(sizeof *no);
     assert(no != NULL);
     no->dado = d;
     no->prox = prox;
     return no;
   }
   
   void lista_insere_inicio(Lista l, dado_t d)
   {
     // aloca um nó para conter o novo dado. 
     // esse nó estará no início da lista, então o nó seguinte a ele é o nó que
     //   atualmente é o primeiro da lista (mesmo que seja NULL)
     nodo *novo = cria_nodo(d, l->prim);
     // ele é o novo primeiro nó
     l->prim = novo;
   }

   // encontra o último nó da lista (o prox dele é NULL)
   static nodo *lista_ultimo_nodo(Lista l)
   {
     if (lista_vazia(l)) return NULL;
     nodo *no = l->prim;
     while (no->prox != NULL) {
       no = no->prox;
     }
     return no;
   }
   
   void lista_insere_fim(Lista l, dado_t d)
   {
     // se a lista estiver vazia, insere no início
     if (lista_vazia(l)) {
       lista_insere_inicio(l, d);
     }
     // aloca um nó par conter o novo dado. 
     // esse nó estará no final da lista, então o nó seguinte a ele é NULL
     nodo *novo = cria_nodo(d, NULL);
     // o antigo último nó deve apontar para o novo último
     nodo *ult = lista_ultimo_nodo(l);
     ult->prox = novo;
   }

   // retorna o nodo em dada posição da lista (pos 0 é o primeiro)
   // retorna NULL se não existir
   static nodo *lista_nodo_pos(Lista l, int pos)
   {
     if (pos < 0) return NULL;
     nodo *no = l->prim;
     int p = 0;
     while (no != NULL && p < pos) {
       no = no->prox;
       p++;
     }
     return no;
   }
   
   void lista_insere_pos(Lista l, int pos, dado_t d)
   {
     if (pos < 0) return;
     if (pos == 0) {
       lista_insere_inicio(l, d);
	    return;
     }
     // acha o nó na posição anterior à inserção
     nodo *anterior = lista_nodo_pos(l, pos-1);
     if (anterior == NULL) return;
     // nó na posição seguinte à inserção
     nodo *seguinte = anterior->prox;
     // aloca um nó par conter o novo dado. 
     // esse nó estará antes de nó seguinte
     nodo *novo = cria_nodo(d, seguinte);
     // o novo nó está depois do nó anterior
     anterior->prox = novo;
   }
```

#### Exercícios

1. Implementa as funções de remoção da lista acima
2. Teste a lista
3. Implementa uma função para inverter a ordem dos nós de uma lista encadeada.


### Listas duplamente encadeadas

Por vezes (em uma fila do tipo deque, por exemplo), necessitamos de operações de inclusão e remoção em ambas as pontas da lista, e a lista simplesmente encadeada não tem um desempenho bom o suficiente para todas essas operações. Não é muito difícil de se otimizar para se ter um bom desempenho em três dessas operações, mas não nas quatro.
A adição de um segundo encadeamento na lista resolve essa limitação, ao custo de mais memória para armazenar mais um ponteiro em cada nó da lista.
Cada nó passa a ter dois ponteiros: um que aponta para o próximo elemento da lista (como em uma lista simplesmente encadeada) e um outro que aponta para o elemento anterior.
Em todas as operações que alteram a lista (inclusão e remoção de elementos), temos que manter atualizados esses dois ponteiros, o que torma a implementação um pouco mais complexa.
No descritor da lista, se coloca um ponteiro para o último elemento da lista, além do primeiro, para permitir que a inclusão e remoção no final da lista seja tão fácil quanto no início. Outra alternativa seria fazer a lista circular (em que o último é o anterior do primeiro) ou mesmo com um nó sentinela (uma lista circular com um nó permanente, sem dado útil, que fica entre o último e o primeiro).

#### Implementação

Suponha que o descritor da lista e o nó sejam declarados como abaixo:
```c
   typedef struct lista *Lista;
   typedef struct nodo nodo;
   struct lista {
     int num;    // número de elementos na lista
     nodo *prim; // nó que contém o primeiro elemento da lista
     nodo *ult;  // nó que contém o último elemento da lista
   };
   struct nodo {
     dado_t info; // a informação que é mantida por este nó
     nodo *prox;  // ponteiro para o nó seguinte
     nodo *ant;   // ponteiro para o nó anterior
   };
```

A operação de remoção de um nó qualquer da lista seria simples, tendo um ponteiro para o nó a remover:
```c
   ///
   nodo *vitima;
   // ... (faz 'vitima' apontar para o nó a remover)
   // cria variáveis auxiliares para apontar para os nós envolvidos
   nodo *seguinte = vitima->prox; // o nó que está depois do nó a ser removido
   nodo *anterior = vitima->ant;   // o nó que está antes do nó a ser removido
   // remove o nó dos encadeamentos
   anterior->prox = seguinte;   // o nó que segue o nó anterior ao removido é o que está depois do removido
   seguinte->ant = anterior;    // o nó que antecede o nó que está após o removido é o que está antes do removido
   // libera a memória do nó removido
   free(vitima);
   l->num--;
```
Abaixo está uma "animação" desse código, com o estado das variáveis após a execução de cada linha.
```
   // estado inicial, a lista tam 3 elementos, A, B, C
   prim=pA (o campo prim do descritor da lista tem um ponteiro para o nó que tem A)
   ult =pC
   num =3  (o campo num da lista tem 3)

        |A|--|pB|   (um nodo que tem o dado A, o ponteiro ant é NULL, prox é pB)
        |B|pA|pC|
        |C|pB|--|
```
```
   nodo *vitima; // ... (faz 'vitima' apontar para o nó a remover)

   prim=pA
   ult =pC
   num =3

   vitima=pB  (o nodo que vai ser removido é o que contém o B)

        |A|--|pB|
        |B|pA|pC|
        |C|pB|--|
```
```
   nodo *seguinte = vitima->prox;

   prim=pA
   ult =pC
   num =3

   vitima=pB
   seguinte=pC

        |A|--|pB|
        |B|pA|pC|
        |C|pB|--|
```
```
   nodo *anterior = vitima->ant;   // o nó que está antes do nó a ser removido

   prim=pA
   ult =pC
   num =3

   vitima=pB
   seguinte=pC
   anterior=pA

        |A|--|pB|
        |B|pA|pC|
        |C|pB|--|
```
```
   anterior->prox = seguinte;

   prim=pA
   ult =pC
   num =3

   vitima=pB
   seguinte=pC
   anterior=pA

        |A|--|pC|
        |B|pA|pC|
        |C|pB|--|
```
```
   seguinte->ant = anterior;

   prim=pA
   ult =pC
   num =3

   vitima=pB
   seguinte=pC
   anterior=pA

        |A|--|pC|
        |B|pA|pC|
        |C|pA|--|
```
```
   free(vitima);

   prim=pA
   ult =pC
   num =3

   vitima=pB
   seguinte=pC
   anterior=pA

        |A|--|pC|
        |C|pA|--|
```
```
   l->num--;

   prim=pA
   ult =pC
   num =2

   vitima=pB
   seguinte=pC
   anterior=pA

        |A|--|pC|
        |C|pA|--|
```
```
   return;

   prim=pA
   ult =pC
   num =2

        |A|--|pC|
        |C|pA|--|
```
Esse código funciona quando se está removendo um nó intermediário, mas teremos problemas com o primeiro e o último nós (os ponteiros `antes` ou `depois` podem ser `NULL` (e não podem ser dereferenciados), e devemos alterar o descritor. Devemos tomar alguns cuidados especiais:
```c
   /// ...
   // remove o nó dos encadeamentos
   if (anterior != NULL) {
     anterior->prox = seguinte;   // o nó que segue o nó anterior ao removido é o que está depois do removido
   } else {
     l->prim = seguinte;       // o nó removido era o primeiro da lista, o que segue é o novo primeiro
   }
   if (seguinte != NULL) {
     seguinte->ant = anterior;    // o nó que antecede o nó que está após o removido é o que está antes do removido
   } else {
     l->ult = anterior;         // o nó removido era o último da lista, o antecessor dele é o novo último
   }
   /// ...
```
Esse código funciona para a remoção quando a lista tem um só elemento?

Na inserção, tem que tomar cuidados semelhantes.

Tendo os dois encadeamentos e o número de elementos, pode-se otimizar um pouco a busca de um elemento por posição: se a posição buscada for mais próxima do início que do final da lista, realiza-se a busca a partir do primeiro elemento em direção ao final da lista, e se for mais próxima do final, busca-se a partir do último elemento em direção ao início.

### Outras alternativas de implementação de listas

#### Ponteiro para o último elemento no descritor da lista simplesmente encadeada

Dessa forma, a operação de inclusão no final da lista pode ser otimizada, sem ser necessário percorrer a lista toda até o final. A remoção no final não é afetada, necessitando o percurso de toda a lista.

#### Lista circular

Ao invés de o último elemento do encadeamento ter um ponteiro em NULL, ele aponta para o primeiro elemento.
Em uma lista simplesmente encadeada, dá para nesse caso se manter no descritor somente um ponteiro para o último elemento e não para o primeiro, e ainda assim não necessitar percorrer a lista toda para inserção e remoção no início e inserção no final.

#### Uso de nó sentinela

Usa-se um nó suplementar, que é alocado na inicialização da lista e somente removido na sua destruição. Esse nó não contém um dado válido, serve simplesmente para garantir que sempre existe um nó no encadeamento, mesmo que a lista esteja vazia, e é para esse nó que o descritor aponta.
Geralmente é usado com a implementação circular de lista encadeada.
Torna mais simples as operações de inserção e remoção, porque não existem mais os casos em que se necessite alterar o ponteiro do descritor, porque ele aponta sempre para o sentinela.

#### Não uso do descritor

Nesse caso, usa-se um ponteiro para o primeiro nó para se representar a lista.
Economiza-se memória, mas a interface de acesso à lista fica menos clara, porque operações que alteram a lista (inserção e remoção) podem ter que alterar o ponteiro que representa a lista, que pertence ao usuário.
O uso de um nó sentinela pode evitar esse problema.

### Outros aspectos de listas (e de outras estruturas de dados)

#### Listas genéricas

Nas implementações de listas acima, o dado armazenado na lista tem o tipo 'dado_t'.
Como fazer se o programa necessita listas de mais de um tipo de dados?

Uma possibilidade é fazer com que a lista armazene ponteiros para os dados, e a memória para esses dados ser gerenciada pelo usuário da lista. Uma vantagem dessa forma é que um dado pode facilmente estar em mais de uma lista ao mesmo tempo, sem precisar ser copiado.
Como a lista não vai tocar nos dados, eles podem ter quelquer tipo, e os ponteiros armazenados pela lista podem ser do tipo `void *`.

Outra possibilidade é a lista saber quantos bytes cada dado tem (informado na inicialização da lista ou a cada inserção e remoção), alocar memória para o nó com espaço suficiente para esse dado, e copiar o dado para o nó a partir de um ponteiro fornecido pelo usuário, usando ua função de cópia de memória. Vimos essa solução na fila do t2, implementada como um vetor.
A linguagem C oferece uma "facilidade" para a declaração de uma estrutura que será alocada com espaço extra para isso: o último campo de uma estrutura pode ser um vetor sem tamanho. Por exemplo, o nó de uma lista duplamente encadeada poderia ser declarado assim:
```c
struct nodo {
    nodo *prox;
    nodo *ant;
    char dado[];
};
```
O dado é declarado como um vetor de char, sem tamanho. O tipo é char porque não é possível declarar um vetor de void.
Uma operação sizeof nessa struct dá o tamanho de toda a estrutura exceto o vetor.
Se sabemos quantos bytes tem o dado, podemos alocar memória para a estrutura com o sizeof somado ao número de bytes para o dado, e podemos copiar o dado do usuário, apontado por pdado, usando uma função de cópia de memória:
```c
  nodo *no = malloc(sizeof *no + tamanho_do_dado);
  memcpy(no->dado, pdado, tamanho_do_dado);
```
