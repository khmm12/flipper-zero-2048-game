#pragma once
#include <gui/gui.h>
#include "game_state.h"

typedef enum {
    UIStateMenu,
    UIStateInProgress,
    UIStateGameOver,
} UIState;

typedef struct {
    UIState ui_state;
    GameState state;
    int8_t selected_menu_item;
} GameController;

typedef struct {
    bool is_handled;
    bool should_exit;
} GameControllerInputHandlerResult;

void game_controlller_init(GameController* gamectrl);
void game_controller_save_state(GameController* gamectrl);
bool game_controller_restore_state(GameController* gamectrl);
void game_controller_draw(const GameController* gamectrl, Canvas* const canvas);
void game_controller_handle_input(
    GameController* gamectrl,
    InputEvent input,
    GameControllerInputHandlerResult* out);

#ifdef __game_controller_c
#define MENU_ITEMS_COUNT 2
void game_controller_show_menu(GameController* gamectrl);
void game_controller_close_menu(GameController* gamectrl);
#endif
