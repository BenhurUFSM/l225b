#ifndef _UTF8_H_
#define _UTF8_H_

// funções auxiliares para codificação UTF8

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

// tipo para representar um byte
// em C23 esse tipo seria char8_t
typedef unsigned char byte;

// tipo para representar um codepoint (código de um caractere unicode)
typedef unsigned int unichar;
#define UNI_INV (unichar)-1   // representa um caractere inválido


// retorna true so o valor em uni representa um código unicode válido
bool u8_unichar_valido(unichar uni);

// retorna o número de bytes usados para codificar um caractere em unicode
//   quando o primeiro byte é codificado é b1
int u8_bytes_no_unichar_que_comeca_com(byte b1);

// calcula o codepoint do caractere que inicia no byte apontado por s
// que tem no máximo maxn bytes
// retorna o número de bytes no caractere ou -1 se erro (código inválido,
//   codificação inválida ou precisa mais que maxn bytes)
// se puni não for NULL e o retorno não for -1, coloca o caractere em *puni
int u8_unichar_nos_bytes(byte *s, int maxn, unichar *puni);

// conta quantos caracteres unicode estão codificados em utf8 nos
//   nbytes que iniciam em *ptr
// faz a verificação da codificação utf8
// retorna o número de caracteres ou -1 se houver erro na codificação
int u8_conta_unichar_nos_bytes(byte *ptr, int nbytes);

// retorna o ponteiro para o primeiro byte do caractere que está
//   n caracteres adiante do caractere cujo 1o byte está em *ptr
// quem chama garante que existem pelo menos n caracteres
// retorna NULL em caso de erro na codificação (que não deveria
//   acontecer, esta função é para ser usada só em região contendo
//   utf8 válido)
// não tem suporte a n negativo
byte *u8_avanca_unichar(byte *ptr, int n);

// coloca em buf a codificação utf8 do caractere uni, ou 0xFFFD se uni for inválido
// retorna o número de bytes usados
// buf tem que ter espaço suficiente (pode ser necessário colocar até 4 bytes)
int u8_converte_pra_utf8(unichar uni, byte *buf);

#endif // _UTF8_H_
// vim: foldmethod=marker shiftwidth=2
