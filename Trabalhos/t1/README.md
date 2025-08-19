# t1

Implemente `strings` melhores em C.

## Parte I

Inicialmente, vamos implementar um grupo de funções para tratar strings constantes.
A descrição dessas funções está no arquivo [bb1.h](Codigo/bb1.h), com uma implementação
parcial (a ser completada por cada aluno) em [bb1.c](Codigo/bb1.c).
O arquivo [bb1_tst.c](Codigo/bb1_tst.c) contém um programa para testar as funções.

## Parte II

Complemente a implementação com funções que permitem alterar strings.
Nossas strings agora podem ser constantes ou alteráveis.
As strings alteráveis usam um vetor para conter os caracteres da string.

A descrição das funções está em [bb2.h](Codigo/bb2.h)

## Parte III

As strings alteráveis agora farão o gerenciamento de sua memória, com alocação dinâmica conforme a necessidade.

Descrição em [cc.h](Codigo/cc.h).

## Como entregar

Crie um diretório chamado "l2-t1-*apelido*" (substitua "*apelido*" pelo seu apelido na planilha), coloque dentro o que quer enviar (`cc.c`), comprima o diretório (tgz, zip, etc), anexe a um mail para "benhur+l225b@inf.ufsm.br" com assunto "entrega l2-t2-*apelido*".

O prazo de entrega é 20ago. Envio dia 21 é depois do prazo. Envio dia 20 é no prazo.

Antes de entregar, compile e rode com a última versão de `cc_tst.c`.
Sugiro também rodar com `valgrind` para identificar eventuais erros de memória.

## RAP (Respostas às questões)

- Não entendi nada do que é pra fazer

   - copia cc.h, cc.c, cc_tst.c
   - lê em cc.h o que cada função deve fazer
   - implementa em cc.c o que não tiver implementado, de modo que cada função faça o que tem que fazer
   - compila o programa de teste
      - `gcc -Wall -o teste cc_tst.c cc.c`
      - se deu *warning* ou erro, corrige e volta a compilar
   - executa o programa de teste
      - `./teste`
      - se acusou algum erro, corrige e volta a compilar
   - executa com o valgrind
      - `valgrind ./teste`
      - se deu alguma mensagem de problema de memória, corrige e volta a compilar
      - em alguns casos, o valgrind diz que pode ser executado com mais opções para ter mais informação sobre o problema (como `valgrind    -leak-check=full` por exemplo)
   - cria um diretório `l2-t1-*fulano*`, se ainda não o fez; e coloca cc.c dentro
   - compacta esse diretório (por exemplo, `tar czf l2-t1-*fulano*.tgz l2-t1-fulano`)
   - anexa o arquivo compactado em um mail, coloca o assunto como pedido, envia para o endereço pedido
   - descansa
