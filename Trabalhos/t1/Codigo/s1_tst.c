#include "s1.h"
#include <assert.h>
#include <stdio.h>

void testa_iguais(s1 a, s1 b)
{
  if (s1_equal(a, b)) {
    printf("teste OK\n");
  } else {
    printf("teste falhou, '");
    s1_print(a);
    printf("' e '");
    s1_print(b);
    printf("' são diferentes\n");
  }
}
void teste_sub()
{
  s1 a = s1_("teste.");
  s1 b = s1_sub(a, 1, 3);
  testa_iguais(b, s1_("est"));
}

int main()
{
  teste_sub();
}
