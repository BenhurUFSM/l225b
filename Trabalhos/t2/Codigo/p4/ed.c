#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tela.h"
#include "str.h"
#include "lstr.h"

// tipos e funções auxiliares {{{1

// este é um editor modal. Estes são os modos possíveis
// normal: as teclas de letras são comandos (em geral de movimentação ou troca de modo)
// insercao: as teclas de letras são incluídas no texto
// troca: as teclas de letras substituem as que estão no texto
// troca1: só uma substituição é feita, volta automaticamente ao modo normal
// selecao_caractere: salva a posição do cursor quando inicia esse modo (âncora)
//   as teclas movem o cursor como no modo normal, o texto entre a âncora e o
//   cursor é selecionado; alguns comandos agem sobre essa seleção
// selecao_linha: como seleção, mas a seleção não considera as colunas, sempre
//   seleciona linhas inteiras
typedef enum { normal, insercao, troca, troca1, selecao_caractere, selecao_linha } modo_t;

// coloca em buf a conversão de uni para utf8 e retorna uma str com isso
str s_uni(byte *buf, unichar uni)
{
  int nbytes = u8_converte_pra_utf8(uni, buf);
  return s_cria_buf(buf, nbytes, 1);
}

// troca as strings em lista por cópias alteráveis
void faz_lista_ter_copias_das_strings(Lstr lista)
{
  for (ls_inicio(lista); ls_avanca(lista); ) {
    str *pcad = ls_item_ptr(lista);
    *pcad = s_copia(*pcad);
  }
}

// tipo para representar uma posição no texto ou na tela
typedef struct {
  int lin;
  int col;
} posicao_t;

// tipo para representar o tamanho de um retângulo na tela
typedef struct {
  int alt;
  int larg;
} tamanho_t;

// retorna o menor entre dois inteiros
static int menor(int a, int b)
{
  if (a <= b) return a;
  return b;
}

// retorna o maior entre dois inteiros
static int maior(int a, int b)
{
  if (a >= b) return a;
  return b;
}

// retorna qual das duas posições está antes da outra
static posicao_t pos_antes(posicao_t a, posicao_t b)
{
  if (a.lin < b.lin) return a;
  if (b.lin < a.lin) return b;
  if (a.col <= b.col) return a;
  return b;
}

// retorna qual das duas posições está depois da outra
static posicao_t pos_depois(posicao_t a, posicao_t b)
{
  if (a.lin > b.lin) return a;
  if (b.lin > a.lin) return b;
  if (a.col >= b.col) return a;
  return b;
}

// texto_t {{{1

// o conteúdo de um arquivo, como uma lista contendo suas linhas
typedef struct {
  Lstr linhas;
  str nome_arquivo;
  // bool alterado;
} texto_t;

// aloca e inicializa um texto à partir de um arquivo
texto_t *texto_cria(str nome_arquivo)
{
  texto_t *txt = malloc(sizeof(*txt));
  assert(txt != NULL);
  txt->nome_arquivo = s_copia(nome_arquivo);
  str conteudo = s_le_arquivo(nome_arquivo);
  txt->linhas = s_separa(conteudo, s_("\n"));
  faz_lista_ter_copias_das_strings(txt->linhas);
  s_destroi(conteudo);
  return txt;
}

void texto_destroi(texto_t *txt)
{
  s_destroi(txt->nome_arquivo);
  ls_destroi(txt->linhas);
  free(txt);
}

// janela_t {{{1

// estrutura que contém os dados sobre uma janela
// uma janela representa parte da tela, que mostra uma parte de um texto
// operações na janela podem alterar a região do texto que é mostrada,
//   além de permitir alterações no texto
// tem uma posição na janela que é o cursor, a posição do texto onde
//   a maior parte das operações pode ser realizada
// tem uma posição que é a âncora, para quando tem uma região do texto
//   selecionada
typedef struct {
  texto_t *txt;
  posicao_t inicio_tela; // em que posição da tela está o início da janela
  tamanho_t tamanho;     // quantas linhas e colunas tem a janela
  posicao_t inicio_txt;  // que posição do texto está no início da janela
  posicao_t cursor_txt;  // em que posição do texto está o cursor
  posicao_t ancora;      // a seleção é entre a âncora e o cursor
  // bool visivel;
} janela_t;

// regiões da tela podem ser apresentadas em cores diferentes;
// estas são as cores usadas
typedef enum {cor_externa, cor_externa_sel, cor_texto, cor_texto_sel, cor_status } jan_cor_t;
void jan_cor(jan_cor_t cor)
{
  switch (cor) {
    case cor_externa: tela_cor_fundo(20, 20, 20); tela_cor_letra(60, 80, 80); break;
    case cor_externa_sel: tela_cor_fundo(20, 20, 20); tela_cor_letra(175, 200, 200); break;
    case cor_texto: tela_cor_fundo(0, 0, 0); tela_cor_letra(200, 230, 230); break;
    case cor_texto_sel: tela_cor_fundo(40, 40, 40); tela_cor_letra(200, 230, 230); break;
    case cor_status: tela_cor_fundo(175, 200, 200); tela_cor_letra(50, 20, 20); break;
  }
}


// aloca e inicializa uma janela para o texto txt
janela_t *jan_cria(texto_t *txt)
{
  janela_t *jan = malloc(sizeof(*jan));
  assert(jan != NULL);
  jan->txt = txt;
  jan->cursor_txt = (posicao_t){0,0};
  jan->inicio_txt = (posicao_t){0,0};
  jan->inicio_tela = (posicao_t){1,1};
  jan->tamanho = (tamanho_t){tela_nlin(), tela_ncol()};
  return jan;
}

void jan_destroi(janela_t *jan)
{
  free(jan);
}

// desenho do texto na janela

// mostra o número da linha, à esquerda do conteúdo da linha
void jan_desenha_numero_da_linha(int num_linha, bool selecionada)
{
  char buf[7];
  sprintf(buf, "%5d ", (unsigned)(num_linha + 1) % 100000);
  jan_cor(selecionada ? cor_externa_sel : cor_externa);
  s_imprime(s_cria(buf));
}

// desenha uma linha, ressaltando os linhas que fazem parte da seleção, quando
//   a seleção está no modo linha
static void jan_desenha_linha_selecao_linha(janela_t *jan, int num_linha, str linha)
{
  int lin_ini = menor(jan->cursor_txt.lin, jan->ancora.lin);
  int lin_fim = maior(jan->cursor_txt.lin, jan->ancora.lin);
  if (num_linha < lin_ini || num_linha > lin_fim) {
    jan_cor(cor_texto);
  } else {
    jan_cor(cor_texto_sel);
  }
  str visivel = s_sub(linha, jan->inicio_txt.col, jan->tamanho.larg - 6);
  s_imprime(visivel);
  tela_limpa_fim_da_linha();
}

// desenha uma linha quando está em modo seleção caractere
static void jan_desenha_linha_selecao_caractere(janela_t *jan, int num_linha, str linha)
{
  // se não é uma linha onde tem uma mudança de seleção, desenha igual a sel_lin
  if (num_linha != jan->cursor_txt.lin && num_linha != jan->ancora.lin) {
    return jan_desenha_linha_selecao_linha(jan, num_linha, linha);
  }
  posicao_t pos_ini = pos_antes(jan->cursor_txt, jan->ancora);
  posicao_t pos_fim = pos_depois(jan->cursor_txt, jan->ancora);
  int larg = jan->tamanho.larg - 6;
  // colunas: i-início da linha  f-fim da linha
  // cor normal entre i e ts, selecionado entre ts e st, normal entre st e f
  int col_i = jan->inicio_txt.col;
  int col_f = menor(s_tam(linha), larg - 1);
  int col_ts = col_i;
  int col_st = col_f;
  if (num_linha == pos_ini.lin) {
    col_ts = maior(col_ts, pos_ini.col);
  }
  if (num_linha == pos_fim.lin) {
    col_st = menor(pos_fim.col, col_st);
  }
  // texto normal antes
  if (col_i < col_ts) {
    jan_cor(cor_texto);
    s_imprime(s_sub(linha, col_i, col_ts - col_i));
  }
  // texto na selecao
  if (col_ts <= col_st) {
    jan_cor(cor_texto_sel);
    s_imprime(s_sub(linha, col_ts, col_st - col_ts + 1));
  }
  // texto normal depois
  if (col_st < col_f) {
    jan_cor(cor_texto);
    s_imprime(s_sub(linha, col_st + 1, col_f - col_st));
  }
  tela_limpa_fim_da_linha();
}

// desenha a linha num_linha do texto na janela, no modo modo
static void jan_desenha_linha(janela_t *jan, int num_linha, str linha, modo_t modo)
{
  jan_desenha_numero_da_linha(num_linha, num_linha == jan->cursor_txt.lin);
  if (modo == selecao_caractere) {
    return jan_desenha_linha_selecao_caractere(jan, num_linha, linha);
  } else if (modo == selecao_linha) {
    return jan_desenha_linha_selecao_linha(jan, num_linha, linha);
  }
  jan_cor(cor_texto);
  str visivel = s_sub(linha, jan->inicio_txt.col, jan->tamanho.larg - 6);
  s_imprime(visivel);
  tela_limpa_fim_da_linha();
}

// desenha toda a janela
void jan_desenha(janela_t *jan, modo_t modo)
{
  // desenha as linhas do texto
  Lstr linhas = jan->txt->linhas;
  for (int i = 0; i < jan->tamanho.alt - 1; i++) {
    tela_lincol(jan->inicio_tela.lin + i, jan->inicio_tela.col);
    int num_linha = i + jan->inicio_txt.lin;
    ls_posiciona(linhas, num_linha);
    if (ls_item_valido(linhas)) {
      str linha = ls_item(linhas);
      jan_desenha_linha(jan, num_linha, linha, modo);
    } else {
      // tá fora do texto
      jan_cor(cor_externa);
      s_imprime(s_("~"));
      tela_limpa_fim_da_linha();
    }
  }
  // desenha linha de estado
  tela_lincol(jan->inicio_tela.lin + jan->tamanho.alt - 1, jan->inicio_tela.col);
  char *str_modo;
  switch (modo) {
    case normal: str_modo = " N "; break;
    case troca: str_modo = " R "; break;
    case troca1: str_modo = " R "; break;
    case insercao: str_modo = " I "; break;
    case selecao_caractere: str_modo = " V "; break;
    case selecao_linha: str_modo = "V-L"; break;
  }
  jan_cor(cor_status);
  printf(" %s | %d:%d | ", str_modo, jan->cursor_txt.lin + 1, jan->cursor_txt.col + 1);
  s_imprime(jan->txt->nome_arquivo);
  tela_limpa_fim_da_linha();
  // posiciona o cursor
  tela_lincol(jan->inicio_tela.lin + (jan->cursor_txt.lin - jan->inicio_txt.lin),
              jan->inicio_tela.col + (jan->cursor_txt.col - jan->inicio_txt.col) + 6);
}

// retorna a strinm na linha onde está o cursor
str jan_linha_corrente(janela_t *jan)
{
  ls_posiciona(jan->txt->linhas, jan->cursor_txt.lin);
  return ls_item(jan->txt->linhas);
}

// move o cursor uma coluna à esquerda
void jan_cursor_esquerda(janela_t *jan)
{
  jan->cursor_txt.col--;
}

// move o cursor para o início da linha
void jan_cursor_inicio_linha(janela_t *jan)
{
  jan->cursor_txt.col = 0;
}

// move o cursor para a primeira linha do texto
void jan_cursor_inicio_texto(janela_t *jan)
{
  jan->cursor_txt.lin = 0;
}

// move o cursor uma posição à direita
void jan_cursor_direita(janela_t *jan)
{
  jan->cursor_txt.col++;
}

// move o cursor para a última posição da linha
void jan_cursor_final_linha(janela_t *jan)
{
  str lin = jan_linha_corrente(jan);
  jan->cursor_txt.col = s_tam(lin);
}

// move o cursor para a última linha do texto
void jan_cursor_final_texto(janela_t *jan)
{
  Lstr linhas = jan->txt->linhas;
  jan->cursor_txt.lin = ls_tam(linhas) - 1;
}

// move o cursor para a linha anterior
void jan_cursor_cima(janela_t *jan)
{
  jan->cursor_txt.lin--;
}

// move o cursor para a linha seguinte
void jan_cursor_baixo(janela_t *jan)
{
  jan->cursor_txt.lin++;
}

// move o cursor uma página (3/4 da altura da janela) para cima
void jan_cursor_pg_cima(janela_t *jan)
{
  jan->cursor_txt.lin -= jan->tamanho.alt * 3 / 4;
}

// move o cursor uma página (3/4 da altura da janela) para baixo
void jan_cursor_pg_baixo(janela_t *jan)
{
  jan->cursor_txt.lin += jan->tamanho.alt * 3 / 4;
}

// considerando palavra como qualquer coisa separada por espaços
// move o cursor para o início da próxima palavra (o primeiro caractere
// não espaço após um espaço)
void jan_cursor_inicio_palavra_direita(janela_t *jan)
{
  str lin = jan_linha_corrente(jan);
  int pos = s_busca_c(lin, jan->cursor_txt.col, s_(" "));
  if (pos == -1) {
    if (jan->cursor_txt.lin >= ls_tam(jan->txt->linhas)) return;
    jan->cursor_txt.lin++;
    lin = jan_linha_corrente(jan); 
    pos = 0;
  }
  pos = s_busca_nc(lin, pos, s_(" "));
  jan->cursor_txt.col = pos;
}

// move o cursor para o início anterior de uma palavra
// (o primeiro caractere não espaço após um espaço, em uma posição anterior)
void jan_cursor_inicio_palavra_esquerda(janela_t *jan)
{
  str lin = jan_linha_corrente(jan);
  int pos = jan->cursor_txt.col - 1;
  if (pos != -1) pos = s_busca_rnc(lin, pos, s_(" "));
  if (pos == -1) {
    if (jan->cursor_txt.lin == 0) return;
    jan->cursor_txt.lin--;
    lin = jan_linha_corrente(jan); 
    pos = s_busca_rnc(lin, s_tam(lin), s_(" "));
  }
  pos = s_busca_rc(lin, pos, s_(" "));
  jan->cursor_txt.col = pos + 1;
}

// move o cursor para o final da palavra (o primeiro caractere não espaço
// seguido por um espaço ou pelo final de linha após o cursor)
void jan_cursor_final_palavra_direita(janela_t *jan)
{
  str lin = jan_linha_corrente(jan);
  int pos = jan->cursor_txt.col + 1;
  if (pos >= s_tam(lin)) pos = -1;
  else pos = s_busca_nc(lin, pos, s_(" "));
  if (pos == -1) {
    if (jan->cursor_txt.lin == ls_tam(jan->txt->linhas)) return;
    jan->cursor_txt.lin++;
    lin = jan_linha_corrente(jan); 
    pos = s_busca_nc(lin, 0, s_(" "));
  }
  pos = s_busca_c(lin, pos, s_(" "));
  if (pos == -1) pos = s_tam(lin);
  else pos--;
  jan->cursor_txt.col = pos;
}

// posiciona a âncora de seleção no posição atual do cursor
void jan_define_ancora(janela_t *jan)
{
  jan->ancora = jan->cursor_txt;
}

// troca as posições âncora <-> cursor
void jan_troca_ancora(janela_t *jan)
{
  posicao_t ancora = jan->ancora;
  jan->ancora = jan->cursor_txt;
  jan->cursor_txt = ancora;
}

// insere uma linha vazia abaixo da linha do cursor
void jan_abre_linha_abaixo(janela_t *jan) {}
// insere uma linha vazia acima da linha do cursor
void jan_abre_linha_acima(janela_t *jan) {}
// quebra a linha na posição do cursor (o conteúdo da linha do cursor
//   a partir da posição do cursor é movido para uma nova linha)
void jan_quebra_linha(janela_t *jan) {}
// a linha abaixo do cursor é removida, e seu conteúdo é concatenado à
//   linha do cursor
void jan_junta_linhas(janela_t *jan) {}
// remove o caractere sob o cursor
void jan_remove_char(janela_t *jan) {}
// altera o caractere sob o cursor para ter o valor de uni
void jan_altera_char(janela_t *jan, unichar uni) {}
// insere o caractere com o valor de uni logo antes do caractere do cursor
void jan_insere_char(janela_t *jan, unichar uni) {}

// remove o caractere à esquerda do cursor, se houver
void jan_remove_char_esquerda(janela_t *jan)
{
  if (jan->cursor_txt.col > 0) {
    jan_cursor_esquerda(jan);
    jan_remove_char(jan);
  }
}

// retorna uma lista com o conteúdo da seleção, quando sel_lin
static Lstr jan_copia_selecao_linhas(janela_t *jan)
{
  int ini = menor(jan->cursor_txt.lin, jan->ancora.lin);
  int fim = maior(jan->cursor_txt.lin, jan->ancora.lin);
  Lstr linhas = jan->txt->linhas;
  ls_posiciona(linhas, ini);
  Lstr sel = ls_cria();
  for (int i = ini; i <= fim; i++) {
    ls_insere_depois(sel, ls_item(linhas));
    ls_avanca(linhas);
  }
  return sel;
}

// retorna uma lista da seleção, quando é uma seleção em caracteres
//   que está toda em uma linha
static Lstr jan_copia_selecao_1linha(janela_t *jan)
{
  int lin = jan->cursor_txt.lin;
  int col_ini = menor(jan->cursor_txt.col, jan->ancora.col);
  int col_fim = maior(jan->cursor_txt.col, jan->ancora.col);
  Lstr linhas = jan->txt->linhas;
  ls_posiciona(linhas, lin);
  str linha = ls_item(linhas);
  Lstr sel = ls_cria();
  str linha_sel = s_sub(linha, col_ini, col_fim - col_ini + 1);
  ls_insere_depois(sel, linha_sel);
  return sel;
}
// retorna uma lista com o conteúdo da seleção, no modo dado
Lstr jan_copia_selecao(janela_t *jan, modo_t modo)
{
  if (modo == selecao_linha) return jan_copia_selecao_linhas(jan);
  if (jan->cursor_txt.lin == jan->ancora.lin) return jan_copia_selecao_1linha(jan);
  // seleção por caracteres, em linhas diferentes
  posicao_t pos_ini = pos_antes(jan->cursor_txt, jan->ancora);
  posicao_t pos_fim = pos_depois(jan->cursor_txt, jan->ancora);
  Lstr sel = ls_cria();
  Lstr linhas = jan->txt->linhas;

  // pega o final da primeira linha
  ls_posiciona(linhas, pos_ini.lin);
  str linha = ls_item(linhas);
  str linha_sel = s_sub(linha, pos_ini.col, s_tam(linha));
  ls_insere_depois(sel, linha_sel);
  // pega as linhas intermediárias
  ls_avanca(linhas);
  for (int i = pos_ini.lin + 1; i <= pos_fim.lin - 1; i++) {
    ls_insere_depois(sel, ls_item(linhas));
    ls_avanca(linhas);
  }
  // pega o início da última linha
  linha = ls_item(linhas);
  linha_sel = s_sub(linha, 0, pos_fim.col + 1);
  ls_insere_depois(sel, linha_sel);
  return sel;
}

// remove as linhas entre linha_inicial e linha_final, inclusive
static void jan_remove_linhas(janela_t *jan, int linha_inicial, int linha_final)
{
  Lstr linhas = jan->txt->linhas;
  ls_posiciona(linhas, linha_inicial);
  for (int i = linha_inicial; i <= linha_final; i++) {
    str linha = ls_remove(linhas); // a posição corrente avança
    s_destroi(linha);
  }
}

// remove a seleção do texto, no modo dado
void jan_remove_selecao(janela_t *jan, modo_t modo)
{
  if (modo == selecao_linha) {
    // remove as linhas entre o cursor e a âncora (da menor pra maior)
    int ini = menor(jan->cursor_txt.lin, jan->ancora.lin);
    int fim = maior(jan->cursor_txt.lin, jan->ancora.lin);
    jan_remove_linhas(jan, ini, fim);
    // põe o cursor na primeira linha após as removidas
    jan->cursor_txt.lin = ini;
  } else if (modo == selecao_caractere) {
    // aqui é mais complicado um pouco
    // se a seleção tá toda em uma linha, tem que remover essa parte da linha
    // senão, cola o início da primeira linha no final da última e remove as
    //   linhas do meio
    posicao_t pos_ini = pos_antes(jan->cursor_txt, jan->ancora);
    posicao_t pos_fim = pos_depois(jan->cursor_txt, jan->ancora);
    // põe o cursor no primeiro caractere após o trecho removido
    jan->cursor_txt = pos_ini;
    // pega a primeira linha
    Lstr linhas = jan->txt->linhas;
    ls_posiciona(linhas, pos_ini.lin);
    str *plinha = ls_item_ptr(linhas);
    if (pos_ini.lin == pos_fim.lin) {
      // está tudo em uma linha só
      s_subst(plinha, pos_ini.col, pos_fim.col - pos_ini.col + 1, s_(""), s_(""));
      return;
    }
    // substitui o final da primeira linha pelo final da última
    ls_posiciona(linhas, pos_fim.lin);
    str ult_linha = ls_item(linhas);
    str final_ult = s_sub(ult_linha, pos_fim.col + 1, s_tam(ult_linha));
    s_subst(plinha, pos_ini.col, s_tam(*plinha), final_ult, s_(""));
    // remove as linhas intermediárias e a última
    jan_remove_linhas(jan, pos_ini.lin+1, pos_fim.lin);
  }
}

// cola o texto em sel no modo dado, antes da posição do cursor
void jan_cola_selecao_antes(janela_t *jan, Lstr sel, modo_t modo)
{
  if (modo == selecao_linha) {
    Lstr linhas = jan->txt->linhas;
    ls_posiciona(linhas, jan->cursor_txt.lin);
    for (ls_final(sel); ls_recua(sel); ) {
      ls_insere_antes(linhas, s_copia(ls_item(sel)));
    }
  } else if (modo == selecao_caractere) {
    Lstr linhas = jan->txt->linhas;
    ls_posiciona(linhas, jan->cursor_txt.lin);
    str *plinha = ls_item_ptr(linhas);
    ls_posiciona(sel, 0);
    str lin_sel = ls_item(sel);
    int tam_sel = ls_tam(sel);
    // se texto tá vazio, cola como linha
    if (!ls_item_valido(linhas)) return jan_cola_selecao_antes(jan, sel, selecao_linha);
    // se só tem uma linha, cola no meio da linha do cursor
    if (tam_sel == 1) {
      s_subst(plinha, jan->cursor_txt.col, 0, lin_sel, s_(""));
      return;
    }
    // tem mais de uma linha, cola a primeira a partir do cursor, as do meio inteiras,
    //   a última grudada com o resto da linha do cursor
    str resto = s_copia(s_sub(*plinha, jan->cursor_txt.col, s_tam(*plinha)));
    s_subst(plinha, jan->cursor_txt.col, s_tam(*plinha), lin_sel, s_(""));
    for (int i = 1; i < tam_sel - 1; i++) {
      ls_avanca(sel);
      ls_insere_depois(linhas, s_copia(ls_item(sel)));
    }
    ls_avanca(sel);
    lin_sel = ls_item(sel);
    s_subst(&resto, 0, 0, lin_sel, s_(""));
    ls_insere_depois(linhas, resto);
  }
}

// cola o texto em sel no modo dado, depois da posição do cursor
void jan_cola_selecao_depois(janela_t *jan, Lstr sel, modo_t modo)
{
  if (modo == selecao_linha) {
    Lstr linhas = jan->txt->linhas;
    ls_posiciona(linhas, jan->cursor_txt.lin);
    for (ls_inicio(sel); ls_avanca(sel); ) {
      ls_insere_depois(linhas, s_copia(ls_item(sel)));
    }
    jan->cursor_txt.lin++;
  } else if (modo == selecao_caractere) {
    jan_cursor_direita(jan);
    jan_cola_selecao_antes(jan, sel, modo);
  }
}

// o cursor pode estar fora do texto. coloca ele em uma posição válida.
// em alguns modos, o cursor pode ficar logo após o final da linha,
// por isso o bool
void jan_poe_cursor_no_texto(janela_t *jan, bool deixa_ficar_apos_final)
{
  Lstr linhas = jan->txt->linhas;

  if (jan->cursor_txt.lin < 0) jan->cursor_txt.lin = 0;
  if (jan->cursor_txt.col < 0) jan->cursor_txt.col = 0;

  if (jan->cursor_txt.lin > ls_tam(linhas) - 1) jan->cursor_txt.lin = ls_tam(linhas) - 1;
  ls_posiciona(linhas, jan->cursor_txt.lin);
  if (!ls_item_valido(linhas)) { // texto vazio
    jan->cursor_txt.col = 0;
    return;
  }
  str linha = ls_item(linhas);
  if (deixa_ficar_apos_final) {
    if (jan->cursor_txt.col > s_tam(linha)) jan->cursor_txt.col = s_tam(linha);
  } else {
    if (jan->cursor_txt.col > s_tam(linha) - 1) jan->cursor_txt.col = s_tam(linha) - 1;
    if (jan->cursor_txt.col < 0) jan->cursor_txt.col = 0;
  }
}

// o cursor pode ter saído da área visível da janela. desloca a janela para 
//   que o cursor fique visível
void jan_poe_janela_no_cursor(janela_t *jan)
{
  // se a janela inicia depois do cursor, faz ela iniciar no cursor
  if (jan->inicio_txt.lin > jan->cursor_txt.lin) {
    jan->inicio_txt.lin = jan->cursor_txt.lin;
  }
  if (jan->inicio_txt.col > jan->cursor_txt.col) {
    jan->inicio_txt.col = jan->cursor_txt.col;
  }

  // se a janela termina antes do cursor, faz ela terminar no cursor
  int ultima_linha = jan->inicio_txt.lin + (jan->tamanho.alt - 2);
  if (ultima_linha < jan->cursor_txt.lin) {
    ultima_linha = jan->cursor_txt.lin;
    jan->inicio_txt.lin = ultima_linha - (jan->tamanho.alt - 2);
  }
  int ultima_coluna = jan->inicio_txt.col + (jan->tamanho.larg - 7);
  if (ultima_coluna < jan->cursor_txt.col) {
    ultima_coluna = jan->cursor_txt.col;
    jan->inicio_txt.col = ultima_coluna - (jan->tamanho.larg - 7);
  }

  // não pode ser negativo
  if (jan->inicio_txt.lin < 0) jan->inicio_txt.lin = 0;
  if (jan->inicio_txt.col < 0) jan->inicio_txt.col = 0;
}

// editor_t {{{1

// estrutura que representa o estado do editor de textos
typedef struct {
  texto_t *txt;  // poderia ter vários
  janela_t *jan; // poderia ter vários
  modo_t modo;   // modo atual do editor
  Lstr selecao;  // texto copiado da seleção
  modo_t modo_selecao; // modo como a seleção foi copiada
  bool termina;  // true se deve encerrar o programa
} editor_t;

editor_t *ed_cria()
{
  editor_t *ed = malloc(sizeof(*ed));
  assert(ed != NULL);
  ed->txt = texto_cria(s_("exemplo.txt"));
  ed->jan = jan_cria(ed->txt);
  ed->modo = normal;
  ed->termina = false;
  ed->selecao = NULL;
  return ed;
}

void ed_destroi(editor_t *ed)
{
  jan_destroi(ed->jan);
  texto_destroi(ed->txt);
  if(ed->selecao != NULL) ls_destroi(ed->selecao);
  free(ed);
}

// retorna a janela que está selecionada para edição
janela_t *ed_janela_corrente(editor_t *ed)
{
  return ed->jan;
}

// troca o modo de edição para o modo dado
void ed_troca_modo(editor_t *ed, modo_t modo)
{
  ed->modo = modo;
}

bool ed_processa_tecla_global(editor_t *ed, tecla tec)
{
  return false;
}

bool ed_processa_setas(editor_t *ed, tecla tec)
{
  janela_t *jan = ed_janela_corrente(ed);
  bool processou = true;
  switch ((int)tec) {
    case t_right: jan_cursor_direita(jan); break;
    case t_left: jan_cursor_esquerda(jan); break;
    case t_up: jan_cursor_cima(jan); break;
    case t_down: jan_cursor_baixo(jan); break;
    case t_pgup: jan_cursor_pg_cima(jan); break;
    case t_pgdn: jan_cursor_pg_baixo(jan); break;
    case t_end: jan_cursor_final_linha(jan); break;
    case t_home: jan_cursor_inicio_linha(jan); break;
    default: processou = false;
  }
  return processou;
}

bool ed_processa_movimentos(editor_t *ed, tecla tec)
{
  if (ed_processa_setas(ed, tec)) return true;
  janela_t *jan = ed_janela_corrente(ed);
  bool processou = true;
  switch ((int)tec) {
    case 'h': jan_cursor_esquerda(jan); break;
    case 'j': jan_cursor_baixo(jan); break;
    case 'k': jan_cursor_cima(jan); break;
    case 'l': jan_cursor_direita(jan); break;
    case ' ': jan_cursor_direita(jan); break;
    case '0': jan_cursor_inicio_linha(jan); break;
    case '^': jan_cursor_inicio_linha(jan); break;
    case '$': jan_cursor_final_linha(jan); break;
    case 'g': jan_cursor_inicio_texto(jan); break; // no vi, é 'gg'
    case 'G': jan_cursor_final_texto(jan); break;
    // sem distinção entre palavra minúscula e maiúscula
    case 'w': jan_cursor_inicio_palavra_direita(jan); break;
    case 'W': jan_cursor_inicio_palavra_direita(jan); break;
    case 'b': jan_cursor_inicio_palavra_esquerda(jan); break;
    case 'B': jan_cursor_inicio_palavra_esquerda(jan); break;
    case 'e': jan_cursor_final_palavra_direita(jan); break;
    case 'E': jan_cursor_final_palavra_direita(jan); break;
    default: processou = false;
  }
  return processou;
}

// está em modo normal e recebeu a tecla tec
// faz o que tem que ser feito nesse caso
void ed_processa_tecla_normal(editor_t *ed, tecla tec)
{
  if (ed_processa_movimentos(ed, tec)) return;
  janela_t *jan = ed_janela_corrente(ed);
  switch ((int)tec) {
    case 'q': ed->termina = true; break;
    case 'i': ed_troca_modo(ed, insercao); break;
    case 'v': jan_define_ancora(jan); ed_troca_modo(ed, selecao_caractere); break;
    case 'V': jan_define_ancora(jan); ed_troca_modo(ed, selecao_linha); break;
    case 'r': ed_troca_modo(ed, troca1); break;
    case 'R': ed_troca_modo(ed, troca); break;
    case 'a': jan_cursor_direita(jan); ed_troca_modo(ed, insercao); break;
    case 'A': jan_cursor_final_linha(jan); ed_troca_modo(ed, insercao); break;
    case 'I': jan_cursor_inicio_linha(jan); ed_troca_modo(ed, insercao); break;
    case 'o': jan_abre_linha_abaixo(jan); jan_cursor_baixo(jan); ed_troca_modo(ed, insercao); break;
    case 'O': jan_abre_linha_acima(jan); jan_cursor_cima(jan); ed_troca_modo(ed, insercao); break;
    case 'J': jan_cursor_final_linha(jan); jan_junta_linhas(jan); break;
    case 'x': jan_remove_char(jan); break;
    case t_del: jan_remove_char(jan); break;
    case 'p': jan_cola_selecao_depois(jan, ed->selecao, ed->modo_selecao); break;
    case 'P': jan_cola_selecao_antes(jan, ed->selecao, ed->modo_selecao); break;
    default: ; // ignora teclas não tratadas
  }
}

// está em modo inserção e recebeu a tecla tec
// faz o que tem que ser feito nesse caso
void ed_processa_tecla_insercao(editor_t *ed, tecla tec)
{
  if (ed_processa_setas(ed, tec)) return;
  janela_t *jan = ed_janela_corrente(ed);
  switch ((int)tec) {
    case t_esc: ed_troca_modo(ed, normal); break;
    case t_enter: jan_quebra_linha(jan); jan_cursor_baixo(jan); jan_cursor_inicio_linha(jan); break;
    case t_back: jan_remove_char_esquerda(jan); break;
    case t_del: jan_remove_char(jan); break;
    default:
      if (u8_unichar_valido(tec) && tec >= ' ') {
        jan_insere_char(jan, tec);
        jan_cursor_direita(jan);
      } else ; // ignora teclas não tratadas
  }
}

// está em modo troca1 e recebeu a tecla tec
// faz o que tem que ser feito nesse caso
void ed_processa_tecla_troca1(editor_t *ed, tecla tec)
{
  janela_t *jan = ed_janela_corrente(ed);
  if (u8_unichar_valido(tec) && tec >= ' ') {
    jan_altera_char(jan, tec);
  } else ; // ignora teclas não tratadas
  ed_troca_modo(ed, normal);
}

// está em modo troca e recebeu a tecla tec
// faz o que tem que ser feito nesse caso
void ed_processa_tecla_troca(editor_t *ed, tecla tec)
{
  janela_t *jan = ed_janela_corrente(ed);
  if (tec == t_esc) {
    ed_troca_modo(ed, normal);
    return;
  }
  if (ed_processa_setas(ed, tec)) return;
  if (u8_unichar_valido(tec) && tec >= ' ') {
    jan_altera_char(jan, tec);
    jan_cursor_direita(jan);
  } else ; // ignora teclas não tratadas
}

// copia o texto selecionado para a área de cópia (de onde pode ser colado mais tarde)
void ed_copia_selecao(editor_t *ed)
{
  if (ed->selecao != NULL) ls_destroi(ed->selecao);
  janela_t *jan = ed_janela_corrente(ed);
  ed->modo_selecao = ed->modo;
  ed->selecao = jan_copia_selecao(jan, ed->modo_selecao);
  faz_lista_ter_copias_das_strings(ed->selecao);
}

// está em modo seleção e recebeu a tecla tec
// faz o que tem que ser feito nesse caso
void ed_processa_tecla_selecao(editor_t *ed, tecla tec)
{
  if (ed_processa_movimentos(ed, tec)) return;
  janela_t *jan = ed_janela_corrente(ed);
  switch ((int)tec) {
    case 'v': ed_troca_modo(ed, ed->modo == selecao_caractere ? normal : selecao_caractere); break;
    case 'V': ed_troca_modo(ed, ed->modo == selecao_linha ? normal : selecao_linha); break;
    case 'o': jan_troca_ancora(jan); break;
    case 'y': ed_copia_selecao(ed); break;
    case 'c': ed_copia_selecao(ed); jan_remove_selecao(jan, ed->modo); ed_troca_modo(ed, insercao); break;
    case 'd': ed_copia_selecao(ed); jan_remove_selecao(jan, ed->modo); ed_troca_modo(ed, normal); break;
    case 'x': ed_copia_selecao(ed); jan_remove_selecao(jan, ed->modo); ed_troca_modo(ed, normal); break;
    case 'p': jan_remove_selecao(jan, ed->modo); jan_cola_selecao_antes(jan, ed->selecao, ed->modo_selecao); ed_troca_modo(ed, normal); break;
    case 'P': jan_remove_selecao(jan, ed->modo); jan_cola_selecao_antes(jan, ed->selecao, ed->modo_selecao); ed_troca_modo(ed, normal); break;
    case t_esc: ed_troca_modo(ed, normal); break;
    default: ; // ignora teclas não tratadas
  }
}

// lê a próxima tecla e faz o que tem que ser feito com ela
void ed_processa_tecla(editor_t *ed)
{
  janela_t *jan = ed_janela_corrente(ed);
  tecla tec = tela_le_tecla();
  if (ed_processa_tecla_global(ed, tec)) return;
  switch (ed->modo) {
    case normal: ed_processa_tecla_normal(ed, tec); break;
    case insercao: ed_processa_tecla_insercao(ed, tec); break;
    case selecao_caractere: ed_processa_tecla_selecao(ed, tec); break;
    case selecao_linha: ed_processa_tecla_selecao(ed, tec); break;
    case troca: ed_processa_tecla_troca(ed, tec); break;
    case troca1: ed_processa_tecla_troca1(ed, tec); break;
  }
  jan_poe_cursor_no_texto(jan, ed->modo == insercao || ed->modo == troca);
  jan_poe_janela_no_cursor(jan);
}

// desenha toda a tela e coloca o cursor na posição corrente
void ed_desenha_tela(editor_t *ed)
{
  //tela_limpa();
  tela_seleciona_cursor(invisivel);
  janela_t *jan = ed_janela_corrente(ed);
  jan_desenha(jan, ed->modo);
  if (ed->modo == insercao)
    tela_seleciona_cursor(barra);
  else if (ed->modo == troca || ed->modo == troca1)
    tela_seleciona_cursor(sublinha);
  else
    tela_seleciona_cursor(bloco);
}

int main()
{
  tela_cria();
  editor_t *ed = ed_cria();

  while (!ed->termina) {
    ed_processa_tecla(ed);
    ed_desenha_tela(ed);
  }

  ed_destroi(ed);
  tela_destroi();
}
// vim: foldmethod=marker shiftwidth=2
