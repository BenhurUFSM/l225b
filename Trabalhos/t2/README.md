# t2 - parte I

Suporte a caracteres unicode codificados em UTF8 nas strings.
Estude o código fornecido, em `str.h`, `str.c` e `str_tst.c`.
A implementação completa de str está em str-i.c.
Caso encontre erros, por favor comunique.

# t2 - parte II

Implemente uma lista de strings, com a interface definida em `lstr.h`.
Use alocação contígua de memória, em que a lista tem um descritor (do tipo struct lstr) alocado dinamicamente quando a lista é criada e uma área de memória contígua também alocada dinamicamente onde são colocadas as strings (estruturas str) pertencentes à lista.
A área para as strings deve ser realocada conforme necessidade.
Pode não ter nada alocado para uma lista vazia, ou com alocação mínima.
A alocação mínima deve comportar 8 strings.
Em uma inserção, se a memória estiver cheia, a região deve ser realocada com tamanho 50% maior (se era 20, vira 30).
Em uma remoção, se a região ocupada for inferior à metade da capacidade, deve ser realocada com tamanho 25% maior que a ocupação (se tinha espaço para 20 e ficou com 9, realoca para 11).

# t2 - parte III

Reimplemente a lista de strings, com a mesma interface, usando lista encadeada.
Cada string deve ser colocada em um nó da lista, que é alocado e liberado conforme a necessidade. O descritor da lista contém ponteiro(s) para algum(ns) desses nós.
Um dos nós que deve ser apontado no descritor é o nó corrente.
Pode ser usado encadeamento simples ou duplo, mas as operações de avanço e recuo não devem realizar um percurso a partir de uma extremidade da lista, mas a partir da posição corrente.
A operação de posicionamento pode ser feita a partir da posição corrente ou a partir das pontas, dependendo da implementação (ou ainda otimizar e posicionar a partir do mais próximo).

Compare o desempenho das duas implementações: faça um programa que cria uma lista com um milhão de itens (podem ser todos strings vazias) (ou menos se ficar muito demorado).
Meça o tempo para inserir esses itens (meça com a inserção acontecendo no final da lista e com a inserção acontecendo no início).
Meça o tempo para percorrer a lista toda, do início ao fim e vice-versa.
Meça o tempo para acessar metade dos itens da lista, em posições aleatórias.
Meça o desempenho de operações de alteração (faça tantas vezes: sorteia uma posição aleatória dentro da lista, sorteia se vai fazer uma inserção ou remoção nessa posição, e faz)

Para medir o tempo, dá para usar a função clock:
```c
#include <time.h>

double agora()
{
    return (double)clock() / CLOCKS_PER_SEC;
}

int main()
{
    int n;
    double inicio, fim;
    printf("Digite um número, rápido ");
    inicio = agora();
    scanf("%d", &n);
    fim = agora();
    printf("Demorou %.3fs para digitar %d!\n", fim - inicio, n);
}
```
Meça também o consumo de memória do programa.
Dá para fazer isso com o valgrind:
```sh
valgrind --tool=massif ./meu_prog
ms_print massif.out.xxxxx
```
A cada execução do valgrind dessa forma, ele gera um arquivo com um nome diferente, que é usado pelo ms_print para fazer um relatório da execução. No comando ms_print acima, xxxxx é um número diferente a cada execução do valgrind. Cuidado para não escolher o arquivo errado.

# t2 - parte IV

breve

# RAP - respostas a perguntas

- aloca quanto?

   para um vetor de n inteiros:
   ```c
   int v[n];
   // ou
   int *v;
   v = malloc(n * sizeof(int));
   ```
   para um vetor de n estruturas do tipo xis:
   ```c
   xis v[n];
   // ou
   xis *v;
   v = malloc(n * sizeof(xis));
   ```
   para um vetor de n ponteiros para estruturas de tipo xis:
   ```c
   xis *v[n];
   // ou
   xis **v;
   v = malloc(n * sizeof(xis *));
   // ou ainda
   typedef xis *Xis;
   Xis v[n];
   // ou
   Xis *v;
   v = malloc(n * sizeof(Xis));
   ```

- como assim posições?

   suponha que tenha uma lista com 3 elementos, A, V e C. Nessa lista, tem 5 posições válidas, antes de A, a posição de A, a posição de V, a posição de C e a posição depois de C (pi, pa, pv, pc, pf).

   A função `ls_item_valido` nessa lista deve retornar `true` se a posição corrente for pa, pv ou pc, e `false` se for pi ou pf.

   Se nessa lista for inserido o item X, dependendo se a inserção for antes ou depois, e se a posição corrente for cada uma dessas 5 posições, a lista vai ficar como abaixo. Após a inserção, em todos os casos a posição corrente será px.
    | posição corrente | inserção antes | inserção depois |
    | :---: | :---: | :---: |
    | pi | XAVC | XAVC |
    | pa | XAVC | AXVC |
    | pv | AXVC | AVXC |
    | pc | AVXC | AVCX |
    | pf | AVCX | AVCX |
