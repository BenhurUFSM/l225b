#include "str.h"
#include <stdio.h>

int num_teste = 0;

void inicia_teste(const char *nome)
{
  printf("\n======== '%s' ========\n", nome);
  num_teste = 0;
}

void verifica_str(str a, str b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  if (s_igual(a, b)) {
    printf("OK\n");
  } else {
    printf("falhou, '");
    s_imprime(a);
    printf("' e '");
    s_imprime(b);
    printf("' são diferentes\n");
  }
}

void verifica_c(unichar a, unichar b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  if (a == b) {
    printf("OK\n");
  } else {
    printf("falhou, '%x' e '%x' são diferentes\n", a, b);
  }
}

void verifica_int(int a, int b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  if (a == b) {
    printf("OK\n");
  } else {
    printf("falhou, '%d' e '%d' são diferentes\n", a, b);
  }
}


void testa_tam()
{
  inicia_teste(__FUNCTION__);
  str a = s_("tamanho8");
  verifica_int(s_tam(a), 8);
  verifica_int(s_tam(s_("")), 0);
}

void testa_ch()
{
  inicia_teste(__FUNCTION__);
  str a = s_("abacaxi");
  verifica_c(s_ch(a, 5), 'x');
  verifica_c(s_ch(a, -1), 'i');
  verifica_c(s_ch(a, 100), UNI_INV);
  verifica_c(s_ch(a, -3), 'a');
}

void testa_sub()
{
  inicia_teste(__FUNCTION__);
  str a = s_("bárco");
  str b = s_sub(a, 2, 3);
  verifica_str(b, s_("rco"));

  b = s_sub(a, 2, 4);
  verifica_str(b, s_("rco"));

  b = s_sub(a, -6, 3);
  verifica_str(b, s_("bá"));

  b = s_sub(a, 10, 5);
  verifica_str(b, s_(""));
}

void testa_copia()
{
  inicia_teste(__FUNCTION__);
  str a = s_("teste.!");
  str b = s_copia(a);
  verifica_str(a, b);
  s_destroi(b);
}

void testa_busca_c()
{
  inicia_teste(__FUNCTION__);
  str a = s_("coruja");
  str b = s_("xjr");
  int pos = s_busca_c(a, 0, b);
  verifica_int(pos, 2);
  pos = s_busca_c(a, 3, b);
  verifica_int(pos, 4);
  verifica_int(s_busca_c(a, 1, s_("ct")), -1);
}

void testa_busca_nc()
{
  inicia_teste(__FUNCTION__);
  str a = s_("margarida");
  str b = s_("magr");
  int pos = s_busca_nc(a, 0, b);
  verifica_int(pos, 6);
  pos = s_busca_nc(a, 7, b);
  verifica_int(pos, 7);
  verifica_int(s_busca_nc(a, 8, b), -1);
}

void testa_busca_rc()
{
  inicia_teste(__FUNCTION__);
  str a = s_("coruja");
  str b = s_("lebre");
  int pos = s_busca_rc(a, 5, b);
  verifica_int(pos, 2);
  pos = s_busca_rc(a, 2, b);
  verifica_int(pos, 2);
  pos = s_busca_rc(a, -1, b);
  verifica_int(pos, 2);
  pos = s_busca_rc(a, 1, b);
  verifica_int(pos, -1);
  pos = s_busca_rc(a, 100, s_("ao"));
  verifica_int(pos, 5);
}

void testa_busca_rnc()
{
  inicia_teste(__FUNCTION__);
  str a = s_("canal");
  str b = s_("a");
  int pos = s_busca_rnc(a, -1, b);
  verifica_int(pos, 4);
  pos = s_busca_rnc(a, -2, b);
  verifica_int(pos, 2);
  pos = s_busca_rnc(a, 1, b);
  verifica_int(pos, 0);
}

void testa_busca_s()
{
  inicia_teste(__FUNCTION__);
  str a = s_("remetente");
  int pos = s_busca_s(a, 0, s_("ente"));
  verifica_int(pos, 5);
  int pos2 = s_busca_s(a, -7, s_("te"));
  verifica_int(pos2, 4);
}

void testa_subst()
{
  inicia_teste(__FUNCTION__);
  str c = s_("abácaxi");
  str d = s_copia(c);
  str e = s_("%-");

  s_subst(&d, 5, 2, s_("te"), e);
  verifica_str(d, s_("abácate"));
  s_subst(&d, 0, s_tam(d), s_(""), e);
  verifica_str(d, s_(""));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, 7, 0, s_("."), e);
  verifica_str(d, s_("abácaxi."));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, 9, 0, s_("."), e);
  verifica_str(d, s_("abácaxi%-."));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, 9, 0, s_(""), e);
  verifica_str(d, s_("abácaxi%-"));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, 10, 50, s_("."), e);
  verifica_str(d, s_("abácaxi%-%."));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, -3, 0, s_("123"), e);
  verifica_str(d, s_("abác123axi"));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, 0, 1, s_("123"), e);
  verifica_str(d, s_("123bácaxi"));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, -9, 1, s_("123"), e);
  verifica_str(d, s_("123-abácaxi"));
  s_subst(&d, 0, s_tam(d), c, e);
  s_subst(&d, 2, 200, s_(""), e);
  verifica_str(d, s_("ab"));

  s_destroi(d);
}

void testa_cat()
{
  inicia_teste(__FUNCTION__);
  str c = s_("espiral");
  str d = s_copia(c);

  s_cat(&d, s_(" bem longa"));
  verifica_str(d, s_("espiral bem longa"));
  s_destroi(d);
}

void testa_catc()
{
  inicia_teste(__FUNCTION__);
  str c = s_("chocalho");
  str d = s_copia(c);

  s_cat(&d, s_("s"));
  verifica_str(d, s_("chocalhos"));
  s_destroi(d);
}

void testa_insere()
{
  inicia_teste(__FUNCTION__);
  str c = s_("mostarda");
  str d = s_copia(c);

  s_insere(&d, 2, s_("haha"));
  verifica_str(d, s_("mohahastarda"));
  s_destroi(d);
}

void testa_remove()
{
  inicia_teste(__FUNCTION__);
  str c = s_("semeadura");
  str d = s_copia(c);

  s_remove(&d, 4, 2);
  verifica_str(d, s_("semeura"));
  s_remove(&d, -2, 200);
  verifica_str(d, s_("semeu"));
  s_destroi(d);
}

void testa_preenche()
{
  inicia_teste(__FUNCTION__);
  str c = s_("provisionar");
  str d = s_copia(c);

  s_preenche(&d, 20, s_("$"));
  verifica_str(d, s_("provisionar$$$$$$$$$"));
  s_preenche(&d, 5, s_("$"));
  verifica_str(d, s_("provisionar$$$$$$$$$"));
  s_destroi(d);
}

void testa_arquivo()
{
  inicia_teste(__FUNCTION__);
  str nome = s_("dados");
  str conteudo = s_("conteúdo do arquivo\nlinha 2\nlinha 3 e última\n");
  str s2lin = s_sub(conteudo, 0, s_busca_rc(conteudo, -2, s_("\n"))+1);
  s_grava_arquivo(s2lin, nome);
  str a = s_le_arquivo(nome);
  printf("===============\n");
  int p = 0, q = 0;
  while (p < s_tam(a)) {
    q = s_busca_c(a, p, s_("\n"));
    if (q == -1) q = s_tam(a);

    printf("%d: ", p);
    s_imprime(s_sub(a, p, q-p));
    printf("\n");
    p = q + 1;
  }
  printf("===============\n");
  verifica_str(a, s2lin);
  s_destroi(a);
}

int main()
{
  testa_tam();
  testa_ch();
  testa_sub();
  testa_copia();
  testa_busca_c();
  testa_busca_nc();
  testa_busca_rc();
  testa_busca_rnc();
  testa_busca_s();
  verifica_str(s_("aaavs/"), s_("aaavs/"));
  testa_subst();
  testa_cat();
  testa_catc();
  testa_insere();
  testa_remove();
  testa_preenche();
  testa_arquivo();
}
