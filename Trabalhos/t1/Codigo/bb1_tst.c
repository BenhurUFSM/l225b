#include "bb1.h"
#include <assert.h>
#include <stdio.h>

void testa_iguais(bb a, bb b)
{
  if (bb_igual(a, b)) {
    printf("teste OK\n");
  } else {
    printf("teste falhou, '");
    bb_imprime(a);
    printf("' e '");
    bb_imprime(b);
    printf("' são diferentes\n");
  }
}
void teste_sub()
{
  bb a = bb_("teste.");
  bb b = bb_sub(a, 1, 3);
  testa_iguais(b, bb_("est"));
}

int main()
{
  teste_sub();
}
