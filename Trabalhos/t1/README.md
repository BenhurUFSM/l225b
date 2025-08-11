# t1

Implemente `strings` melhores em C.

Enquanto não vem uma descrição mais completa, implemente uma função que realiza a substituição de uma substring em uma string por outra string.
A função deve ter a assinatura abaixo:
```c
   bool strsubst(int n, char s[n], int pos, int tam, char *outra);
```
O vetor `s`, de tamanho `n` contém uma string (terminada por `'\0'`).
A função deve substituir a substring de tamanho `tam` que inicia pelo caractere que está na posição `pos` de `s` pela string que inicia em `outra`.
Se a substituição for bem sucedida, a função deve retornar `true`.
Caso contrário, deve retornar `false`. Motivos para isso:
- `s` não sem tamanho suficiente para conter o resultado
- `pos`/`tam` não constituem uma substring de `s`

A string em `s` pode ser vazia.
A substring pode ser vazia.
A string em `outra` pode ser vazia.

Suponha que `s` tenha tamanho 12, e conteúdo "abacaxi".
- A chamada `strsubst(12, s, 5, 2, "te")` deve substituir o valor em `s` por "abacate".
- A chamada `strsubst(12, s, 7, 0, ".")` deve substituir o valor em `s` por "abacaxi.".
- A chamada `strsubst(12, s, 3, 4, "")` deve substituir o valor em `s` por "aba".
- A chamada `strsubst(12, s, 0, 0, "Um ")` deve substituir o valor em `s` por "Um abacaxi".
- A chamada `strsubst(12, s, 0, 0, "Umas ")` deve não alterar o valor em `s` e retornar `false`.


## Parte I

Inicialmente, vamos implementar um grupo de funções para tratar strings constantes.
A descrição dessas funções está no arquivo [s1.h](Codigo/s1.h), com uma implementação
parcial (a ser completada por cada aluno) em [s1.c](Codigo/s1.c).
O arquivo [s1_tst.c](Codigo/s1_tst.c) contém um programa para testar as funções.

## Parte II

Complemente a implementação com funções que permitem alterar strings.
Nossas strings agora podem ser constantes ou alteráveis.
As strings alteráveis usam um vetor para conter os caracteres da string.

A descrição das funções está em [s2.h](Codigo/s2.h)

## Parte III

As strings agora farão o gerenciamento de sua memória, com alocação dinâmica conforme a necessidade.

Descrição em [ss.h](Codigo/ss.h).
