#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "model.h"

static board_t *new_board(const int num_row, const int num_column) {
	board_t *board = calloc(1, sizeof(board_t));
	if (board == NULL)
		return NULL;

	board->cells = calloc(num_row, sizeof(cell_t *));
	if (board->cells == NULL) {
		free(board);
		return NULL;
	}

	for (int i = 0; i < num_row; i++) {
		board->cells[i] = calloc(num_column, sizeof(cell_t));
		if (board->cells[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(board->cells[j]);
			return NULL;
		}
	}

	board->num_row = num_row;
	board->num_column = num_column;	

	return board;
}

static board_t *board_copy(const board_t *from) {
	board_t *to = new_board(from->num_row, from->num_row);
	if (to == NULL)
		return NULL;

	for (int r = 0; r < from->num_row; r++)
		for (int c = 0; c < from->num_column; c++)
			to->cells[r][c] = from->cells[r][c];

	return to;
}

static bool is_outside(const board_t *this, const int r, const int c) {
	return r < 0 || r >= this->num_row || c < 0 || c >= this->num_column;
}

static int count_alive_cells(const board_t *this, const int r, const int c) {
	int count = 0;
	for (int dr = -1; dr <= 1; dr++) {
		for (int dc = -1; dc <= 1; dc++) {
			if (dr == 0 && dc == 0)
				continue;
			if (is_outside(this, r + dr, c + dc))
				continue;
			if (this->cells[r + dr][c + dc] == cell_state_alive)
				count++;
		}
	}
	return count;
}

board_t *board_init(const int num_row, const int num_column) {
	// Initialize rand
	srand((unsigned int)time(NULL));

	board_t *board = new_board(num_row, num_column);
	if (board == NULL)
		return NULL;

	for (int r = 0; r < board->num_row; r++)
		for (int c = 0; c < board->num_column; c++)
			board->cells[r][c] = rand() % 2;

	return board;
}

void board_free(board_t *this) {
	for (int i = 0; i < this->num_row; i++)
		free(this->cells[i]);
	free(this->cells);
	free(this);
}

static bool is_born(const cell_t *cell, const int count) {
	return *cell == cell_state_death && count == 3;
}

static bool will_survive(const cell_t *cell, const int count) {
	return *cell == cell_state_alive && (count == 2 || count == 3);
}

static bool be_underpopulated(const cell_t *cell, const int count) {
	return *cell == cell_state_alive && count <= 1;
}

static bool be_overcrowded(const cell_t *cell, const int count) {
	return *cell == cell_state_alive && count >= 4;
}

int board_next_generation(board_t *this) {
	board_t *next = this;
	board_t *old = board_copy(this);
	if (old == NULL)
		return -1;

	for (int r = 0; r < old->num_row; r++) {
		for (int c = 0; c < old->num_column; c++) {
			const int count = count_alive_cells(old, r, c);
			const cell_t *old_cell = &old->cells[r][c];
			cell_t *next_cell = &next->cells[r][c];
			if (is_born(old_cell, count))
				*next_cell = cell_state_alive;
			else if (will_survive(old_cell, count))
				*next_cell = cell_state_alive;
			else if (be_underpopulated(old_cell, count))
				*next_cell = cell_state_death;
			else if (be_overcrowded(old_cell, count))
				*next_cell = cell_state_death;
		}
	}

	board_free(old);

	this->num_generation++;
	return this->num_generation;
}

void board_restart(board_t *this) {
	for (int r = 0; r < this->num_row; r++)
		for (int c = 0; c < this->num_column; c++)
			this->cells[r][c] = rand() % 2;
	this->num_generation = 0;
}
