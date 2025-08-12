#include "bb1.h"
#include <assert.h>
#include <stdio.h>

void testa_iguais(bb1 a, bb1 b)
{
  if (bb1_igual(a, b)) {
    printf("teste OK\n");
  } else {
    printf("teste falhou, '");
    bb1_imprime(a);
    printf("' e '");
    bb1_imprime(b);
    printf("' são diferentes\n");
  }
}
void teste_sub()
{
  bb1 a = bb1_("teste.");
  bb1 b = bb1_sub(a, 1, 3);
  testa_iguais(b, bb1_("est"));
}

int main()
{
  teste_sub();
}
