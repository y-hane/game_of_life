#include <stdbool.h>
#include <stdio.h>
#include "model.h"
#include "view.h"
#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	HDC hdc;
	HBITMAP bmp;
	int width;
	int height;
	HBRUSH background;
} view_t;

enum {
	// color
	window_background_color = RGB(255, 255, 255),
	board_background_color = RGB(240, 240, 240),
	alive_cell_color = RGB(0, 163, 129),
	grid_color = RGB(220, 220, 220),

	// animation speed
	slow_fps = 15,
	fast_fps = 30,

	generation_digit_max = 4,
};

static void view_init(view_t *this, const HDC hdc, const int width,
                      const int height) {
	// Create compatible DC and bitmap.
	this->hdc = CreateCompatibleDC(hdc);
	this->bmp = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(this->hdc, this->bmp);
	this->width = width;
	this->height = height;
}

static void view_free(view_t *this) {
	DeleteObject(this->background);
	DeleteObject(this->bmp);
	DeleteDC(this->hdc);
}

static void view_draw(const view_t *this, const HDC to, const int x,
                      const int y) {
	BitBlt(to, x, y, this->width, this->height, this->hdc, 0, 0, SRCCOPY);
}

static void view_select_background_brush(const view_t *this) {
	SelectObject(this->hdc, this->background);
}

static void fill_rect(const HDC hdc, const int x, const int y, const int width,
                      const int height) {
	PatBlt(hdc, x, y, width, height, PATCOPY);
}

static void view_fill(const view_t *this) {
	fill_rect(this->hdc, 0, 0, this->width, this->height);
}

static void draw_line(const HDC hdc, const int begin_x, const int begin_y,
                      const int end_x, const int end_y) {
	MoveToEx(hdc, begin_x, begin_y, NULL);
	LineTo(hdc, end_x, end_y);
}

static void draw_grids(const HDC hdc, const board_t *board) {
	for (int r = 0; r <= board->num_row; r++)
		draw_line(hdc, 0, r * cell_size, board->num_row * cell_size, r * cell_size);
	for (int c = 0; c <= board->num_column; c++)
		draw_line(hdc, c * cell_size, 0, c * cell_size, board->num_column * cell_size);
}

static void draw_cells(const HDC hdc, const board_t *board) {
	for (int r = 0; r < board->num_row; r++)
		for (int c = 0; c < board->num_column; c++)
			if (board->cells[r][c] == cell_state_alive)
				fill_rect(hdc, c * cell_size, r * cell_size, cell_size, cell_size);
}

static void set_fps(const HWND hwnd, const int fps) {
	KillTimer(hwnd, 1);
	SetTimer(hwnd, 1, 1000 / fps, NULL);
	InvalidateRect(hwnd, NULL, false);
}

static void draw_generation(const HDC hdc, const int num_generation) {
	if (num_generation >= 10000)
		return;
	static const char label[] = "Generation: ";
	char buf[LENGTH(label) + generation_digit_max + 1];
	sprintf_s(buf, LENGTH(buf), "%s%d", label, num_generation);
	TextOutA(hdc, margin, 0, buf, (int)strlen(buf));
}

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	static board_t *board;
	static view_t window_view;
	static view_t board_view;

	static HBRUSH alive_cell_brush;
	static HPEN grid_pen;
	static HFONT font;

	static int fps = slow_fps;
	static int num_generation = 0;

	switch (msg) {
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		DeleteObject(alive_cell_brush);
		DeleteObject(grid_pen);
		DeleteObject(font);
		view_free(&window_view);
		view_free(&board_view);
		board_free(board);
		PostQuitMessage(0);
		break;
	case WM_TIMER: {
		num_generation = board_next_generation(board);
		InvalidateRect(hwnd, NULL, false);
		break;
	}
	case WM_PAINT: {
		view_fill(&window_view);
		draw_generation(window_view.hdc, num_generation);
		view_select_background_brush(&board_view);
		view_fill(&board_view);
		SelectObject(board_view.hdc, alive_cell_brush);
		draw_cells(board_view.hdc, board);
		draw_grids(board_view.hdc, board);
		view_draw(&board_view, window_view.hdc, margin, font_height);

		PAINTSTRUCT ps;
		const HDC hdc = BeginPaint(hwnd, &ps);
		view_draw(&window_view, hdc, 0, 0);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_COMMAND:
		if (lp != 0)
			break;
		switch (LOWORD(wp)) {
		case 11:
			board_restart(board);
			break;
		case 21:
			fps = slow_fps;
			set_fps(hwnd, fps);
			break;
		case 22:
			fps = fast_fps;
			set_fps(hwnd, fps);
			break;
		default:
			break;
		}
		break;
	case WM_CREATE: {
		board = board_init(num_row, num_column);
		if (board == NULL) {
			PostQuitMessage(0);
			break;
		}

		const HDC hdc = GetDC(hwnd);
		view_init(&window_view, hdc, window_width, window_height);
		view_init(&board_view, hdc, board_width, board_height);
		ReleaseDC(hwnd, hdc);

		window_view.background = CreateSolidBrush(window_background_color);
		board_view.background = CreateSolidBrush(board_background_color);
		alive_cell_brush = CreateSolidBrush(alive_cell_color);
		grid_pen = CreatePen(PS_SOLID, 1, grid_color);
		font = CreateFontA(font_height, 0, 0, 0, FW_REGULAR, false, false, false,
		                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		                   DEFAULT_QUALITY, 0, "Meiryo");

		view_select_background_brush(&window_view);
		SelectObject(window_view.hdc, font);
		SelectObject(board_view.hdc, grid_pen);

		SetTimer(hwnd, 1, 1000 / fps, NULL);
		InvalidateRect(hwnd, NULL, false);
		break;
	}
	default:
		return DefWindowProcA(hwnd, msg, wp, lp);
	}
		return 0;
}
