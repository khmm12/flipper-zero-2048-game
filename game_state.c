#define __game_state_c
#include "game_state.h"

#include <furi.h>

void game_state_reset(GameState* const game_state);
void game_state_post_update(GameState* const state);
void game_state_undo(GameState* const state);
void game_state_apply_move_result(GameState* const state, const MoveResult* const move_result);
void game_state_save_score(GameState* const state);

void game_state_init(GameState* const game_state) {
    game_state_reset(game_state);
}

void game_state_send(GameState* state, GameEvent event) {
    MoveResult move_result;

    switch(event) {
    case GameMoveLeft:
        game_board_table_move_left(state->board.table, &move_result);
        game_state_apply_move_result(state, &move_result);
        break;
    case GameMoveRight:
        game_board_table_move_right(state->board.table, &move_result);
        game_state_apply_move_result(state, &move_result);
        break;
    case GameMoveUp:
        game_board_table_move_up(state->board.table, &move_result);
        game_state_apply_move_result(state, &move_result);
        break;
    case GameMoveDown:
        game_board_table_move_down(state->board.table, &move_result);
        game_state_apply_move_result(state, &move_result);
        break;
    case GameMoveUndo:
        game_state_undo(state);
        break;
    case GameReset:
        game_state_reset(state);
        break;
    default:
        break;
    }
}

bool game_state_dump(GameState* const state, GameStateDumpCallback cb) {
    return cb(state);
}

bool game_state_load(GameState* state, GameStateLoadCallback cb) {
    GameState tmp;
    game_state_init(&tmp);

    if(cb(&tmp)) {
        memcpy(state, &tmp, sizeof(GameState));
        return true;
    }

    return false;
}

void game_state_reset(GameState* const state) {
    // Reset board
    game_state_board_init(&state->board);

    // Reset history stack
    game_state_board_history_init(&state->history);

    // Reset game state
    state->is_over = false;
}

void game_state_undo(GameState* const state) {
    game_state_board_history_pop(&state->history, &state->board);
    game_state_post_update(state);
}

void game_state_apply_move_result(GameState* const state, const MoveResult* const move_result) {
    if(!move_result->is_table_updated) return;

    // Save the current state to the history stack
    game_state_board_history_push(&state->history, &state->board);

    // Apply the move to the board
    state->board.score += move_result->score_points;
    state->board.moves++;
    game_board_table_copy(move_result->new_table, state->board.table);

    // Add a new random digit to the board
    game_board_table_push_random_digit(state->board.table);

    // Apply the move to the game state
    game_state_post_update(state);
}

void game_state_post_update(GameState* const state) {
    if(game_state_board_is_over(&state->board)) {
        state->is_over = true;
        game_state_save_score(state);
    } else {
        state->is_over = false;
    }
}

void game_state_save_score(GameState* const state) {
    if(state->board.score >= state->top_score) {
        state->top_score = state->board.score;
    }
}
