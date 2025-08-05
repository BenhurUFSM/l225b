#include "cc.h"
#include <assert.h>
#include <stdio.h>

void testa_arquivo()
{
  cc a = cc_le_arq(cc_("cc_tst.c"));
  int p = 0, q = 0;
  while (p < cc_tam(a)) {
    q = cc_busca_ch(a, p, cc_("\n"));
    if (q == -1) q = cc_tam(a);

    printf("%d: ", p);
    cc_print(cc_sub(a, p, q-p));
    printf("\n");
    p = q + 1;
  }
  cc_destroi(a);
}

void println(cc c)
{
  putchar('[');
  cc_print(c);
  printf("](%d;%d)", cc_tam(c), c.cap);
  putchar('\n');
}

void testa_subst()
{
  cc c = cc_("teste.");
  cc d = cc_copia(c);
  assert(cc_igual(c, d));
  cc e = cc_sub(c, 1, 3);
  println(e);
  assert(cc_igual(e, cc_("est")));
  cc_subst(&d, 3, 10, e, 'X');
  println(d);
  assert(cc_igual(d, cc_("tesest")));
  cc_subst(&d, cc_tam(d), 10, cc_("Y"), 'B');
  println(d);
  assert(cc_igual(d, cc_("tesestY")));
  cc_subst(&d, 0, 0, cc_("a"), 'B');
  println(d);
  assert(cc_igual(d, cc_("atesestY")));
  cc_subst(&d, cc_busca_ch(d, 0, cc_("se")), 1, cc_(""), 0);
  println(d);
  assert(cc_igual(d, cc_("atsestY")));
  cc f = cc_sub(d, -2, 20);
  println(f);
  assert(cc_igual(f, cc_("tY")));
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
