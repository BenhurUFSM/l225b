#include "cc.h"
#include <assert.h>
#include <stdio.h>

void testa_arquivo()
{
  char *nome = "cc_tst.c";
  printf("===============\n");
  cc a = cc_le_arquivo(cc_cria(nome));
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
  printf("===============\n");
  printf("Verifique se o texto acima corresponde ao conteúdo de '%s'\n\n", nome);
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
  static int n = 0;
  n++;
  printf("teste %d: ", n);
  if (cc_igual(a, b)) {
    printf("OK\n");
  } else {
    printf("falhou, '");
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
  cc e;
  testa_iguais(c, d);
  e = cc_sub(c, -4, 2);
  testa_iguais(e, cc_("st"));
  e = cc_sub(c, 1, 3);
  testa_iguais(e, cc_("est"));
  cc_subst(&d, 3, 10, e, 'X');
  testa_iguais(d, cc_("tesest"));
  cc_subst(&d, cc_tam(d), 10, cc_("Y"), 'B');
  testa_iguais(d, cc_("tesestY"));
  cc_subst(&d, 0, 0, cc_("a"), 'B');
  testa_iguais(d, cc_("atesestY"));
  cc_subst(&d, cc_busca_c(d, 0, cc_("se")), 1, cc_(""), 0);
  testa_iguais(d, cc_("atsestY"));
  cc_subst(&d, cc_busca_rc(d, 0, cc_("se")), 1, cc_(""), 0);
  testa_iguais(d, cc_("atsetY"));
  e = cc_sub(d, -2, 20);
  testa_iguais(e, cc_("tY"));
	cc_destroi(c);
	cc_destroi(d);
}

int main()
{
  testa_arquivo();
  testa_subst();
}
