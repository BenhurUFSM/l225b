#include "cc.h"
#include <assert.h>
#include <stdio.h>

void testa_arquivo()
{
  cc a = cc_le_arquivo(cc_("cc_tst.c"));
  int p = 0, q = 0;
  while (p < cc_tam(a)) {
    q = cc_busca_c(a, p, cc_("\n"));
    if (q == -1) q = cc_tam(a);

    printf("%d: ", p);
    cc_imprime(cc_sub(a, p, q-p));
    printf("\n");
    p = q + 1;
  }
  cc_destroi(a);
}

void println(cc c)
{
  putchar('[');
  cc_imprime(c);
  printf("](%d;%d)", cc_tam(c), c.cap);
  putchar('\n');
}

void testa_iguais(cc a, cc b)
{
  if (cc_igual(a, b)) {
    printf("teste OK\n");
  } else {
    printf("teste falhou, '");
    cc_imprime(a);
    printf("' e '");
    cc_imprime(b);
    printf("' são diferentes\n");
  }
}
void testa_subst()
{
  cc c = cc_("teste.");
  cc d = cc_copia(c);
  testa_iguais(c, d);
  cc e = cc_sub(c, 1, 3);
  println(e);
  testa_iguais(e, cc_("est"));
  cc_subst(&d, 3, 10, e, 'X');
  println(d);
  testa_iguais(d, cc_("tesest"));
  cc_subst(&d, cc_tam(d), 10, cc_("Y"), 'B');
  println(d);
  testa_iguais(d, cc_("tesestY"));
  cc_subst(&d, 0, 0, cc_("a"), 'B');
  println(d);
  testa_iguais(d, cc_("atesestY"));
  cc_subst(&d, cc_busca_c(d, 0, cc_("se")), 1, cc_(""), 0);
  println(d);
  testa_iguais(d, cc_("atsestY"));
  cc f = cc_sub(d, -2, 20);
  println(f);
  testa_iguais(f, cc_("tY"));
  printf("\n[%s]\n[%.*s]\n[%.*s]\n[%s]\n", d.mem, d.tam, d.mem, e.tam, e.mem, e.mem);
  printf("\n[%.*s]%d\n", f.tam, f.mem, f.tam);
	cc_destroi(c);
	cc_destroi(d);
}

int main()
{
  testa_arquivo();
  testa_subst();
}
