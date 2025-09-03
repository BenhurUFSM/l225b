## Dicionário, tabela *hash*

### Dicionário

O TAD **dicionário**, também chamado de **mapa** e **vetor associativo**, modela uma coleção de dados, em que cada dado está associado a uma *chave*.
O dicionário permite encontrar o dado associado a uma determinada chave que tenha sido anteriormente inserido no mesmo.
Em geral, não é permitido que mais de um dado diferente corresponda a uma mesma chave (embora tenham implementações em que isso é possível).

As operações mais comuns de um dicionário são:
- inserção(chave, valor) - insere no dicionário o valor, associado à chave;
- consulta(chave) - obtém o valor correspondente a determinada chave (ou uma informação que diz que essa chave é desconhecida);
- remoção(chave) - remove a chave (e o valor correspondente) do dicionário; em geral retorna o valor removido (ou uma informação que diz que essa chave é desconhecida).

#### Implementação de dicionário

São várias as possibilidades de implementação desse TAD.
Uma implementação bem simples é o uso de uma lista, contendo pares chave-valor, e o uso de busca sequencial pela chave na lista para a implementação das operações. O desempenho não vai ser muito bom, mas pode ser razoável, se a enorme maioria das operações esperadas for de inserção e aceita-se ter mais de um valor por chave ou sabe-se que as chaves são sempre inéditas. A operação de inserção nesse caso seria inserção no início ou final da lista, e as operações que necessitam consulta (bem menos frequentes) fariam busca linear na lista.

Uma outra implementação com busca bem mais otimizada é um vetor, com os dados ordenados por chave. A busca pode ser feita por busca binária, com desempenho proporcional a log<sub>2</sub>*n* em um vetor com *n* chaves. A inserção e remoção necessitam movimentação de dados em memória, para manter os dados ordenados no vetor.
Pode ser uma implementação razoável para casos em que a operação de consulta é bem mais frequente que operações de inserção e remoção.

A busca binária pode ser feita assim:
```c
typedef struct tupla {
  chave_t chave;
  valor_t valor;
} tupla;

int acha_pos(int n, tupla v[n], chave_t chave)
{
  int ini = 0; // posição inicial onde a chave pode estar no vetor
  int fim = n; // posição logo após região onde chave pode estar
  while (ini < fim) {
    int meio = (ini + fim) / 2;  // se fim == ini+1, isso dá ini
    switch (compara_chaves(v[meio].chave, chave)) {
      case igual: return meio;
      case menor: ini = meio + 1;
                  break;
      case maior: fim = meio;
                  break;
    }
  }
  return NAO_TEM;
}
```
Ou, com um versão recursiva:
```c
int acha_pos(tupla v[], int ini, int fim, chave_t chave)
{
  if (fim <= ini) return NAO_TEM;
  int meio = (ini + fim) / 2;  // se fim == ini+1, isso dá ini
  switch (compara_chaves(v[meio].chave, chave)) {
    case igual: return meio;
    case menor: return acha_pos(v, meio + 1, fim, chave);
    case maior: return acha_pos(v, ini, meio, chave);
  }
}
```

Outras possibilidades incluem implementar o dicionário com uma árvore de busca (que veremos logo), ou ainda como uma tabela *hash*, descrita a seguir.
  
### Tabela *hash*

Também chamada de tabela de dispersão, uma tabela *hash* é um vetor que contém um conjunto de dados, cada um deles associado a uma chave única.
A posição de cada dado no vetor é calculada por uma função, que transforma a chave do dado em um inteiro, que é o índice do vetor onde esse dado está.
Essa função se chama função *hash* ou função de dispersão, e é parte importante no desempenho de uma tabela *hash*.
Em geral, a posição é calculada em 2 fases, a função *hash* produz um número inteiro qualquer (potencialmente grande), e depois esse número é limitado ao tamanho do vetor (usualmente como o resto da divisão do número *hash* pelo número de elementos do vetor).

Em alguns casos raros, consegue-se uma função *hash* ótima, em que cada chave leva a um índice diferente do vetor, mas em geral não é assim, e existem conflitos, em que chaves diferentes resultam no mesmo índice.
Caso mais de uma chave com o mesmo índice tenham que ser colocadas na tabela, é necessário alguma forma de resolver esses conflitos.

As duas formas principais de resolver conflitos são: 
- usar uma lista para cada entrada na tabela *hash* para manter todos os dados que têm o mesmo *hash*;
- colocar o dado com conflito em outra posição da tabela (na posição seguinte, por exemplo).

No primeiro caso, uma busca na tabela envolve calcular o *hash*, e buscar (ou inserir, ou remover) o dado na lista correspondente.

Em pseudo-código, as operações seriam mais ou menos assim:
```c
void hash_insere(h, chave, valor) {
  lista l = h->tabela[hash(chave)]
  lista_insere(l, chave, valor)
}
val_t hash_busca(h, chave) {
  lista l = h->tabela[hash(chave)]
  return lista_busca(l, chave)
}
```

No segundo caso, tem uma sequência de posições a procurar (nos índices seguintes, ou calculado por outro *hash*).
A busca de uma chave na tabela vira uma busca sequencial dentro do vetor, a partir da posição calculada pela função *hash* até encontrar a chave procurada ou até encontrar uma posição vazia ou até percorrer todo o vetor, caso a tabela esteja cheia (sem esquecer de voltar ao início do vetor se chegar no fim).
Esse método só funciona bem se a tabela tiver um *fator de carga* (posições ocupadas dividido pelo tamanho da tabela) baixo.
Na remoção, não se pode marcar a posição do dado removido como vazia (a não ser que a posição seguinte esteja vazia), porque isso pode interromper uma busca futura, Em vez disso, usa-se um valor especial para marcar posição livre mas que já foi ocupada.
No código abaixo, essas duas condições são codificadas por valores especiais de chave, "vazia" e "livre".

Em pseudo-código, poderia ser algo como:
```c
void hash_insere(h, chave, valor) {
  // falta testar se a tabela tá cheia ou com mais itens do que um limite (e reagir)
  int pos = hash(chave)
  int pos_livre = -1    // posicao de insercao
  for (int i = 0; i < h->tamanho; i++) {
    if h->tabela[pos].chave == chave
      // a chave já existe -- substitui o valor para chave já existente
      //   outra possibilidade seria dar erro, ou retornar o valor antigo
      h->tabela[pos].valor = valor
      return
    if h->tabela[pos] == vazia
      // chegou numa posição vazia, a chave não existe
      // se não achou uma posição livre antes, insere aqui.
      if (pos_livre == -1) pos_livre = pos
      break
    if h->tabela[pos].chave == livre && pos_livre == -1
      // "livre" é o valor que representa uma posição que já esteve ocupada
      // se uma chave igual não for encontrada, a posição de inserção é a primeira livre
      // salva a posição e continua a busca
      pos_livre = pos
    // calcula a próxima posição
    pos = (pos + 1) % h->tamanho
  }
  // a busca terminou sem encontrar a chave.
  // se uma posição livre foi encontrada, insere o novo valor nessa posição
  if pos_livre != -1
    h->tabela[pos_livre].chave = chave
    h->tabela[pos_livre].valor = valor
    h->n_elem++
  else
    // a tabela tá cheia!
}

val_t hash_remove(h, chave) {
  int pos = hash(chave)
  for (int i = 0; i < h->tamanho; i++) {
    if h->tabela[pos].chave == chave
      h->tabela[pos]. chave = livre
      return h->tabela[pos].valor
    if h->tabela[pos] == vazia // chegou numa posição vazia, a chave não existe
      break
    // calcula a próxima posição
    pos = (pos + 1) % h->tamanho
  }
  return NAO_TEM
}
```
Em uma tabela *hash* dinâmica, não se sabe de antemão o tamanho ideal para a tabela. Em geral resolve-se o problema alterando a capacidade da tabela quando o fator de carga estiver acima ou abaixo de algum limite (acima de 1/2 na inclusão ou abaixo de 1/8 na remoção, por exemplo). Nesse caso, tem que criar uma nova tabela, inserir todos os elementos da tabela antiga na nova, destruir a antiga e seguir com a nova, porque o valor da função *hash* das chaves é dependente do tamanho da tabela, e os dados devem mudar de posição.

