#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef enum
{
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_Q,
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_G,
    KEY_B,
    KEY_X,
    KEY_ESC
} KeyCode;

typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool shoot;
    bool quit;
    bool god_toggle;
    bool bomb;
    bool special;
    float up_time;
    float down_time;
    float left_time;
    float right_time;
    float shoot_time;
} InputState;

void input_init(void);
void input_cleanup(void);
KeyCode input_read_key(void);
void input_update_state(InputState *state, float dt);

#endif
