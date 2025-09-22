## Fila de prioridade

Uma **fila de prioridade** é uma estrutura de dados em que os dados são inseridos em uma ordem qualquer, e são retirados em ordem de prioridade. 
A prioridade é um valor que é obtido à partir do dado, ou um valor que é inserido associado ao dado.

Uma implementação simples de uma fila de prioridade seria uma lista, em que cada dado é inserido no final da lista, e uma operação de remoção faz uma busca linear entre todos os dados, para encontrar aquele que tem maior prioridade, que é então retirado da lista.
Uma outra possibilidade seria manter os dados na lista ordenados por prioridade, realizando a inserção de forma a manter essa ordenação e a remoção na ponta da lista onde está o dado de maior prioridade.
Outra possibilidade ainda é colocar os dados em uma árvore de busca, em que o maior dado é facilmente encontrado.
Nas primeiras implementações, uma das operações é bastante custosa, na terceira implementação, tem um custo médio para ambas operações. Deve haver uma forma mais barata de implementar uma fila de prioridade, porque não necessitamos saber sempre a ordem total entre os elementos na fila.

Uma implementação de fila de prioridade que tem um desempenho bom em ambas as operações, e que é comumente utilizada para isso é a estrutura ***heap***.

### Heap

Uma ***heap*** é uma árvore em que a prioridade do nó raiz não é inferior à prioridade do nó raiz de nenhuma de suas subárvores, que também são *heap*.
Dessa forma, o nó raiz contém o nó de maior prioridade da árvore inteira.
Existem várias implementações de *heap*, uma das mais comuns é a binária.

Uma ***heap* binária** é uma árvore com a característica *heap* vista acima, que além disso é uma árvore *quase completa*, ou seja, uma árvore em que todos os níveis estão completos (com todos os nós possíveis), exceto talvez o último, e, nesse caso, os nós do último nível são todos colocados à esquerda, no início do nível.

Uma árvore binária quase completa pode ser facilmente mapeada para um vetor, numerando-se os nós por nível e da esquerda para a direita; e essa numeração correspondendo aos índices do vetor. O nó raiz fica no índice 0, seus filho esquerdo no índice 1 e direito no 2, os filhos de 1 em 3 e 4, os filhos de 2 em 5 e 6, os filhos de 3 em 7 e 8 e assim por diante. Caso a árvore ganhe um novo nó, ele deve ser colocado no final do vetor (para manter a forma de quase completa); caso um nó seja retirado, deve perder a última posição do vetor.
Seguindo a numeração acima, os filhos do nó que está na posição *i* do vetor devem estar nas posições *i\*2+1* e *i\*2+2* (se essas posições estiverem além do final do vetor, tal filho não existe), e seu pai na posição *(i-1)/2*, ignorando o resto da divisão (exceto o nó 0, que não tem pai).

Uma heap poderia ser implementada em C com o tipo abaixo:
```c
   typedef struct heap heap_t;
   struct heap {
     dado_t *vet;   // vetor dinamicamente alocado para os dados
     int n;         // número de nós atualmente no heap (em dados[0] até dados[n-1])
     // ...         // mais informações (por exemplo, tamanho da área alocada, para
                    //   saber quando realocar)
   };
```

As principais operações de uma *heap* são inserção e remoção de um nó, e ambas devem manter as duas características da *heap*.

#### Inserção

No caso da **inserção**, a árvore vai ganhar um novo nó, e para manter a forma de árvore quase completa, esse nó deve ser o próximo do último nível ou o primeiro à esquerda em um novo nível (esse nó fica na primeira posição após o final atual do vetor). A inserção nesse local pode invalidar a característica *heap* da árvore, caso a prioridade do nó inserido seja superior à de seu nó pai. Logo, em uma inserção a árvore deve ser verificada e corrigida. Felizmente, a correção é simples: basta percorrer o caminho ascendente desde o ponto de inserção em direção à raiz, deslocando os valores para baixo até encontrar um que tenha prioridade superior à do nó inserido.

A inserção pode ser implementada em C assim:
```c
void heap_insere(heap_t *h, dado_t d)
{
  heap_insere_no_final_realocando_se_necessario(h, d);
  heap_corrige_subindo(h, h->n-1);
}
```
A correção verifica se a condição heap está ok entre o nó informado e seu pai, e troca os valores se o filho tiver mais prioridade que o pai. 
Caso o dado tenha trocado de lugar, a correção deve continuar em sua nova posição. Em C:
```c
void heap_corrige_subindo(heap_t *h, int pos)
{
  if (pos == 0) return;           // se o dado está na raiz, não tem o que corrigir
  int ppos = pai(pos);            // posição do pai do nó em pos
  if (prio_maior(h->vet[pos], h->vet[ppos])) {
    // a prioridade do filho é maior que a do pai, precisa corrigir
    heap_troca(h, pos, ppos);     // troca os dados entre pai e filho
    heap_corrige_subindo(h, ppos);// continua a verificação no nó pai
  }
}
```
Uma otimização possível é primeiro baixar os valores com prioridade inferior e só depois inserir o novo valor, evitando as trocas, que implicam em potencialmente a cópia do novo valor em vários nós da árvore. Em C, poderia ser algo assim:
```c
void heap_insere(heap_t *h, dado_t d)
{
  heap_realoca_se_necessario_para_incluir_mais_um(h);
  int pos = h->n;                 // primeira posicao livre no vetor -- a busca
                                  //   por uma posição para d começa aqui
  while (true) {
    if (pos == 0) break;          // na raiz, nada a fazer
    int ppos = pai(pos);          // posição do pai do nó em pos
    if (prio_maior(h->vet[ppos], d)) {
      break;                      // o pai tem prioridade maior, pos é a posição
                                  //   procurada para o novo valor
    }
    h->vet[pos] = h->vet[ppos];   // desce o valor que tem menor prioridade
    pos = ppos;                   // continua onde estava o valor menor
  }
  h->vet[pos] = d;                // coloca o valor na posicao encontrada
  h->n++;                         // atualiza o número de dados
}
```

#### Remoção

Na **remoção**, o valor a remover está na raiz, que está no primeiro nó do vetor, mas a árvore deve perder o último nó pra continuar com a forma correta.
O dado que está no último nó deve ser colocado em outro local da árvore, mas deve ser um local em que a ordem *heap* não seja violada.
A solução é partir da raiz (que perde seu dado), escolhendo para colocar nela o dado com maior prioridade entre seus filhos e o dado sendo relocado.
Se for escolhido o dado de algum dos filhos, continua-se com a mesma operação nesse nó filho, e continua dessa forma até que seja escolhido o dado sendo relocado. No pior dos casos, se chega até um nó folha.

Em C poderia ser algo como:
```c
dado_t heap_remove(heap_t *h)
{
  if (h->n == 0) return NAO_TEM;   // ou outra reação mais drástica
  dado_t ret = h->vet[0];          // salva o valor da raiz -- vai ser retornado mais tarde
  int pos_r = h->n-1;              // posicao do dado a recolocar
  if (pos_r != 0) {                // a heap não ficou vazia
    h->vet[0] = h->vet[pos_r];     // coloca o dado do final na raiz
    h->n--;                        // tem um elemento a menos
    heap_corrige_descendo(h, 0);   // corrige a condição heap, se necessário
  } else {
    h->n--;
  }
  return ret;
}
```
A correção de um nó nesse caso tem que verificar se o valor do nó tem maior prioridade que os filhos e se não for o caso, subir o valor do filho com maior prioridade:
```c
void heap_corrige_descendo(heap_t *h, int pos)
{
  int pos_m = pos;                  // posição com maior prio
  // vê se algum filho tem prio maior
  int pos_e = filho_esquerdo(pos);
  if (pos_e < h->n && prio_maior(h->vet[pos_e], h->vet[pos_m])) pos_m = pos_e;
  int pos_d = filho_direito(pos);
  if (pos_d < h->n && prio_maior(h->vet[pos_d], h->vet[pos_m])) pos_m = pos_d;
  if (pos_m != pos) {
    heap_troca(h, pos, pos_m);      // troca os dados entre pai e filho
    heap_corrige_descendo(h, pos_m);// continua a verificação no nó filho
  }
}
```
Uma otimização possível, eliminando as trocas (e a recursão):
```c
dado_t heap_remove(heap_t *h)
{
  if (h->n == 0) return NAO_TEM;   // ou outra reação mais drástica
  dado_t ret = h->vet[0];          // salva o valor na raiz -- vai ser retornado mais tarde
  int pos_r = h->n-1;              // posicao do dado a recolocar
  int pos = 0;                     // posicao sem valor válido
  while (true) {
    int pos_e = filho_esquerdo(pos);
    int pos_d = filho_direito(pos);
    int pos_m = heap_pos_maior_prio(h, pos_r, pos_e, pos_d);
    h->vet[pos] = h->vet[pos_m];   // o dado de maior prio vai para a posicao vazia
    if (pos_m == pos_r) break;     // o dado que foi pro lugar livre é o que precisava ser relocado
    pos = pos_m;                   // o local onde estava o mais prioritário agora tá vazio -- segue
  }
  h->n--;                          // tem um dado a menos
  return ret;
}
```
A função que calcula a posição que tem o dado com maior prioridade deve verificar se as posições recebidas são válidas.
Pode ser algo assim:
```c
int heap_pos_maior_prio(heap_t *h, int pos_r, int pos_e, int pos_d)
{
  // pos_r é a última posição válida
  int pos_m = pos_r;    // chuta que o maior é o que tá no final
  if (pos_e < pos_r && prio_maior(h->vet[pos_e], h->vet[pos_m])) pos_m = pos_e;   // filho esq tem > prio
  if (pos_d < pos_r && prio_maior(h->vet[pos_d], h->vet[pos_m])) pos_m = pos_d;   // filho dir tem > prio
  return pos_m;
}
```
