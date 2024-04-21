#ifndef GAME_OF_LIFE_MODEL_H
#define GAME_OF_LIFE_MODEL_H

/**
 * @brief The type of cell state.
 */
typedef enum {
	cell_state_death = 0,
	cell_state_alive,
} cell_state_t;

typedef cell_state_t cell_t;

/**
 * @brief two-dimensional grid of cells.
 */
typedef struct {
	cell_t **cells;
	int num_row;
	int num_column;
	int num_generation;
} board_t;

/**
 * @brief Transition to the next generation.
 * @return The number of generations. if error occured, return -1
 * @param current The current board
 */
int board_next_generation(board_t *this);

/**
 * @brief Create a new board.
 * @return A pointer of the board. If failed to allocate memory, return NULL.
 * @param num_row The number of rows.
 * @param num_column The number of columns.
 */
board_t *board_init(const int num_row, const int num_column);

/**
 * @brief Release allocated memory.
 */
void board_free(board_t *this);

void board_restart(board_t *this);

#endif
