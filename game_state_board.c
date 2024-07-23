#include "game_state_board.h"

void game_state_board_init(GameStateBoard* board) {
    game_board_table_init(board->table);
    board->score = 0;
    board->moves = 0;
}

bool game_state_board_is_over(GameStateBoard* const board) {
    return !game_board_table_has_empty_cells(board->table) &&
           !game_board_table_can_move(board->table);
}
