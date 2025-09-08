# Árvores

Uma árvore é uma estrutura de que organiza dados de uma forma hierárquica.
Um exemplo de tal hierarquia seria a organização de diretórios e arquivos em um disco.
Cada dado em uma árvore é armazenado em um "nó".
Uma árvore é um conjunto de nós, com as seguintes características:
- se o conjunto de nós está vazio, a árvore é vazia;
- senão, um dos nós do conjunto é chamado de "raiz" da árvore, e os demais são divididos em subconjuntos disjuntos, cada um deles uma árvore, chamadas de subárvores da raiz.

Existem várias formas de se representar graficamente uma árvore, a mais comum é representar um nó com um círculo, dentro do qual é colocado o valor do nó, o círculo que corresponde à raiz acima dos que correspondem às raízes de suas subárvores, com setas (também chamadas arestas) ligando a raiz da árvore às raízes das subárvores.

Alguns conceitos de árvores:
- filhos de um nó: raízes das subárvores da árvore que tem esse nó como raiz;
- nó raiz: nó da árvore que não tem pai (uma árvore tem um único nó raiz);
- nó folha ou nó externo: nó da árvore que não tem filho;
- nó interno: nó que tem filho;
- grau de um nó: número de filhos que o nó tem;
- grau de uma árvore: maior grau encontrado em um nó da árvore;
- caminho em uma árvore: sequência de nós (não repetidos) da árvore, onde dois nós adjacentes têm a relação pai-filho;
- comprimento de um caminho: número de arestas em um caminho, um a menos que o número de nós do caminho (alguns autores consideram que o comprimento de um caminho é o número de nós do caminho, mas aparentemente esses autores são minoritários);
- profundidade de um nó: comprimento do caminho entre a raiz da árvore e esse nó;
- nível de uma árvore: conjunto de nós que estão na mesma profundidade; a raiz está no nível 0, seus filhos no nível 1, os filhos deles (netos da raiz) no nível 2, etc;
- altura de uma árvore: comprimento do maior caminho entre a raiz e alguma folha da árvore ou profundidade do nó mais profundo da árvore; uma árvore somente com o nó raiz tem altura 0; uma árvore vazia tem altura -1.

Quando a ordem dos filhos de um nó é importante, diz-se que a árvore é **ordenada**. Nesse caso, a árvore que tem a raiz A e os filhos B e C é considerada diferente da árvore que tem raiz A e filhos C e B. Se as árvores forem não ordenadas, essas duas árvores são consideradas iguais.

Nesta disciplina, nos concentraremos em um tipo especial de árvores, as árvores de grau 2, ou *árvores binárias*.

## Árvores binárias

Em uma árvore binária, cada nó tem no máximo 2 filhos. Nos restringiremos ao estudo de árvores ordenadas, então a ordem entre esses dois filhos é uma informação importante. Como são só dois filhos, eles são comumente chamados de filho esquerdo e direito.

Árvores binárias são estruturas definidas de forma recursiva, uma árvore binária pode ser:
- vazia; ou
- um nó, chamado de raiz e dois filhos, esquerdo e direito, que são árvores binárias.

Uma forma comum de implementação de uma árvore é como um ponteiro para um nó, e esse nó contém o valor associado ao nó e dois ponteiros para os nós raiz das árvores filhas. Uma árvore vazia pode ser implementada como um ponteiro nulo ou como um nó cujo valor é ignorado e tem os dois ponteiros para nulo. Na primeira implementação, será alocado um nó para cada valor armazenado na árvore. Na segunda, será alocado, além desses, um nó a mais para cada filho vazio, duplicando o número de nós ocupados pela árvore, mas simplificando a interface de funções que alteram a árvore (veremos mais tarde na discussão sobre inserção e remoção).

```c
   typedef struct no no_t;
   struct no {
     dado_t val;
     no_t *esq;
     no_t *dir;
   };
   typedef no_t arvore;  // um sinônimo, uma árvore é representada por um ponteiro para um nó
```

Grande parte dos algoritmos que operam sobre árvores são mais facilmente implementados em forma recursiva.
Por exemplo, para calcular quantos nós tem uma árvore, pode-se usar uma definição recursiva:
   - uma árvore vazia tem 0 nós
   - uma árvore não vazia tem um nó a mais que a soma dos nós de suas duas subárvores.
Em código C:
```c
   int num_nos(arvore *a)
   {
      if (vazia(a)) return 0;
      return 1 + num_nos(a->esq) + num_nos(a->dir);
   }
```
Para calcular a altura de uma árvore:
   - uma árvore vazia tem altura -1
   - uma árvore não vazia tem altura que é um além da maior altura entre as árvores filhas.
Em código C:
```c
   int altura(arvore *a)
   {
      if (vazia(a)) return -1;
      return 1 + maior(altura(a->esq), altura(a->dir));
   }
```
A implementação dessas funções de forma não recursiva é um tanto mais trabalhosa.

A função que testa se uma árvore é vazia testa se o ponteiro é NULL no caso de se implementar ávore vazia como ponteiro NULL ou testa se os ponteiros esq e dir são NULL, se for essa a implementação escolhida.

### Exercícios

1. Faça uma função que recebe uma árvore e retorna quantos nós folha ela tem
2. Faça uma função que recebe uma árvore e retorna quantos nós não folha ela tem
3. Faça uma função que recebe uma árvore e um valor e retorna um booleano que diz se a árvore contém ou não um nó com esse valor. Use a função auxiliar `bool valores_iguais(dado_t, dado_t)` para comparar dois valores.
4. Faça uma função que recebe uma árvore e um valor e retorna o nível nessa árvore que está o nó que contém esse valor, ou -1 se o valor não estiver em nenhum nó da árvore. A raiz de uma árvore está no nível 0, os filhos da raiz no nível 1 etc.
5. Faça uma função que recebe uma árvore e retorna o "fator de equilíbrio" do nó raiz dessa árvore. O "fator de equilíbrio" a ser considerado é a diferença entre a altura da subárvore esquerda e a da direita.
