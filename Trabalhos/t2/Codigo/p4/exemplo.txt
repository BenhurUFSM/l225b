## Árvore AVL

Uma árvore binária de busca pode ter um desempenho muito bom para ser usada como uma estrutura de dados de busca, mas esse desempenho depende bastante da forma como os dados estão distribuídos na árvore.
Em casos extremos de distribuição mal feita, a árvore pode ficar com um nó por nĩvel, resultando em um desempenho pior que uma lista linear.
Isso acontecerá, por exemplo, se incluirmos dados em ordem crescente em uma ABB implementada como visto na aula passada.
Para que a estrutura da árvore seja bem aproveitada e se tenha um bom desempenho, é necessário que os dados estejam bem distribuĩdos na árvore, de forma equilibrada.
O objetivo de equilibrar a árvore é limitar sua altura, já que os algoritmos usados em uma ABB têm geralmente um desempenho que é proporcional a essa altura.
Uma forma de se ter uma árvore equilibrada é inserindo os dados nela em uma ordem otimizada, mas só é viável quando os dados são conhecidos de antemão.
Outra forma é alterar a árvore normalmente e reorganizá-la quando se detecta que ela está desequilibrada. 
Para isso, deve-se definir uma forma de se medir o equilíbrio da árvore, e uma forma de se reequilibrá-la quando um desequilíbrio for detectado.
A detecção de desequilíbrio pode ser realizada quando houver alterações na árvore, que são causadas por inclusões e remoções de dados.

Existem [vários](https://en.wikipedia.org/wiki/Self-balancing_binary_search_tree) tipos de árvores que se auto-equilibram.
Vamos estudar um pouco mais a fundo uma delas, as **árvores AVL** (AVL são as iniciais dos inventores, Andelson-Velsky e Landis).

### Equilíbrio em árvores AVL

Em árvores AVL, o equilíbrio da árvore é dado pela diferença de altura entre as subárvores de um nó.
A árvore é considerada equilibrada se todos os seus nós apresentam uma diferença entre a altura de seu filho esquerdo e de seu filho direito inferior a 2.
Essa diferença é chamada de *fator de equilíbrio*, e geralmente é calculada como a altura do filho esquerdo menos a do direito, e são aceitos valores 0, 1 ou -1.

A forma de se reequilibrar uma árvore AVL é durante as operações de inserção e remoção.
Essas operações são realizadas como vimos para ABB, mas no final da operação, antes do retorno recursivo ao nó anterior, verifica-se o fator de equilíbrio do nó e se realiza uma operação de reequilíbrio caso seja necessário.
Dessa forma, ao concluir a operação de inserção ou remoção, garante-se que a árvore encontra-se novamente equilibrada.

O reequilíbrio é realizado por meio de alterações chamadas *rotações*.
Uma rotação envolve dois nós (pai e filho) e suas sub-árvores.
Uma rotação não altera as características de ordenamento dos dados de uma ABB.
Na figura abaixo, X e Y são nós da árvore, A, B e C são subárvores.
Uma rotação à direita transforma a árvore com raiz X na árvore com raiz Y.
Uma rotação à esquerda transforma a árvore com raiz Y na árvore com raiz X.
Em ambas as árvores da figura, a ordem dos valores está de acordo com as regras de uma ABB: todos os nós de A têm valores menores que Y, os nós de B têm valores entre X e Y e os nós de C têm valores maiores que X.

![rotacao AVL](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-rota%C3%A7%C3%A3o%20AVL.png)

### Inclusão

Vamos analisar primeiro o reequilíbrio de uma árvore AVL após uma inclusão; depois analisaremos a remoção.
Antes da inclusão a árvore está equilibrada.
A inclusão pode desequilibrar a árvore.
A ideia básica é que realizando a inclusão e retornando-se no caminho desde o nó onde a inclusão foi realizada (um novo nó folha) até a raiz, quando se encontra um nó que ficou desequilibrado, esse nó é rotacionado para corrigir esse desequilíbrio, e não é necessária a correção em nenhum outro ponto da árvore.
Supondo que isso seja verdade, vamos considerar a árvore da esquerda da figura acima, onde uma inserção à esquerda de X causa um desequilíbrio em X.
O caso de um desequilibrio do nó X devido a uma inserção em seu lado direito (um valor maior que X) poderia ser tratado de forma semelhante, trocando esquerda por direita, como se a árvore fosse espelhada.

Pela nossa suposição, essa inserção não deve causar desequilíbrio em nenhum nó abaixo de X, porque se esse fosse o caso esse desequilíbrio teria sido corrigido quando do retorno por esse nó.
Como a inserção será à esquerda de X, o nó X só irá se desequilibrar se houver um aumento na altura de seu filho esquerdo (Y). 
A inserção é de um único valor, então a altura de Y só pode ter um aumento de um.
Logo, antes da inserção, como a árvore está equilibrada e fica desequilibrada em X após a inserção, o fator de equilíbrio de X deve ser 1 antes da inserção, e deve virar 2 após.
Se o fator de equilíbrio em X fosse 0 ou -1 antes da inserção, ele não se desequilibraria com um aumento de 1 na altura de Y.
Para que o fator de equilíbrio de X antes da inserção seja 1, a altura da árvore Y antes da inserção deve ser 1 a mais que a altura da árvore C. 
A inclusão será realizada na subárvore A ou B, e para que aumente a altura de Y, essa subárvore deve aumentar sua altura. Caso A e B tenham altura diferente entre si, se a inclusão for na árvore mais baixa, ela chegará no máximo até a altura da árvore mais alta, não alterando a altura de Y; se a inclusão for na árvore mais alta, ela aumentará ainda mais a diferença de altura para a árvore mais baixa, desequilibrando Y, o que vai contra nossa premissa de que o primeiro nó a ficar desequilibrado é X. Logo, A e B devem ter a mesma altura antes da inclusão.
Supondo que a altura da árvore C seja *h*, a árvore antes da inclusão deve estar como a figura abaixo, com a altura da subárvore colocada à direita e o seu fator de equilíbrio acima do nó raiz de cada subárvore.

![AVL antes da inclusão LL](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20antes%20da%20inclus%C3%A3o.png)

Temos duas possibilidades, a inclusão pode ser feita na subárvore A ou na subárvore B.
Vamos analisar cada caso separadamente.

No primeiro caso, a inclusão será realizada na subárvore A, aumentando sua altura. Essa inclusão faz com que a árvore fique como a figura abaixo, com as alterações marcadas em vermelho.

![AVL depois da inclusão LL](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20inclus%C3%A3o%20LL.png)

O nó X está desequilibrado, com fator de equilíbrio 2. Efetuando a rotação, o nó Y vira a nova raiz dessa subárvore, e as alturas de X e Y, assim como seus fatores de equilíbrio são também alterados. Essas alteraçãos estão na figura abaixo (com os valores alterados em azul).

![AVL depois da inclusão e rotação LL](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20inclus%C3%A3o%20e%20rota%C3%A7%C3%A3o%20LL.png)

A altura total da nova subárvore é *h+2*, que é a mesma altura que a subárvore tinha antes da inclusão.
Dessa forma, os nós acima dessa subárvore não são afetados (quanto ao seu fator de equilíbrio) pelas alterações.

No segundo caso, a inclusão é realizada na subárvore B. A figura abaixo mostra como fica a árvore, depois dessa inclusão, que aumenta a altura da subárvore B.

![AVL depois da inclusão LR](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20inclus%C3%A3o%20LR.png)

Se efetuarmos a mesma rotação nessa árvore, o problema de desequilíbrio não será resolvido (faça o desenho para confirmar).
Para resolvê-lo, precisamos incluir a raiz da subárvore B na rotação.
A figura abaixo representa a mesma árvore da figura acima, expandindo a raiz da subárvore B. As duas filhas de B (Be e Bd) devem ter a mesma altura antes da inclusão, por motivo semelhante ao visto acima para os filhos de Y. A solução é a mesma, sendo a inclusão realizada em Be ou Bd. Na figura, foi feita em Bd.

![AVL depois da inclusão LR, expandido](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20inclus%C3%A3o%20LR%2C%20expandido.png)

A solução consistem em uma rotação dupla, inicialmente rodando B e Y na subárvore Y e depois fazendo uma nova rotação entre X e B, de forma que B fique na raiz da subárvore, tendo como filho esquerdo Y e direito X. A figura abaixo mostra as duas rotações, com as alterações de altura e fator de equilíbrio marcadas em azul na configuração final.
Assim como no caso anterior, a altura final da subárvore é *h+2*, que é a mesma de antes da inclusão, não afetando o fator de equilíbrio de nós que estejam acima da subárvore rotacionada.

![AVL depois da inclusão e rotação LR, expandido](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20inclus%C3%A3o%20e%20rota%C3%A7%C3%A3o%20LR%2C%20expandido.png)

Resumindo, ao se detectar, no retorno do caminho da inclusão, um nó desequilibrado, deve-se observar o fator de equilíbrio de seu filho mais alto. Se os dois fatores tiverem o mesmo sinal, procede-se a uma rotação simples entre eles. Se tiverem sinais contrários, procede-se a uma rotação dupla.

 
 ### Remoção
 
 O desequilíbrio causado por uma remoção é semelhante ao de uma inclusão e a solução é também semelhante.
 A diferença principal é que, no caso da inclusão, o ramo da árvore onde a inclusão ocorre é o que fica mais alto depois da inclusão, e é nesse ramo que devem ser realizadas rotações para resolver o desequilíbrio; no caso da remoção, o ramo onde ela ocorre fica mais baixo, e a rotação para resolver o desequilíbrio deve acontecer no outro ramo, que é quem deve ceder nós para reequilibrar a árvore.
 
 Como no caso da inclusão, a remoção só remove um nó, então só deve alterar em no máximo uma unidade a altura da árvore.
 Para causar um desequilíbrio, essa diminuição de um na altura de um ramo deve fazer com que um nó passe a ter um fator de equilíbrio 2 ou -2; para que isso seja possível, o fator de equilíbrio desse nó antes da remoção deve ser 1 ou -1. Abaixo está uma figura com a configuração necessária de uma subárvore para que uma remoção no seu filho direito possa causar um desequilíbrio em sua raiz.

![AVL antes da remoção](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20antes%20da%20remo%C3%A7%C3%A3o.png)

Após a remoção, a subárvore direita tem sua altura reduzida, e desequilibra a raiz, como pode ser visto na figura abaixo.

![AVL depois da remoção](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20remo%C3%A7%C3%A3o.png)

Para reequilibrar a árvore, vamos precisar realizar rotações na subárvore esquerda, que é mais alta e pode fornecer nós para diminuir sua altura e aumentar a altura da subárvore direita.
A figura abaixo tem a mesma árvore acima, com o nó raiz da subárvore esquerda expandido.
Existem 3 possibilidades de reequilíbrio, de acordo com o fator de equilíbrio (0, 1 ou -1) da raiz da esquerda.
Na figura abaixo está o primeiro caso, com fator de equilíbrio 0.

![AVL depois da remoção, expandido0](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20remo%C3%A7%C3%A3o%2C%20expandido0.png)

Esse desequilíbrio pode ser resolvido com uma rotação envolvendo os nós X e Y, como pode ser visto abaixo.

![AVL depois da remoção e rotação, expandido0](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20remo%C3%A7%C3%A3o%20e%20rota%C3%A7%C3%A3o%2C%20expandido0.png)

A subárvore ficou equilibrada (com fator -1), e com a mesma altura que tinha antes da remoção, o que significa que todo o resto da árvore continua tão equilibrado quanto antes da remoção.

No segundo caso, a subárvore esquerda tem fator de equilíbrio 1, e é resolvido da mesma forma.
A figura abaixo mostra o estado da árvore antes e depois da rotação.
Nesse caso, ambas subárvores ficaram com fator de equilíbrio 0, mas a altura total da subárvore ficou menor que antes da inclusão.
Isso quer dizer que outro nó acima da árvore pode também ficar desequilibrado.
Diferente da inclusão, na remoção pode-se necessitar de mais de uma rotação para reequilibrar a árvore.

![AVL depois da remoção e rotação, expandido1](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20remo%C3%A7%C3%A3o%20e%20rota%C3%A7%C3%A3o%2C%20expandido1.png)

No terceiro caso, a subárvore esquerda tem fator de equilíbrio -1.
Nesse caso, uma rotação simples não basta, necessitamos de uma rotação dupla.
A figura abaixo mostra o estado da árvore logo após a remoção do nó e detecção do desequilíbrio, a subárvore B expandida para melhor visualizar a rotação dupla e o estado final, após a rotação dupla.
O desenho mostra com a subárvore B tendo fator de equilíbrio 0, mas o resultado seria também equilibrado para os casos em que B tivesse 1 ou -1 (D ou E com altura h-2).
Como no caso anterior, a subárvore total ficou com altura menor que antes da remoção, o que pode acarretar a necessidade de outra rotação mais acima na árvore.

![AVL depois da remoção e rotação, expandido-1](https://github.com/BenhurUFSM/ed21a/blob/main/Complementos/ed21a-AVL%20depois%20da%20remo%C3%A7%C3%A3o%20e%20rota%C3%A7%C3%A3o%2C%20expandido-1.png)

Resumindo, na remoção, assim como na inclusão, pode-se resolver o desequilíbrio com uma rotação simples ou dupla no ponto onde o desequilíbrio é detectado, no retorno desde o nó inserido até o nó raiz.
No caso da remoção podem ser necessário realizar mais de uma rotação, e, como a rotação é feita no ramo da subárvore que não foi alvo da remoção, o nó abaixo do nó onde o desequilíbrio foi identificado (nó Y nas figuras) pode ter fator de equilíbrio 0, 1 ou -1 (no caso da inserção, somente podia ter 1 ou -1).
Pode-se reequilibrar a remoção com o mesmo código da inserção que decide que rotação aplicar, desde que trate corretamente o caso do fator de equilíbrio 0 no filho.

### Implementação

Um dado bastante usado na implementação de uma árvore AVL é o fator de equilíbrio de um nó.
Para calculá-lo, necessita-se saber a altura de seus dois filhos.
O cálculo da altura envolve o percurso completo das duas sub-árvores.
Para melhorar o desempenho de árvores AVL, é comum evitar esses percursos, armazenando em cada nó a sua altura. 
A altura de um nó só é alterada na inclusão, remoção e rotação.

Abaixo está um dos códigos de inclusão em uma ABB, com a árvore vazia sendo representada por um ponteiro nulo e a inclusão retornando o nó raiz (possivelmente alterado).

```c
    // inserção em uma ABB com árvore vazia sendo representada por ponteiro nulo
    //   versão com retorno da árvore
    arv_t *insere(arv_t *a, dado_t valor)
    {
       if (vazia(a)) {  // a == NULL
          a = cria_no(valor, NULL, NULL);
       } else if (valor == a->dado) {
          ; // ou reaje de outra forma para inserção de valor já existente
       } else if (valor < a->dado) {
          a->esq = insere(a->esq, valor);   // valor pequeno, insere na subárvore esquerda
       } else {
          a->dir = insere(a->dir, valor);   // valor grande, insere na subárvore direita
       }
       return a;
    }
 ```
 
Para uma árvore AVL, esse código deve ser modificado para alterar a altura da árvore, verificar o fator de equilíbrio e realizar as rotações caso necessário.
A função `cria_no()` deve ser alterada para inicializar o novo campo `altura` com o valor adequado a um nó folha (0).
Para a atualização da altura, cálculo do fator de equilíbrio e eventual reequilíbrio da árvore, adiciona-se, logo antes do comando `return`, uma chamada a uma função que realiza essas operações.
Essa função pode alterar a raiz dessa subárvore, então ela deve receber o valor da raiz e retorná-la, possivelmente alterada.
Então, antes do `return` coloca-se `a = avl_equilibra(a);` ou troca-se o `return a;` por `return avl_equilibra(a);`.
A nova função poderia ser:
 
 ```c
    arv_t *avl_equilibra(arv_t *a)
    {
       if (vazia(a)) return a;  // pode acontecer na remoção
       calcula_altura(a); // pode ter mudado pela alteração na árvore
       int fat_eq = fator_de_equilibrio(a);
       if (fat_eq >= -1 && fat_eq <= 1) {
          // está equilibrada -- nada a fazer
          return a;
       }
       // está desequilibrada -- o tipo de rotação depende do fator de equilibrio do filho mais alto
       if (fat_eq == 2) { // filho mais alto está na esquerda
          if (fator_de_equilibrio(a->esq) == -1) {
             return rotacao_dupla_a_esquerda(a);
          } else {
             return rotacao_simples_a_esquerda(a);
          }
       } else {  // filho mais alto está na direita
          // mesma coisa pro outro lado
       }
    }
 ```
 
 As funções auxiliares usadas acima:
 ```c
    int altura(arv_t *a)
    {
       if (vazia(a)) return -1;
       return a->altura;
    }
    
    void calcula_altura(arv_t *a)
    {
       int alt_esq = altura(a->esq);
       int alt_dir = altura(a->dir);
       a->altura = max(alt_esq, alt_dir) + 1;
    }
    
    int fator_de_equilibrio(arv_t *a)
    {
       return altura(a->esq) - altura(a->dir);
    }
    
    arv_t *rotacao_simples_a_esquerda(arv_t *a)
    {
       // cria variáveis auxiliares com as subárvores envolvidas (com nomes como na figura)
       arv_t *x = a;
       arv_t *y = x->esq;
       arv_t *b = y->dir;
       
       // faz a rotação
       x->esq = b;
       y->dir = x;

       // os dois nós envolvidos na rotação alteraram a altura -- recalcula
       // da forma como calcula_altura() funciona, tem que chamar de baixo para cima,
       //   e o nó mais abaixo agora é o "x"
       calcula_altura(x);
       calcula_altura(y);

       return y;  // y é a nova raiz
    }
    
    arv_t *rotacao_dupla_a_esquerda(arv_t *a)
    {
       a->esq = rotacao_simples_a_direita(a->esq);
       return rotacao_simples_a_esquerda(a);
    }
    
    arv_t *rotacao_simples_a_direita(arv_t *a)
    {
          // mesma coisa pro outro lado
    }
    
    arv_t *rotacao_dupla_a_direita(arv_t *a)
    {
          // mesma coisa pro outro lado
    }
 ```


O código da remoção pode ser como abaixo.

```c
   arv_t *remove(arv_t *a, dado_t valor)
   {
      if (vazia(a)) {
         ;                   // arvore vazia não tem o dado, não faz nada
      } else if (valor < a->dado) {
         a->esq = remove(a->esq, valor);      // valor pequeno, remove da esquerda
      } else if (valor > a->dado) {
         a->dir = remove(a->dir, valor);      // valor grande, remove da direita
      } else {
         if (folha(a)) {  // o caso fácil, transforma a em árvore vazia
            free(a);
            a = NULL;
         } else {  // o caso menos fácil, o nó tem filho(s)!
            dado_t novo_valor;  // este é o novo valor a colocar no nó e remover dos descendentes
            if (!vazio(a->esq)) { // preferencia pela esquerda, poderia ser pela direita, ou aleatório
               novo_valor = maior_valor(a->esq);
               a->esq = remove(a->esq, novo_valor);
            } else {
               novo_valor = menor_valor(a->dir);
               a->dir = remove(a->dir, novo_valor);
            }
            a->dado = novo_valor;
         }
      }
      return avl_equilibra(a);
   }
```
