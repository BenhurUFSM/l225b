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

Continue o editor de textos fornecido.
O editor usa lista de string para representar as linhas do texto em memória.
O código está em (p4)[Codigo/p4].

Foram necessárias algumas alterações no código anterior:
- str: o código referente a utf8 foi colocado em novos arquivos, utf8.h e utf8.c
- str: foi adicionada uma nova função, s_cria_buf, para criar uma str à partir do conteúdo de um buffer, que não precisa conter uma string C terminada em \0. Para isso, a função recebe o tamanho do conteúdo do buffer, em bytes e em caracteres.
- lstr: ls_destroi: deve destruir as strings da lista, além de destruir a lista
- lstr: ls_item: deve retornar a string não alterável, isso pode ser obtido chamando s_sub com a string inteira
- lstr: nova função ls_item_ptr, que retorna um ponteiro para a string na posição corrente. Isso é necessário para poder alterar as strings da lista sem remover e reincluir.

Código novo:
- utf8.ch - nem tão novo, são as funções de tratamento de codificação unicode utf8 que foram movidas de str.c para cá. Isso porque algumas são necessárias fora de str.c.
- tela.ch - funções para uso da tela (posicionamento do cursor, seleção de cores, entrada de dados não bloqueante, uma tecla por vez, com um pouco de tratamento de teclas especiais)
- ed.c - o código do editor. As funções que estão com o corpo "{}" são as que devem ser implementadas para a parte 4.

O editor é um editor modal, baseado no funcionamento do editor "vi".
Ele tem os seguintes modos de funcinamento:
- normal - as teclas de letras correspondem a comandos
- inserção - as teclas de letras são inseridas no texto
- troca - as teclas de letra substituem os caracteres no texto
- troca1 - como troca, mas volta ao modo normal após uma substituição
- seleção - quando entra nesse modo, a posição do cursor é salva (essa posição é chamada âncora), e as teclas de letra correspondem a comandos. Nesse modo, parte do texto é selecionado, entre a âncora e o cursor. Alguns dos comandos movem o cursor, outros operam sobre a seleção. Existem 2 modos:
   - seleção de linha - a seleção ignora a coluna da âncora e do cursor, só seleciona linhas inteiras
   - seleção de caracteres - a seleção usa a informação de coluna também.

Em todos os modos as teclas dedicadas à movimentação do cursor funcionam ± como esperado: setas, home, end, pgup, pgdn.

As teclas a seguir movem o cursor, e funcionam nos modos normal e de seleção:
- h - move à esquerda
- j - move para baixo
- k - move para cima
- l - move para a direita
- espaço - move para a direita
- 0 ou ^ - move para o início da linha (no vi, ^ move para o primeiro não espaço)
- $ - move para o final da linha
- g - move para a primeira linha (no vi, g é um prefixo, gg move para a primeira linha)
- G - move para a última linha
- w ou W - move para o início da próxima palavra (no vi, W considera palavras separadas por espaço, w palavras só com letras, não foi implementada essa distinção)
- b ou B - move para o início da palavra anterior
- e ou E - move para o final da palavra

No modo normal, tem ainda:
- q - termina a execução
- i - passa para o modo de inserção
- v - passa para o modo de seleção por caractere
- V - passa para o modo de seleção por linha
- r - passa para o modo troca1
- R - passa para o modo troca
- a - avança um caractere e passa para o modo de inserção
- A - passa para o modo de inserção no final da linha
- I - passa para o modo de inserção no início da linha
- o - abre uma linha abaixo da linha do cursor e passa para o modo de inserção nela
- O - abre uma linha acima da linha do cursor e passa para o modo de inserção nela
- J - cola a linha seguinte no final da linha corrente
- x ou del - apaga o caractere sob o cursor
- p - cola a seleção que tenha sido copiada anteriormente após o cursor. Se a seleção foi feita em modo caractere, copia a partir do caractere onde está o cursor, senão em nova linha abaixo do cursor.
- P - cola a seleção copiada antes do cursor

No modo de inserção, caracteres imprimíveis são inseridos na posição do cursor, e além disso:
- esc - muda para o modo normal
- enter - quebra a linha na posição do cursor
- backspace - remove o caractere antes do cursor
- del - remove o caractere no cursor.

No modo troca, caracteres imprimíveis substituem o caractere no cursor, e
- esc - muda para o modo normal

No modo seleção,
- v - muda para a seleção por caractere se estiver no modo de seleção por linha
- V - muda para a seleção por linha se estiver no modo de seleção por caractere
- o - troca a âncora pelo cursor (para permitir alterar o outro lado da seleção)
- y - copia a seleção
- c - copia e remove a seleção e passa para modo de inserção
- d ou x - copia e remove a seleção, passa para modo normal
- p ou P - substitui a seleção pela seleção copiada

Favor informar bugs encontrados.

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


- como criar uma string com um caractere unicode (parte 4)
   ```c
   byte b[4];
   str s = s_uni(b, codigo_do_caractere);
   // a função s_uni está em ed.c
   // a string s referencia os bytes em b. se precisar se s após a destruição
   //   de b, faça uma cópia com s_copia.

   ```
