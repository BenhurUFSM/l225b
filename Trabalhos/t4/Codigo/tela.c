#include "tela.h"
#include "utf8.h"

// implementado usando
//   - sequências de escape ANSI para controlar a saída (cursor, cores)
//   - termios para alterar o modo de entrada do terminal
//   - ioctl para descobrir o tamanho do terminal
//   - signal para ser sinalizado quando o terminal mudar de tamanho


#include <stdio.h>
#include <stdbool.h>

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>

// muda o processamento de caracteres de entrada pelo terminal
// se 'cru' for true, coloca em leitura de caracteres individuais;
// se for false, devolve o estado que estava quando foi chamado com true.
// deve ser chamado com true antes de ser chamado com false
static void tela_altera_modo_entrada(bool cru)
{
  static struct termios estado_original;
  if (cru) {
    // lê e guarda o estado atual do terminal
		tcgetattr(1, &estado_original);
    // altera o estado do terminal para não ecoar o que é digitado
    //   nem esperar <enter>
		struct termios t = estado_original;
		t.c_lflag &= (~ECHO & ~ICANON);
    t.c_cc[VMIN] = 0; // pode retornar de uma leitura sem nenhum caractere
    t.c_cc[VTIME] = 0; //1; // espera até tantos décimos de segundo antes de retornar
		tcsetattr(1, TCSANOW, &t);
  } else {
    // recoloca o terminal no estado original
    tcsetattr(1, TCSANOW, &estado_original);
  }
}

static void tela_altera_modo_saida(void)
{
  // faz com que os caracteres impressos sejam enviados diretamente
  // para a tela, sem serem mantidos em um buffer em memória
  //setvbuf(stdout, NULL, _IONBF, 0);
  // faz com que os caracteres impressos sejam enviados para um buffer
  // em memória, e só sejam enviados à tela quando o buffer encher ou
  // for chamado fflush (que é chamado por tela_mostra)
  setvbuf(stdout, NULL, _IOFBF, 30000);
}

void tela_seleciona_cursor(tipo_cursor tipo)
{
  char c;
  switch (tipo) {
    case invisivel:
      printf("\033[?25l");
      return;
    case barra:
      c = '5';
      break;
    case sublinha:
      c = '3';
      break;
    default:
      c = '1';
      break;
  }
    printf("\033[%c q\033[?25h", c);
}

static void tela_seleciona_tela_alternativa(bool alt)
{
  if (alt) {
    printf("\033[?1049h");
  } else {
    printf("\033[?1049l");
  }
}

static void tela_le_nlincol(int);

static int fps;

void tela_cria(int fps_desejado)
{
  fps = fps_desejado;
  tela_altera_modo_entrada(true);
  tela_altera_modo_saida();
  tela_seleciona_tela_alternativa(true);
  // chama tela_le_nlincol se tela mudar de tamanho
  signal(SIGWINCH, tela_le_nlincol);
  tela_le_nlincol(0);
  tela_limpa();
}

void tela_destroi(void)
{
  tela_limpa();
  tela_altera_modo_entrada(false);
  tela_seleciona_tela_alternativa(false);
  tela_seleciona_cursor(bloco);
}

void tela_mostra()
{
  static double ult = 0;
  double dif = tela_relogio() - ult;
  int espera = 1000000 / fps - (int)(dif * 1000000);
  if (ult != 0 && espera > 0) usleep(espera);
  ult = tela_relogio();
  fflush(stdout);
}

void tela_limpa(void)
{
  printf("\033[2J");
}

void tela_limpa_fim_da_linha(void)
{
  printf("\033[K");
}

void tela_lincol(int lin, int col)
{
  printf("\033[%d;%dH", lin, col);
}

static int nlin, ncol; 
static bool tela_foi_redimensionada = false;

static void tela_le_nlincol(int nada)
{
  struct winsize tam;
  ioctl(1, TIOCGWINSZ, &tam);
  nlin = tam.ws_row;
  ncol = tam.ws_col;
  tela_limpa();
  tela_foi_redimensionada = true;
}

int tela_nlin(void)
{
  return nlin;
}

int tela_ncol(void)
{
  return ncol;
}

void tela_cor_normal(void)
{
  printf("\033[m");
}

void tela_cor_letra(int vermelho, int verde, int azul)
{
  printf("\033[38;2;%d;%d;%dm", vermelho, verde, azul);
}

void tela_cor_fundo(int vermelho, int verde, int azul)
{
  printf("\033[48;2;%d;%d;%dm", vermelho, verde, azul);
}

static bool le_1_byte(byte *pb);
static tecla trata_esc(void);
static tecla trata_utf8(byte primeiro_byte);

tecla tela_le_tecla(void)
{
  if (tela_foi_redimensionada) {
    tela_foi_redimensionada = false;
    return t_resize;
  }
  byte a;
  if (!le_1_byte(&a)) return t_none;
  switch (a) {
    case 127:    return t_back;
    case '\n':   return t_enter;
    case '\033': return trata_esc();
    default:     return trata_utf8(a);
  }
}

static bool le_1_byte(byte *pb)
{
  if (read(1, pb, 1) != 1) return false;
  return true;
}

static tecla trata_esc(void)
{
  // A codificação de caracteres de entrada obedece a "padrões" misturados
  //   que foram sendo adaptados conforme a necessidade e o estado mental
  //   dos "responsáveis"...
  // só um ESC quer dizer que a tecla ESC foi apertada
  // dois ESC quer dizer que foi apertado Alt-ESC (a não ser que tenha um
  //   tempinho entre eles, aí quer dizer que ESC foi apertado 2 vezes)
  // se depois do ESC tem um [, é o início de uma sequência, que pode ter
  //   dois formatos:
  // ESC [ (mod) letra  -- em que (mod) é um número opcional que codifica
  //   os modificadores (shift, control, alt, super) e letra é uma letra
  //   maiúscula que codifica a tecla
  // ESC [ número (;mod) ~  -- em que (;mod) é como acima (mas precedido de
  //   ponto e vírgula, se houver) e o número (em decimal, podendo ocupar
  //   um ou dois caracteres) codifica a tecla
  // a codificação dos modificadores é feita somando 1,2,4 e 8 conforme
  //   esteja apertado shift, control, alt e/ou super, respectivamente, e
  //   ao total somando 1. (1 é nenhum modificador, 2 é shift, 3 é control,
  //   4 é control+shift etc)
  // a letra do primeiro caso pode ser:
  //   A seta cima   B seta baixo   C seta direita   D seta esquerda
  //   F end         H home         P F1             Q F2
  //   R F3          S F4
  //   no caso dos F1-4 (os demais F não são codificados assim), a letra
  //   P-S é geralmente precedida do modificador, mesmo que não tenha (1
  //   nesse caso) --  F1 é geralmente codificado como ESC[1P
  // o número no segundo caso pode ser:
  //   1 a 8 - home ins del end pgup pgdn home end
  //   10 a 15 - F0 a F5
  //   17 a 21 - F6 a F10
  //   23 a 26 - F11 a F14
  // então, obviamente, quando se digita F4 o terminal envia ESC[1S e quando
  //   se digita F5, envia ESC[15~ com shift, seria F4=ESC[2S e F5=ESC[15;2~
  // me deu uma desvontade de fazer uma função para decodificar isso tudo,
  //   mas dou o maior apoio para quem quiser fazê-lo
  // o código abaixo implementa a decodificação de poucos casos, nenhum com
  //   modificador; pior: se o código não for decodificado, provavelmente
  //   restarão caracteres na entrada, que serão tratados como se o usuário
  //   tivesse digitado eles
  unsigned char c[4];
  int n = 0;
  if (read(1, &c[n++], 1) != 1) {
    return t_esc;
  }
  if (c[0] == '[') {
    if (read(1, &c[n++], 1) != 1) {
      return t_none;
    }
    switch(c[1]) {
      case 'A': return t_up;
      case 'B': return t_down;
      case 'C': return t_right;
      case 'D': return t_left;
      case 'F': return t_end;
      case 'H': return t_home;
    }
    if (c[1] >= '0' && c[1] <= '9') {
      if (read(1, &c[n++], 1) != 1) {
        return t_none;
      }
      if (c[2] == '~') {
        switch (c[1]) {
          case '1': return t_home;
          case '2': return t_insert;
          case '3': return t_del;
          case '4': return t_end;
          case '5': return t_pgup;
          case '6': return t_pgdn;
          case '7': return t_home;
          case '8': return t_end;
        }
      }
    }
  }
  return t_none;
}

static tecla trata_utf8(byte primeiro_byte)
{
  int nbytes = u8_bytes_no_unichar_que_comeca_com(primeiro_byte);
  unichar uni;
  byte buf[4];
  buf[0] = primeiro_byte;
  for (int i = 1; i < nbytes; i++) {
    if (!le_1_byte(&buf[i])) return t_none;
  }
  if (u8_unichar_nos_bytes(buf, nbytes, &uni) == -1) return t_none;
  return (tecla)uni;
}

double tela_relogio(void)
{
  struct timespec agora;
  clock_gettime(CLOCK_REALTIME, &agora);
  return agora.tv_sec + agora.tv_nsec*1e-9;
}
