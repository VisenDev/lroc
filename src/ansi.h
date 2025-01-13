#include <stdio.h>
#include <stdint.h>

#define ANSI_ESC "\x1b["
#define ANSI_CURSOR_RESET ANSI_ESC "0;0H"
#define ANSI_CLEAR ANSI_ESC "2J"
#define ANSI_CLEAR_SCREEN ANSI_CURSOR_RESET ANSI_CLEAR

#define ANSI_8BIT_FG(color_code) ANSI_ESC "38;5;" color_code "m"
#define ANSI_8BIT_BG(color_code) ANSI_ESC "48;5;" color_code "m"
#define ANSI_GOTO(x, y) ANSI_ESC x ";" y "H"
