#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

#define STR_VAZIA (str){0,0,0,NULL}

// funções auxiliares para codificação UTF8 {{{1

// codificação UTF8
// Um codepoint unicode tem um valor entre 0 e x10FFFF.
// UTF8 codifica um codepoint em 1 a 4 bytes, dependendo do valor do codepoint.
// O primeiro byte inicia com o primeiro bit (o mais significativo) 0 ou com os
// dois primeiros bits 11.
// Os demais bytes (se houver) iniciam com os 2 primeiros bits 10.
// Analisando esses bits, é fácil identificar um byte inicial ou de continuação.
// Cada byte UTF8 contém alguns dos bits que compõem o valor do codepoint,
// representados por x na tabela abaixo
//
// hexa do codepoint    1º byte   2º byte   3º byte   4º byte
// 000000 a 00007F     0xxxxxxx
// 000080 a 0007FF     110xxxxx  10xxxxxx
// 000800 a 00FFFF     1110xxxx  10xxxxxx  10xxxxxx
// 010000 a 10FFFF     11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
//
// Os codepoints entre 0xD800 e 0xDFFF são usados como auxiliares na codificação
// UTF16 e não podem ser usados para representar caracteres.
// Quando um codepoint pode ser representado por mais de um tamanho de sequência
// (o código 101 poderia ser representado por 00000101 ou 11000000 10000101 por
// exemplo), só a representação mais curta é considerada válida.


// retorna o número de bytes usados para codificar um caractere em unicode
//   quando o primeiro byte é codificado é b1
static int u8_bytes_no_unichar_que_comeca_com(byte b1)
{
  if (b1 <= 0b01111111) return 1;
  if (b1 >= 0b11000000 && b1 <= 0b11011111) return 2;
  if (b1 >= 0b11100000 && b1 <= 0b11101111) return 3;
  if (b1 >= 0b11110000 && b1 <= 0b11110111) return 4;
  return -1; // ou aborta?
}

// b é um byte de continuação de uma codificação utf8?
static bool u8_byte_de_continuacao(byte b)
{
  return (b & 0b11000000) == 0b10000000;
}

// adiciona os 6 bits do byte do continuação cont em *uni
// retorna false se cont não for válido
static bool u8_soma_bits_de_continuacao(unichar *uni, byte cont)
{
  if (!u8_byte_de_continuacao(cont)) return false;
  byte bits = cont & 0b00111111;
  *uni = ((*uni) << 6) | bits;
  return true;
}

// calcula o codepoint do caractere que inicia no byte apontado por s
// que tem no máximo maxn bytes
// retorna o número de bytes no caractere ou -1 se erro (código inválido,
//   codificação inválida ou precisa mais que maxn bytes)
// se puni não for NULL e o retorno não for -1, coloca o caractere em *puni
static int u8_unichar_nos_bytes(byte *s, int maxn, unichar *puni)
{
  byte primeiro_byte = s[0];
  int nbytes = u8_bytes_no_unichar_que_comeca_com(primeiro_byte);
  if (nbytes < 1 || nbytes > maxn || nbytes > 4) return -1;

  // monta o código uni à partir dos bytes utf8
  //   máscara binária com os bits válidos para o 1o byte de cada tamanho
  byte mascara[] = { 0b01111111, 0b00011111, 0b00001111, 0b00000111, };
  unichar uni = primeiro_byte & mascara[nbytes - 1];
  int nbytes_adicionados = 1;
  while (nbytes > nbytes_adicionados) {
    if (!u8_soma_bits_de_continuacao(&uni, s[nbytes_adicionados])) return -1;
    nbytes_adicionados++;
  }

  // verifica se o código é válido
  if (nbytes > 1 && uni < 0x80) return -1;
  if (nbytes > 2 && uni < 0x800) return -1;
  if (nbytes > 3 && uni < 0x10000) return -1;
  if (uni > 0x10FFFF) return -1;
  if (uni >= 0xD800 && uni <= 0xDFFF) return -1;

  if (puni != NULL) *puni = uni;
  return nbytes;
}

// conta quantos caracteres unicode estão codificados em utf8 nos
//   nbytes que iniciam em *ptr
// faz a verificação da codificação utf8
// retorna o número de caracteres ou -1 se houver erro na codificação
static int u8_conta_unichar_nos_bytes(byte *ptr, int nbytes)
{
  int num_uni = 0;
  while (nbytes > 0) {
    int nb1 = u8_unichar_nos_bytes(ptr, nbytes, NULL);
    if (nb1 < 1) return -1;
    num_uni++;
    ptr += nb1;
    nbytes -= nb1;
  }
  return num_uni;
}

// retorna o ponteiro para o primeiro byte do caractere que está
//   n caracteres adiante do caractere cujo 1o byte está em *ptr
// quem chama garante que existem pelo menos n caracteres
// retorna NULL em caso de erro na codificação (que não deveria
//   acontecer, esta função é para ser usada só em região contendo
//   utf8 válido)
// não tem suporte a n negativo
static byte *u8_avanca_unichar(byte *ptr, int n)
{
  byte *p = ptr;
  for (int i = 0; i < n; i++) {
    int nb1 = u8_bytes_no_unichar_que_comeca_com(*p);
    if (nb1 < 1) return NULL;
    p += nb1;
  }
  return p;
}

// conta o número de caracteres unicode codificados em UTF8 em uma string C
// função pública, só para a macro s_()
// não faz uma análise mais completa da validade como u8_conta_unichar_nos_bytes
int u8_nchars_na_strC(char *s)
{
  int l = 0;
  while (*s != '\0') {
    if (!u8_byte_de_continuacao((byte)*s)) l++;
    s++;
  }
  return l;
}

// funções auxiliares {{{1

// testa se um número é potência de 2
static bool pot2(int n)
{
    return n == (n & -n);
}

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(str cad)
{
  assert(cad.tamb >= cad.tamc);
  if (cad.tamc > 0) {
    assert(cad.mem != NULL);
  }
  if (cad.cap > 0) {
    assert(cad.mem != NULL);
    assert(cad.cap > cad.tamb);
    assert(cad.mem[cad.tamb] == '\0');
    assert(cad.cap >= MIN_ALLOC);
    assert(pot2(cad.cap));
  }
}

// verifica se a string apontada por pcad é alterável
static bool s_alteravel(str *pcad)
{
  return pcad->cap > 0;
}

// operações de criação e destruição {{{1

str s_cria(char *strC)
{
  str cad;
  cad.mem = (byte *)strC;
  cad.cap = 0;
  cad.tamb = strlen(strC);
  int tamc = u8_conta_unichar_nos_bytes(cad.mem, cad.tamb);
  // se tem erro no utf8, desiste
  if (tamc == -1) return STR_VAZIA;
  cad.tamc = tamc;
  return cad;
}

void s_destroi(str cad)
{
  s_ok(cad);
  if (cad.cap > 0) free(cad.mem);
}


// operações de acesso {{{1

int s_tam(str cad)
{
  s_ok(cad);
  return cad.tamc;
}

char *s_mem(str cad)
{
  s_ok(cad);
  return (char *)cad.mem;
}

// retorna o endereço do byte onde inicia o caractere na posição pos de cad
// versão sem medo -- pos está dentro dos limites válidos para cad
static byte *s_ender_pos_sm(str cad, int pos)
{
  if (pos == cad.tamc) return cad.mem + cad.tamb;
  return u8_avanca_unichar(cad.mem, pos);
}

unichar s_ch(str cad, int pos)
{
  s_ok(cad);
  // o ajuste de pos é diferente nesta função, porque tem que retornar UNI_INV
  if (pos < 0) pos += cad.tamc;
  if (pos < 0 || pos >= cad.tamc) return UNI_INV;
  // acha e converte o caractere
  byte *ptr = s_ender_pos_sm(cad, pos);
  int nbytes_pulados = ptr - cad.mem;
  unichar uni = UNI_INV;
  u8_unichar_nos_bytes(ptr, cad.tamb - nbytes_pulados, &uni);
  return uni;
}

// versão sem medo de sub (pos e tam estão dentro dos limites de cad)
static str s_sub_sm(str cad, int pos, int tam)
{
  byte *end_ini = s_ender_pos_sm(cad, pos);
  byte *end_fim = u8_avanca_unichar(end_ini, tam);
  return (str){ .tamc = tam, .tamb = end_fim - end_ini, .cap = 0, .mem = end_ini };
}

// altera a posição em *ppos para que dentro dos limites de lim caracteres
static void s_ajeita_pos(int *ppos, int lim)
{
  if (*ppos < 0) *ppos += lim;
  if (*ppos < 0) *ppos = 0;
  if (*ppos > lim) *ppos = lim;
}

// altera a posição e o tamanho de um trecho para que esteja dentro dos limites
//   de lim caracteres
static void s_ajeita_pos_tam(int *ppos, int *ptam, int lim)
{
  // se pos é negativo, conta do fim
  if (*ppos < 0) *ppos += lim;
  // se continua negativo, tá antes do início
  if (*ppos < 0) {
    *ptam += *ppos;
    *ppos = 0;
  }
  // se tá além do limite, limita
  if (*ppos >= lim) {
    *ppos = lim;
    *ptam = 0;

  }
  // se termina após o limite, limita
  if (*ppos + *ptam > lim) {
    *ptam = lim - *ppos;
  }
}

str s_sub(str cad, int pos, int tam)
{
  s_ok(cad);
  s_ajeita_pos_tam(&pos, &tam, cad.tamc);
  if (tam == 0) return STR_VAZIA;
  return s_sub_sm(cad, pos, tam);
}

str s_apara(str cad, str sobras)
{
  // implementa este, pliz?
  return cad;
}

// operações de busca {{{1

int s_busca_c(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  s_ajeita_pos(&pos, cad.tamc);
  // extrai uma substring com cada caractere e procura em chs
  for (int i = pos; i < cad.tamc; i++) {
    str ch = s_sub_sm(cad, i, 1);
    if (s_busca_s(chs, 0, ch) != -1) return i;
  }
  return -1;
}

int s_busca_nc(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  s_ajeita_pos(&pos, cad.tamc);
  // extrai uma substring com cada caractere e procura em chs
  for (int i = pos; i < cad.tamc; i++) {
    str ch = s_sub_sm(cad, i, 1);
    if (s_busca_s(chs, 0, ch) == -1) return i;
  }
  return -1;
}

int s_busca_rc(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  s_ajeita_pos(&pos, cad.tamc);
  // extrai uma substring com cada caractere e procura em chs
  for (int i = pos; i >= 0; i--) {
    str ch = s_sub_sm(cad, i, 1);
    if (s_busca_s(chs, 0, ch) != -1) return i;
  }
  return -1;
}

int s_busca_rnc(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  s_ajeita_pos(&pos, cad.tamc);
  // extrai uma substring com cada caractere e procura em chs
  for (int i = pos; i >= 0; i--) {
    str ch = s_sub_sm(cad, i, 1);
    if (s_busca_s(chs, 0, ch) == -1) return i;
  }
  return -1;
}

// retorna true se tam bytes em b1 são iguais aos em b2
// poderia usar memcmp para isso
static bool bytes_iguais(int tam, byte b1[tam], byte b2[tam])
{
  for (int i = 0; i < tam; i++) {
    if (b1[i] != b2[i]) return false;
  }
  return true;
}

// procura agulha em palheiro, retorna a posição em palheiro onde está agulha, ou NULL
static byte *busca_bytes(int tam_agulha, byte agulha[tam_agulha],
                         int tam_palheiro, byte palheiro[tam_palheiro])
{
  for (int i = 0; i <= tam_palheiro - tam_agulha; i++) {
    if (bytes_iguais(tam_agulha, palheiro + i, agulha)) return palheiro + i;
  }
  return NULL;
}

int s_busca_s(str cad, int pos, str buscada)
{
  s_ok(cad);
  s_ok(buscada);
  s_ajeita_pos(&pos, cad.tamc);
  if (pos >= cad.tamc) return -1;
  byte *end_ini = s_ender_pos_sm(cad, pos);
  byte *end_fim = cad.mem + cad.tamb;
  // usa memmem para procurar o byte onde inicia a string buscada
  // memmem não é padrão :(
  // byte *end_achou = memmem(end_ini, end_fim - end_ini, buscada.mem, buscada.tamb);
  byte *end_achou = busca_bytes(buscada.tamb, buscada.mem, end_fim - end_ini, end_ini);
  if (end_achou == NULL) return -1;
  // transforma os bytes contados em caracteres
  return pos + u8_conta_unichar_nos_bytes(end_ini, end_achou - end_ini);
}


// operações de alteração {{{1

// realoca a memória de *pcad (que é pcad->mem, com tamanho pcad->cap), se necessário,
//   para que possa conter uma string com "precisa" bytes
// segue a regra de alocação definida, pelo menos um byte a mais, pelo menos MIN_ALLOC
//   bytes, tamanho é potência de 2
// a string é considerada alterável, mesmo que cap seja 0 (para ser usado para alocação
//   inicial, com cap==0 e mem==NULL; realloc é igual malloc quando recebe NULL)
static void s_realoca(str *pcad, unsigned int precisa)
{
  int nbytes = pcad->cap;
  if (nbytes < MIN_ALLOC) nbytes = MIN_ALLOC;
  while (nbytes <= precisa) nbytes *= 2;
  while (nbytes > MIN_ALLOC && nbytes > 3 * precisa) nbytes /= 2;
  if (nbytes != pcad->cap) {
    pcad->mem = realloc(pcad->mem, nbytes);
    assert(pcad->mem != NULL);
    pcad->cap = nbytes;
  }
}

str s_copia(str cad)
{
  s_ok(cad);
  str nova;
  nova.tamc = cad.tamc;
  nova.tamb = cad.tamb;
  // força a alocação de acordo com as regras
  nova.cap = 0;
  nova.mem = NULL;
  s_realoca(&nova, nova.tamb);
  // copia para o novo lar, sem esquecer do \0
  memcpy(nova.mem, cad.mem, nova.tamb);
  nova.mem[nova.tamb] = '\0';
  s_ok(nova);
  return nova;
}

void s_cat(str *pcad, str cadb)
{
  // insere no fim
  s_insere(pcad, pcad->tamc, cadb);
}

void s_insere(str *pcad, int pos, str cadb)
{
  s_ok(*pcad);
  s_ok(cadb);
  if (!s_alteravel(pcad)) return;
  // garante que tem memória e se livra de posição incômoda
  s_realoca(pcad, pcad->tamb + cadb.tamb);
  s_ajeita_pos(&pos, pcad->tamc);
  // move o final da string para dar espaço para cadb
  byte *end_ini = s_ender_pos_sm(*pcad, pos);
  memmove(end_ini + cadb.tamb, end_ini, pcad->mem + pcad->tamb - end_ini);
  // copia cadb para o espaço gerado
  memcpy(end_ini, cadb.mem, cadb.tamb);
  // ajusta os tamanhos
  pcad->tamb += cadb.tamb;
  pcad->tamc += cadb.tamc;
  // finaliza a string -- poderia ter copiado um byte a mais no memmove
  pcad->mem[pcad->tamb] = '\0';
}

void s_remove(str *pcad, int pos, int tam)
{
  s_ok(*pcad);
  if (!s_alteravel(pcad)) return;
  // se livra de posição incômoda
  s_ajeita_pos_tam(&pos, &tam, pcad->tamc);
  // calcula os endereços do primeiro byte a remover e do primeiro que fica
  byte *end_ini = s_ender_pos_sm(*pcad, pos);
  byte *end_fim = u8_avanca_unichar(end_ini, tam);
  // copia o final sobre o que é removido (não esquece de copiar o \0)
  int nbytes_remocao = end_fim - end_ini;
  memmove(end_ini, end_fim, pcad->mem + pcad->tamb - end_fim + 1);
  // ajusta os tamanhos
  pcad->tamb -= nbytes_remocao;
  pcad->tamc -= tam;
  // garante que o quantidade alocada segue as regras
  s_realoca(pcad, pcad->tamb);
}

void s_preenche(str *pcad, int tam, str enchimento)
{
  s_ok(*pcad);
  s_ok(enchimento);
  if (!s_alteravel(pcad)) return;
  int nchar_adicao = tam - pcad->tamc;
  // implementação não muito otimizada...
  while (nchar_adicao > 0) {
    str adicao = s_sub(enchimento, 0, nchar_adicao);
    s_cat(pcad, adicao);
    nchar_adicao -= s_tam(adicao);
  }
}

// funções auxiliares para implementar os vários casos de s_subst

// substituição antes do início da string, com enchimento, sem medo
static void s_subst_antes_com_enchimento_sm(str *pcad, int pos, int tam, str cadb, str enchimento)
{
  s_insere(pcad, 0, cadb);
  int pos_adicao = s_tam(cadb);
  // o enchimento deveria começar em pos, com tam caracteres sendo substituídos;
  //   calcula qual seria o primeiro caractere do enchimento depois disso
  int pos_enchimento = tam % s_tam(enchimento);
  int n_enchimento = -(pos + tam);
  while (n_enchimento > 0) {
    str adicao = s_sub(enchimento, pos_enchimento, n_enchimento);
    s_insere(pcad, pos_adicao, adicao);
    n_enchimento -= s_tam(adicao);
    pos_adicao += s_tam(adicao);
    // agora enche desde o primeiro caractere do enchimento
    pos_enchimento = 0;
  }
}

// substituição no final da string, com enchimento, sem medo
static void s_subst_depois_com_enchimento_sm(str *pcad, int pos, str cadb, str enchimento)
{
  s_preenche(pcad, pos, enchimento);
  s_cat(pcad, cadb);
}

// substitui o que tiver em pcad a partir de pos por cadb, sem medo
static void s_subst_final_sm(str *pcad, int pos, str cadb)
{
  s_remove(pcad, pos, s_tam(*pcad) - pos); // poderia ter s_trunca...
  s_cat(pcad, cadb);
}

// substitui tam caracteres em pcad a partir de pos por cadb, sem medo
static void s_subst_sm(str *pcad, int pos, int tam, str cadb)
{
  s_remove(pcad, pos, tam);
  s_insere(pcad, pos, cadb);
}

void s_subst(str *pcad, int pos, int tam, str cadb, str enchimento)
{
  s_ok(*pcad);
  s_ok(cadb);
  s_ok(enchimento);
  if (!s_alteravel(pcad)) return;
  if (pos < 0) pos += s_tam(*pcad);
  if (tam < 0) tam = 0; // sem suporte a tamanhos negativos

  if (pos + tam < 0) { // precisa de enchimento no início
    s_subst_antes_com_enchimento_sm(pcad, pos, tam, cadb, enchimento);
    return;
  }

  // se começa fora e termina dentro, recalcula o tamanho a eliminar
  if (pos < 0) {
    tam += pos;
    pos = 0;
  }

  if (pos > s_tam(*pcad)) { // precisa de enchimento no final
    s_subst_depois_com_enchimento_sm(pcad, pos, cadb, enchimento);
  } else if (pos + tam >= s_tam(*pcad)) {
    s_subst_final_sm(pcad, pos, cadb);
  } else {
    // tá tudo dentro da string
    s_subst_sm(pcad, pos, tam, cadb);
  }
}


// operações de acesso a arquivo {{{1

str s_le_arquivo(str nome)
{
  s_ok(nome);

  // abre o arquivo
  FILE *arq;
  char *nomec = s_strc(nome);
  arq = fopen(nomec, "r");
  free(nomec);
  if (arq == NULL) return STR_VAZIA;

  // descobre o tamanho do arquivo
  long tam_arq;
  assert(fseek(arq, 0, SEEK_END) == 0);
  tam_arq = ftell(arq);
  assert(tam_arq >= 0);
  rewind(arq);
  
  // aloca espaco e preenche com o conteúdo do arquivo
  str nova;
  nova.mem = NULL;
  nova.cap = 0;
  s_realoca(&nova, tam_arq);
  int bytes_lidos = fread(nova.mem, 1, tam_arq, arq);
  // bytes_lidos pode ser diferente de tam_arq. em windows, a representação
  //   de um final de linha tem tamanho diferente no arquivo e em memória.
  assert(bytes_lidos >= 0);
  fclose(arq);

  // ajusta outros campos da string
  nova.tamb = bytes_lidos;
  nova.mem[nova.tamb] = '\0';
  nova.tamc = u8_conta_unichar_nos_bytes(nova.mem, nova.tamb);

  return nova;
}

void s_grava_arquivo(str cad, str nome)
{
  s_ok(cad);
  s_ok(nome);

  // abre o arquivo
  FILE *arq;
  char *nomec = s_strc(nome);
  arq = fopen(nomec, "w");
  free(nomec);
  if (arq == NULL) return;

  // escreve o conteúdo da string
  fwrite(cad.mem, 1, cad.tamb, arq); // ignora erros
  fclose(arq);
}


// outras operações {{{1

bool s_igual(str cad, str cadb)
{
  s_ok(cad);
  s_ok(cadb);
  if (cad.tamb != cadb.tamb) return false;
  return memcmp(cad.mem, cadb.mem, cad.tamb) == 0;
}

void s_imprime(str cad)
{
  s_ok(cad);
  fwrite(cad.mem, 1, cad.tamb, stdout);
}

char *s_strc(str cad)
{
  char *strc = malloc(cad.tamb + 1);
  if (strc == NULL) return NULL;
  memcpy(strc, cad.mem, cad.tamb);
  strc[cad.tamb] = '\0';
  return strc;
}

Lstr s_separa(str cad, str separadores)
{
  Lstr lista = ls_cria();
  int ini = 0, fim;
  while (ini < s_tam(cad)) {
    // busca a posição do primeiro separador após ini
    fim = s_busca_c(cad, ini, separadores);
    if (fim == -1) fim = s_tam(cad);
    ls_insere_depois(lista, s_sub(cad, ini, fim - ini));
    // pula o separador
    ini = fim + 1;
  }
  return lista;
}

// vim: foldmethod=marker shiftwidth=2
