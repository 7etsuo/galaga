#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdbool.h>

#define TERM_MIN_WIDTH 80
#define TERM_MIN_HEIGHT 24
#define TERM_MAX_WIDTH 120
#define TERM_MAX_HEIGHT 40

typedef struct
{
    char *buffer;
    int width;
    int height;
    bool cursor_visible;
} TerminalBuffer;

void terminal_init(void);
void terminal_cleanup(void);
void terminal_get_size(int *width, int *height);
bool terminal_validate_size(void);

TerminalBuffer *terminal_buffer_create(int width, int height);
void terminal_buffer_destroy(TerminalBuffer *buf);
void terminal_buffer_clear(TerminalBuffer *buf);
void terminal_buffer_set_char(TerminalBuffer *buf, int x, int y, char ch, uint8_t color);
void terminal_buffer_set_string(TerminalBuffer *buf, int x, int y, const char *str, uint8_t color);
void terminal_buffer_flush(TerminalBuffer *buf);

void terminal_hide_cursor(void);
void terminal_show_cursor(void);
void terminal_clear_screen(void);
void terminal_move_cursor(int x, int y);
void terminal_set_color(uint8_t color);
void terminal_reset_color(void);

#define COLOR_BLACK 0
#define COLOR_RED 196
#define COLOR_GREEN 46
#define COLOR_YELLOW 226
#define COLOR_BLUE 21
#define COLOR_MAGENTA 201
#define COLOR_CYAN 51
#define COLOR_WHITE 231
#define COLOR_GRAY 240
#define COLOR_ORANGE 208
#define COLOR_PINK 213
#define COLOR_PURPLE 93

#endif
