#include "cc.h"
#include <stdio.h>

int num_teste = 0;

void inicia_teste(const char *nome)
{
  printf("\n======== '%s' ========\n", nome);
  num_teste = 0;
}

void verifica_cc(cc a, cc b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
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

void verifica_c(char a, char b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  if (a == b) {
    printf("OK\n");
  } else {
    printf("falhou, '%c' e '%c' são diferentes\n", a, b);
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
  cc a = cc_("tamanho8");
  verifica_int(cc_tam(a), 8);
  verifica_int(cc_tam(cc_("")), 0);
}

void testa_ch()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("abacaxi");
  verifica_c(cc_ch(a, 5), 'x');
  verifica_c(cc_ch(a, -1), 'i');
  verifica_c(cc_ch(a, 100), '\0');
  verifica_c(cc_ch(a, -3), 'a');
}

void testa_sub()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("barco");
  cc b = cc_sub(a, 2, 3);
  verifica_cc(b, cc_("rco"));

  b = cc_sub(a, 2, 4);
  verifica_cc(b, cc_("rco"));

  b = cc_sub(a, -6, 3);
  verifica_cc(b, cc_("ba"));

  b = cc_sub(a, 10, 5);
  verifica_cc(b, cc_(""));
}

void testa_copia()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("teste.!");
  cc b = cc_copia(a);
  verifica_cc(a, b);
  cc_destroi(b);
}

void testa_busca_c()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("coruja");
  cc b = cc_("xjr");
  int pos = cc_busca_c(a, 0, b);
  verifica_int(pos, 2);
  pos = cc_busca_c(a, 3, b);
  verifica_int(pos, 4);
  verifica_int(cc_busca_c(a, 1, cc_("ct")), -1);
}

void testa_busca_nc()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("margarida");
  cc b = cc_("magr");
  int pos = cc_busca_nc(a, 0, b);
  verifica_int(pos, 6);
  pos = cc_busca_nc(a, 7, b);
  verifica_int(pos, 7);
  verifica_int(cc_busca_nc(a, 8, b), -1);
}

void testa_busca_rc()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("coruja");
  cc b = cc_("lebre");
  int pos = cc_busca_rc(a, 5, b);
  verifica_int(pos, 2);
  pos = cc_busca_rc(a, 2, b);
  verifica_int(pos, 2);
  pos = cc_busca_rc(a, -1, b);
  verifica_int(pos, 2);
  pos = cc_busca_rc(a, 1, b);
  verifica_int(pos, -1);
  pos = cc_busca_rc(a, 100, cc_("ao"));
  verifica_int(pos, 5);
}

void testa_busca_rnc()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("canal");
  cc b = cc_("a");
  int pos = cc_busca_rnc(a, -1, b);
  verifica_int(pos, 4);
  pos = cc_busca_rnc(a, -2, b);
  verifica_int(pos, 2);
  pos = cc_busca_rnc(a, 1, b);
  verifica_int(pos, 0);
}

void testa_busca_s()
{
  inicia_teste(__FUNCTION__);
  cc a = cc_("remetente");
  int pos = cc_busca_s(a, 0, cc_("ente"));
  verifica_int(pos, 5);
  int pos2 = cc_busca_s(a, -7, cc_("te"));
  verifica_int(pos2, 4);
}

void testa_subst()
{
  inicia_teste(__FUNCTION__);
  cc c = cc_("abacaxi");
  cc d = cc_copia(c);

  cc_subst(&d, -9, 1, cc_("123"), '%');
  verifica_cc(d, cc_("123%abacaxi"));

  cc_destroi(d);
}

void testa_cat()
{
  inicia_teste(__FUNCTION__);
  cc c = cc_("espiral");
  cc d = cc_copia(c);

  cc_cat(&d, cc_(" bem longa"));
  verifica_cc(d, cc_("espiral bem longa"));
  cc_destroi(d);
}

void testa_catc()
{
  inicia_teste(__FUNCTION__);
  cc c = cc_("chocalho");
  cc d = cc_copia(c);

  cc_catc(&d, 's');
  verifica_cc(d, cc_("chocalhos"));
  cc_destroi(d);
}

void testa_insere()
{
  inicia_teste(__FUNCTION__);
  cc c = cc_("mostarda");
  cc d = cc_copia(c);

  cc_insere(&d, 2, cc_("haha"));
  verifica_cc(d, cc_("mohahastarda"));
  cc_destroi(d);
}

void testa_remove()
{
  inicia_teste(__FUNCTION__);
  cc c = cc_("semeadura");
  cc d = cc_copia(c);

  cc_remove(&d, 4, 2);
  verifica_cc(d, cc_("semeura"));
  cc_remove(&d, -2, 200);
  verifica_cc(d, cc_("semeu"));
  cc_destroi(d);
}

void testa_preenche()
{
  inicia_teste(__FUNCTION__);
  cc c = cc_("provisionar");
  cc d = cc_copia(c);

  cc_preenche(&d, 20, '$');
  verifica_cc(d, cc_("provisionar$$$$$$$$$"));
  cc_preenche(&d, 5, '$');
  verifica_cc(d, cc_("provisionar$$$$$$$$$"));
  cc_destroi(d);
}

void testa_arquivo()
{
  inicia_teste(__FUNCTION__);
  cc nome = cc_("dados");
  cc conteudo = cc_("conteúdo do arquivo\nlinha 2\nlinha 3 e última\n");
  cc s2lin = cc_sub(conteudo, 0, cc_busca_rc(conteudo, -2, cc_("\n"))+1);
  cc_grava_arquivo(s2lin, nome);
  cc a = cc_le_arquivo(nome);
  printf("===============\n");
  int p = 0, q = 0;
  while (p < cc_tam(a)) {
    q = cc_busca_c(a, p, cc_("\n"));
    if (q == -1) q = cc_tam(a);

    printf("%d: ", p);
    cc_imprime(cc_sub(a, p, q-p));
    printf("\n");
    p = q + 1;
  }
  printf("===============\n");
  verifica_cc(a, s2lin);
  cc_destroi(a);
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
  verifica_cc(cc_("aaavs/"), cc_("aaavs/"));
  testa_subst();
  testa_cat();
  testa_catc();
  testa_insere();
  testa_remove();
  testa_preenche();
  testa_arquivo();
}
