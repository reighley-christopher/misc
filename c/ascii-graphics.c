#include <stdio.h>
#include <ncursesw/curses.h>
#include <locale.h>

/*this doesn't work very well if I haven't set up a terminal that supports utf-8 and 24 bit color*/

int main(int argc, char *argv[])
  {
  printf("\033[22;31m \xE2\x98\xA0\n"); 
  /*this is U+2620 (skull and crossbones) how did it become E2 98 A0 ? ie how to UTF-8 :
    2 byte code in unicode become 3 bytes in UTF-8
    the first nibble will be E because a 3 byte code always starts with the bits 1110 (a 4 byte codes starts with 11110 and a 2 byte code with 110)
    the first 4 bits are then the same as the code point (in this case 2)
    the next bits are always 10 (is this an endianness thing?)
    the 6 bits that follow are from the code point (so 0110 00 (10)) -> the next byte will be 1001 1000 = 0x98
    the next bits are always 10 and the last 6 bits from the code point (00) 10 0000 -> the next byte is 1010 0000 = 0xA0
    gaa
  */
//  printf("\033[38;5;237m \xE2\x98\xA0\n"); /*TODO my terminal has broad color support how do I check capabilities*/
//  printf("\033[48;5;250m \xE2\x98\xA0\n"); 
  /*U+2581 - U+2588 offer partial blocks 
    2 bytes code point needs 3 byte UTF-8 :
    1110 xxxx 10xx xxxx 10xx xxxx
         0010   01 0110   00 0001 =
       E    2    9    6    8    1
  */
  printf("\xE2\x96\x81");
  printf("\xE2\x96\x82");
  printf("\xE2\x96\x83");
  printf("\xE2\x96\x84");
  printf("\xE2\x96\x85");
  printf("\xE2\x96\x86");
  printf("\xE2\x96\x87");
  printf("\xE2\x96\x88\n");

  printf("\033[48;5;186m"); /*set background color to off white*/
  /*see https://misc.flogisoft.com/bash/tip_colors_and_formatting  */
  int i, j;
  for(i = 0; i < 28; i++)
    {
    for(j = 0; j < 28; j++)
      {
      printf("  ");
      }
    printf("\n");
    }

  WINDOW *left, *right, *gutter;
  int maxx, maxy; 

  unsigned char skull[3] = {0xE2, 0x96, 0x81};
  wchar_t skill = 'A';
  cchar_t ch;
  setlocale(LC_ALL, "");
  initscr();
  start_color();

  getmaxyx(stdscr, maxy, maxx);

  left = newwin(maxy, 3*maxx/4-1, 0, 0);
  right = newwin(maxy, maxx/4-1, 0, 3*maxx/4+1);
  gutter = newwin(maxy, 2, 0, 3*maxx/4-1);
  wbkgd(left, COLOR_PAIR(16)); /*note if I want jet black I had better use one from the RGB part of the pallette*/
  wbkgd(right, COLOR_PAIR(16)); /*note if I want jet black I had better use one from the RGB part of the pallette*/
  wbkgd(gutter, COLOR_PAIR(255)); /*note if I want jet black I had better use one from the RGB part of the pallette*/
  //addch('X');
  for(i = 1; i < 256; i ++)
    {
    init_pair(i, 16, i);
    setcchar(&ch, &skill, 0, i, 0);
    wadd_wch(left, &ch);
    }
 
  setcchar(&ch, &skill, 0, 2, 0);
  wadd_wch(left, &ch); 
  waddstr(left, "ksdfasldfjsdf");
 
  init_pair(2, 46, 16);
  skill = 0x30b0;
  for(i = 0; i < 40; i++)
    {
    skill = skill+1;
    setcchar(&ch, &skill, 0, 2, 0);
    mvwadd_wch(right, i, 0, &ch);
    }

  waddstr(gutter, "sakjdfjdsfljds");
  wrefresh(left);
  wrefresh(right);
  wrefresh(gutter);
  endwin();
  return 0;
  }
