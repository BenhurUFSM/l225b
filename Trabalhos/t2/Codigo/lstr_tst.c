#include "lstr.h"
#include "str.h"
#include <stdio.h>

int main()
{
  s_grava_arquivo(s_("abacaxi\U0001f34d  \n abacate🥑, \ntomate\xf0\x9f\x8d\x85"), s_("dados"));
  str f = s_le_arquivo(s_("dados"));
  Lstr l = s_separa(f, s_("\n"));
  // imprime o arquivo
  int n = 0;
  for (ls_inicio(l); ls_avanca(l); ) {
    str s = ls_item(l);
    n++;
    printf("%d: nc=%d, nb=%d [", n, s.tamc, s.tamb);
    s_imprime(s);
    printf("]\n");
  }
  // altera as strings
  n = 0;
  for (ls_inicio(l); ls_avanca(l); ) {
    // as strings em l não são alteráveis -- remove, copia, insere
    // se tivesse algo como ls_substitui, não precisaria remover e inserir,
    //   poderia pegar o item, copiar, alterar e substituir
    // ou se tivesse um ls_item_ptr, para obter uma referência para a string,
    //   poderia alterá-la in loco
    str s = s_copia(s_apara(ls_remove(l), s_(", ")));
    n++;
    // podia ter um s_formata, para formatar str como printf
    char ss[20];
    sprintf(ss, "%d: [", n);
    s_insere(&s, 0, s_cria(ss));
    s_cat(&s, s_("]"));
    ls_insere_antes(l, s);
  }
  // as strings em l foram todas copiadas, dá para liberar f, que tem todo o
  //   conteúdo do arquivo
  s_destroi(f);
  ls_imprime(l);

  // as strings em l são alteráveis, tem que detruí-las antes de destruir a lista
  // bem que podia ter uma versão ls_destroi que destroi destruindo as strings...
  for (ls_inicio(l); ls_avanca(l); ) {
    s_destroi(ls_item(l));
  }
  ls_destroi(l);
}
