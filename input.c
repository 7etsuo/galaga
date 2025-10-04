#include "input.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

/* Input timing constants */
#define KEY_HOLD_TIME 0.75f

static int original_flags = 0;

void input_init(void)
{
    original_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, original_flags | O_NONBLOCK);
}

void input_cleanup(void)
{
    fcntl(STDIN_FILENO, F_SETFL, original_flags);
}

KeyCode input_read_key(void)
{
    char buf[3];
    int n = read(STDIN_FILENO, buf, sizeof(buf));

    if (n <= 0)
        return KEY_NONE;

    /* Single character keys */
    if (n == 1)
    {
        switch (buf[0])
        {
        case ' ':
            return KEY_SPACE;
        case 'q':
        case 'Q':
            return KEY_Q;
        case 'w':
        case 'W':
            return KEY_W;
        case 'a':
        case 'A':
            return KEY_A;
        case 's':
        case 'S':
            return KEY_S;
        case 'd':
        case 'D':
            return KEY_D;
        case 'g':
        case 'G':
            return KEY_G;
        case 'b':
        case 'B':
            return KEY_B;
        case 'x':
        case 'X':
            return KEY_X;
        case 27:
            return KEY_ESC;
        default:
            return KEY_NONE;
        }
    }

    /* Arrow key escape sequences */
    if (n >= 3 && buf[0] == 27 && buf[1] == '[')
    {
        switch (buf[2])
        {
        case 'A':
            return KEY_UP;
        case 'B':
            return KEY_DOWN;
        case 'C':
            return KEY_RIGHT;
        case 'D':
            return KEY_LEFT;
        default:
            return KEY_NONE;
        }
    }

    return KEY_NONE;
}

void input_update_state(InputState *state, float dt)
{
    /* Read all pending key presses */
    KeyCode key;
    while ((key = input_read_key()) != KEY_NONE)
    {
        switch (key)
        {
        case KEY_UP:
        case KEY_W:
            state->up = true;
            state->up_time = KEY_HOLD_TIME;
            break;
        case KEY_DOWN:
        case KEY_S:
            state->down = true;
            state->down_time = KEY_HOLD_TIME;
            break;
        case KEY_LEFT:
        case KEY_A:
            state->left = true;
            state->left_time = KEY_HOLD_TIME;
            break;
        case KEY_RIGHT:
        case KEY_D:
            state->right = true;
            state->right_time = KEY_HOLD_TIME;
            break;
        case KEY_SPACE:
            state->shoot = true;
            state->shoot_time = KEY_HOLD_TIME;
            break;
        case KEY_G:
            state->god_toggle = true;
            break;
        case KEY_B:
            state->bomb = true;
            break;
        case KEY_X:
            state->special = true;
            break;
        case KEY_Q:
        case KEY_ESC:
            state->quit = true;
            break;
        default:
            break;
        }
    }

    /* Decay key timers */
    if (state->up_time > 0.0f)
    {
        state->up_time -= dt;
        if (state->up_time <= 0.0f)
            state->up = false;
    }

    if (state->down_time > 0.0f)
    {
        state->down_time -= dt;
        if (state->down_time <= 0.0f)
            state->down = false;
    }

    if (state->left_time > 0.0f)
    {
        state->left_time -= dt;
        if (state->left_time <= 0.0f)
            state->left = false;
    }

    if (state->right_time > 0.0f)
    {
        state->right_time -= dt;
        if (state->right_time <= 0.0f)
            state->right = false;
    }

    if (state->shoot_time > 0.0f)
    {
        state->shoot_time -= dt;
        if (state->shoot_time <= 0.0f)
            state->shoot = false;
    }
}
