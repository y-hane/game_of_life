#ifndef GAME_OF_LIFE_VIEW_H
#define GAME_OF_LIFE_VIEW_H
#include <windows.h>

// constants
enum {
	// Board size
	num_row = 50,
	num_column = 50,
	cell_size = 10,
	board_width = cell_size * num_column + 1, // for grid
	board_height = cell_size * num_row + 1, // for gird

	// label size
	font_height = 30,

	// window size
	margin = cell_size,
	window_width = board_width + (margin * 2),
	window_height = board_height + (margin * 3) + font_height, // add offset of menu bar
};

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

#endif
