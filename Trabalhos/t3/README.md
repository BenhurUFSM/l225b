# t3 - fila de prioridades

O Código anexo implementa um jogo de digitação de palavras, usando uma lista de prioridades.
Para compilá-lo, necessita lstr, str, tela do trabalho anterior.

O arquivo fila-prio.h tem a interface de uma fila de prioridade.
Nessa fila, os dados são colocados em qualquer ordem, e retirados por ordem de prioridade.

A fila é genérica, pode ser usada com qualquer tipo de dados.
Para isso, na criação da fila deve ser informado o tamanho de um dado, em bytes.
Na inserção, informa-se um ponteiro para o dado a inserir, a fila copia tantos bytes desse local para o interios da fila.
Na remoção, informa-se um ponteiro para o local onde o dado removido deve ser colocado; a fila copia os tantos bytes para essa posição.

A fila não conhece o tipo de dados com o qual ela está lidando, mas para poder escolher o mais prioritário, tem que ao menos poder comparar dois desses dados.
Para isso, deve ser fornecido na inicialização da fila um ponteiro para uma função de comparação. Essa função deve receber como argumentos dois ponteiros para void, que apontam para dois dados na fila, e retornar um int que diz qual tem mais prioridade.
O retorno é um número positivo se o primeiro dado tem maior prioridade que o segundo, negativo se o primeiro for menos prioritário que o segundo e zero se ambos tiverem a mesma prioridade.

O arquivo fila-prio.c implementa a fila de prioridade, em uma lista, usando um alocação contígua de memória. Uma inserção na fila insere no final da lista e uma remoção faz uma busca entre todos os elementos da lista para encontrar o mais prioritário.
Em uma remoção, o dado no final da lista é deslocado para ocupar o local do dado removido, para manter a lista contígua.

Você deve reimplementar a fila de prioridade como uma árvore heap, mantendo a mesma interface (fila-prio.c deve ser alterado, fila-prio.h não).

Compare o desempenho das duas implementações.

