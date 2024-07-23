#define __game_controller_c
#include "game_controller.h"

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <storage/storage.h>
#include "app_shared.h"

static bool game_state_load_callback(GameState* state);
static bool game_state_save_callback(GameState* gamectrl);

void game_controlller_init(GameController* gamectrl) {
    game_state_init(&gamectrl->state);
    game_state_load(&gamectrl->state, game_state_load_callback);
    gamectrl->ui_state = gamectrl->state.is_over ? UIStateGameOver : UIStateInProgress;
}

void game_controller_save_state(GameController* gamectrl) {
    game_state_dump(&gamectrl->state, game_state_save_callback);
}

void game_controller_show_menu(GameController* gamectrl) {
    gamectrl->ui_state = UIStateMenu;
    gamectrl->selected_menu_item = 0;
}

void game_controller_close_menu(GameController* gamectrl) {
    gamectrl->ui_state = UIStateInProgress;
}

bool game_state_load_callback(GameState* state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    bool is_loaded = false;

    File* file = storage_file_alloc(storage);
    uint16_t bytes_readed = 0;
    if(storage_file_open(file, SAVE_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
        bytes_readed = storage_file_read(file, state, sizeof(GameState));
        is_loaded = true;
    }

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    if(is_loaded) {
        is_loaded = bytes_readed == sizeof(GameState);

        if(is_loaded) {
            FURI_LOG_I(LOG_TAG, "Game state loaded");
        } else {
            FURI_LOG_W(LOG_TAG, "Failed to load game. The storage file contains invalid data.");
        }
    } else {
        FURI_LOG_I(LOG_TAG, "Failed to load game. The storage file does not exist.");
    }

    return is_loaded;
}

bool game_state_save_callback(GameState* gamectrl) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    bool is_saved = false;

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVE_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, gamectrl, sizeof(GameState));
        is_saved = true;
    }

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    if(is_saved) {
        FURI_LOG_I(LOG_TAG, "Game state saved");
    } else {
        FURI_LOG_E(LOG_TAG, "Failed to save game state. The storage file can not be opened.");
    }

    return is_saved;
}
