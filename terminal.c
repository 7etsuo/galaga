#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#define OUTPUT_BUFFER_SIZE (80 * 40 * 20)

static struct termios orig_termios;
static bool terminal_initialized = false;

void terminal_init(void)
{
    if (terminal_initialized)
        return;

    /* Save terminal state */
    tcgetattr(STDIN_FILENO, &orig_termios);

    /* Configure raw mode */
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    /* Use alternate screen buffer (compatible with most terminals) */
    printf("\033[?1049h");

    /* Disable line wrapping to prevent auto-scroll */
    printf("\033[?7l");

    /* Clear screen and hide cursor */
    printf("\033[2J\033[H\033[?25l");
    fflush(stdout);

    terminal_initialized = true;
}

void terminal_cleanup(void)
{
    if (!terminal_initialized)
        return;

    /* Re-enable line wrapping */
    printf("\033[?7h");

    /* Show cursor and reset colors */
    printf("\033[?25h\033[0m");

    /* Exit alternate screen buffer */
    printf("\033[?1049l");
    fflush(stdout);

    /* Restore terminal settings */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    terminal_initialized = false;
}

void terminal_get_size(int *width, int *height)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        *width = 80;
        *height = 24;
    }
    else
    {
        *width = ws.ws_col;
        *height = ws.ws_row;
    }
}

bool terminal_validate_size(void)
{
    int width, height;
    terminal_get_size(&width, &height);
    return (width >= TERM_MIN_WIDTH && height >= TERM_MIN_HEIGHT);
}

TerminalBuffer *terminal_buffer_create(int width, int height)
{
    TerminalBuffer *buf = malloc(sizeof(TerminalBuffer));
    if (!buf)
    {
        return NULL;
    }

    buf->width = width;
    buf->height = height;
    buf->cursor_visible = false;
    buf->buffer = calloc(width * height * 2, sizeof(char));

    if (!buf->buffer)
    {
        free(buf);
        return NULL;
    }

    terminal_buffer_clear(buf);
    return buf;
}

void terminal_buffer_destroy(TerminalBuffer *buf)
{
    if (buf)
    {
        free(buf->buffer);
        free(buf);
    }
}

void terminal_buffer_clear(TerminalBuffer *buf)
{
    if (!buf || !buf->buffer)
        return;

    /* Clear buffer properly: set chars to space and colors to default */
    int total_cells = buf->width * buf->height;
    for (int i = 0; i < total_cells; i++)
    {
        buf->buffer[i * 2] = ' ';             /* Character */
        buf->buffer[i * 2 + 1] = COLOR_BLACK; /* Color */
    }
}

void terminal_buffer_set_char(TerminalBuffer *buf, int x, int y, char ch, uint8_t color)
{
    /* Silently ignore out-of-bounds writes */
    if (!buf || x < 0 || x >= buf->width || y < 0 || y >= buf->height)
        return;

    int index = (y * buf->width + x) * 2;
    buf->buffer[index] = ch;
    buf->buffer[index + 1] = color;
}

void terminal_buffer_set_string(TerminalBuffer *buf, int x, int y, const char *str, uint8_t color)
{
    if (!buf || !str)
    {
        return;
    }

    int i = 0;
    while (str[i] != '\0' && (x + i) < buf->width)
    {
        terminal_buffer_set_char(buf, x + i, y, str[i], color);
        i++;
    }
}

void terminal_buffer_flush(TerminalBuffer *buf)
{
    if (!buf || !buf->buffer)
        return;

    /* Use single write buffer to reduce flickering */
    static char output_buffer[OUTPUT_BUFFER_SIZE];
    int pos = 0;

    /* Start from top-left */
    pos += sprintf(output_buffer + pos, "\033[1;1H");

    uint8_t current_color = 255;

    /* Draw all lines except the very last one to prevent scrolling */
    int safe_height = buf->height - 1; /* Render 0-22, skip line 23 */

    for (int y = 0; y < safe_height; y++)
    {
        /* Position cursor at start of line */
        if (y > 0)
            pos += sprintf(output_buffer + pos, "\033[%d;1H", y + 1);

        for (int x = 0; x < buf->width; x++)
        {
            /* On line 22, stop before the last character to prevent scrolling */
            if (y == safe_height - 1 && x == buf->width - 1)
                break;

            int index = (y * buf->width + x) * 2;
            char ch = buf->buffer[index];
            uint8_t color = buf->buffer[index + 1];

            /* Only change color when needed */
            if (color != current_color)
            {
                pos += sprintf(output_buffer + pos, "\033[38;5;%dm", color);
                current_color = color;
            }

            output_buffer[pos++] = ch;
        }
    }

    /* Reset color */
    pos += sprintf(output_buffer + pos, "\033[0m");

    /* Single atomic write to reduce tearing */
    write(STDOUT_FILENO, output_buffer, pos);
}

void terminal_hide_cursor(void)
{
    printf("\033[?25l");
    fflush(stdout);
}

void terminal_show_cursor(void)
{
    printf("\033[?25h");
    fflush(stdout);
}

void terminal_clear_screen(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

void terminal_move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y + 1, x + 1);
}

void terminal_set_color(uint8_t color)
{
    printf("\033[38;5;%dm", color);
}

void terminal_reset_color(void)
{
    printf("\033[0m");
    fflush(stdout);
}
