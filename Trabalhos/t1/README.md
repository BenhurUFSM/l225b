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

- Não entendi muito bem o que é pra fazer

   - copia cc.h, cc.c, cc_tst.c
   - lê em cc.h o que cada função deve fazer
   - implementa em cc.c o que não tiver implementado, de modo que cada função faça o que tem que fazer
   - compila o programa de teste
      - `gcc -Wall -o teste cc_tst.c cc.c`
      - se deu *warning* ou erro, corrige e volta a compilar
   - executa o programa de teste
      - `./teste`
      - se acusou algum erro, corrige e volta a compilar
   - coloca mais algum teste em cc_tst.c, caso queira testar mais casos; compila etc
   - executa com o valgrind
      - `valgrind ./teste`
      - se deu alguma mensagem de problema de memória, corrige e volta a compilar
      - em alguns casos, o valgrind diz que pode ser executado com mais opções para ter mais informação sobre o problema (como `valgrind    -leak-check=full` por exemplo)
   - cria um diretório `l2-t1-*fulano*`, se ainda não o fez; e coloca cc.c (e o cc_tst, caso tenha colocado mais testes) dentro
   - compacta esse diretório (por exemplo, `tar czf l2-t1-*fulano*.tgz l2-t1-fulano`)
   - anexa o arquivo compactado em um mail, coloca o assunto como pedido, envia para o endereço pedido
   - descansa

- Pra que serve o pontoagá

   Em programas maiores, é necessário um cuidado maior com a organização do código, para mantê-lo sob controle.
   É comum separar o código de um programa em mais de um arquivo, para que a organização física reflita a modularização lógica.

   A compilação em C é realizada um arquivo por vez (se chama de compilação separada).
   Para cada arquivo a compilar, o compilador parte do zero, e só acessa o que está no arquivo .c que ele está compilando.
   Então, para que o código em um arquivo tenha acesso a algo que esteja em outro, tem-se que informar o compilador.
   Por exemplo, suponha que na função `f`, que está no arquivo `a.c` se queira fazer uma chamada à função `g`, que está no arquivo `b.c`.
   ```c
   /*** a.c ***/
   // ...
   void f() {
     // ...
     y = g(5);
     // ...
   }
   // ...
   ```
   ```c
   /*** b.c ***/
   // ...
   int g(int x) {
     // ...
   }
   // ...
   ```
   Quando o compilador estiver compilando `a.c`, ele não tem acesso ao conteúdo de `b.c`.
   Então, quando ele chegar à linha que tem uma chamada a `g`, será a primeira vez que vê esse nome, e isso gera um erro, porque para poder compilar uma chamada de função, é necessário que o compilador saiba, além do nome da função, o número de argumentos, o tipo de cada um e o tipo do valor de retorno da função.
   Podemos fornecer essa informação, declarando o função `g` antes de chamá-la em `a.c`:
   ```c
   /*** a.c ***/
   // ...
   int g(int);
   // ...
   void f() {
     // ...
     y = g(5);
     // ...
   }
   // ...
   ```
   Agora, quando o compilador chegar à linha que contém a chamada a `g`, já terá passado pela linha que contém a declaração de `g`, e a compilação poderá seguir sem erros.
   Mas essa solução não é muito escalável. Toda função que está em um módulo deve ser declarada em todos os demais módulos que a usam.
   Alterar a declaração de uma função para mudar o número ou tipo dos argumentos exige um trabalho enorme.

   Para atenuar esses problemas, se coloca em um arquivo extra todas as informações de um módulo que se deseja que sejam acessívei pelos demais módulos, e se instrui o compilador a incluir esse arquivo onde for necessário. A forma como geralmente se faz isso é criar um arquivo com o mesmo nome do arquivo que contém o módulo, com a extensão `.h`.
   No exemplo, seria criado um arquivo `b.h` para conter as declarações das coisas que estão no arquivo `b.c` que gostaríamos que os demais módulos tenham acesso.
   Nos arquivos que contém módulos que acessam essas coisas, incluímos esse arquivo.
   ```c
   /*** b.h ***/
   // ...
   int g(int);
   // ...
   ```
   ```c
   /*** a.c ***/
   // ...
   #include "b.h"
   // ...
   void f() {
     // ...
     y = g(5);
     // ...
   }
   // ...
   ```
   Incluímos o `b.h` também no `b.c`, para que quando esse arquivo for compilado o compilador possa verificar se a declaração que foi vista pelos demais módulos corresponde ao que estimplementado.
   ```c
   /*** b.c ***/
   // ...
   #include "b.h"
   // ...
   int g(int x) {
     // ...
   }
   // ...
   ```
