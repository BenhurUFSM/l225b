### Recursão

Uma função recursiva é uma função que chama ela própria para realizar parte de seu trabalho.
Por exemplo, uma função que imprime um número em decimal poderia realizar essa impressão usando uma função que imprime o número que corresponde aos primeiros dígitos e outra que imprime o último dígito:
```c
  void imp_num(int n)
  {
     imp_os_dígitos(n / 10);
     imp_o_digito(n % 10);
  }
```
Precisamos agora de uma função que imprime os dígitos de um número, mas é exatamente isso que se espera que a função imp_num faça! Vamos usar ela então:
```c
  void imp_num(int n)
  {
     imp_num(n / 10);
     imp_num(n % 10);
  }
```
Parece que não tem como isso funcionar! E não funciona mesmo, afinal não tem nenhum comando para imprimir alguma coisa. Vamos alterar para realizar a impressão direta do caso mais simples, que é quando se tem um só dígito:
```c
  void imp_num(int n)
  {
     imp_num(n / 10);
     putchar('0' + n % 10);
  }
```
Ainda não vai funcionar, porque a primeira coisa que a função faz é chamar ela mesma, e quando for chamada vai chamar de novo e de novo etc, não termina nunca.
Dá para resolver não chamando a função a não ser que espere que a função chamada imprima alguma coisa. Se o valor a imprimir for menor que 10, só tem que imprimir um dígito, que é impresso pela segunda linha, então não tem que chamar a função para imprimir mais dígitos, porque não tem mais dígitos para imprimir:
```c
  void imp_num(int n)
  {
     if (n >= 10) {
       imp_num(n / 10);
     }
     putchar('0' + n % 10);
  }
```
Agora sim!

Uma função recursiva tem que ter 3 características:
- tem que ter alguma chamada a si mesma (mesmo que indiretamente, através de outra função)
- tem que ter alguma entrada que não cause a recursão (com a qual ela não chame a si mesma)
- na chamada a si mesma, deve passar como argumento um valor mais próximo daquele que não causa a recursão

Outro exemplo, cálculo do fatorial:
```c
  long long fat(int n)
  {
     if (n == 0) return 1;  // caso sem chamada recursiva
     return n * fat(n - 1); // chama com um valor mais próximo de 0
  }
```

## Exercício

1. Faça uma função para calcular e retornar o MDC entre 2 números, usando o método euclidiano:
   - MDC entre dois números quaisquer é igual ao MDC entre o segundo desses números e o resto da divisão do primeiro pelo segundo
   - MDC entre um número x qualquer e zero é o número x

