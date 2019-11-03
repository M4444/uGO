#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum BOARD_SIZES {
	SMALL = 9,
	MEDIUM = 13,
	LARGE = 19
};

typedef enum {
	EMPTY,
	BLACK,
	WHITE
} SPOT_STATE;

typedef struct {
	int x;
	int y;
} Coord;

typedef struct {
	int size;
	SPOT_STATE **spots;
	Coord last_move;
} BoardState;

void repeat_char(char c, int n) {
	for (int i = 0; i < n; i++) {
		putchar(c);
	}
}

bool is_star_point(int size, int x, int y) {
int high_line = size > 9 ? 4 : 3;

return (x == high_line - 1 ||
        x == size/2 && size % 2 == 1 && (size > 9 || x == y) ||
        x == size - high_line) &
       (y == high_line - 1 ||
        y == size/2 && size % 2 == 1 && (size > 9 || y == x) ||
        y == size - high_line);
}

void render_board(BoardState *b)
{
	int size = b->size;
	// Top border
	putchar('+');
	repeat_char('-', 2 * size + 1);
	puts("+");
	// Middle
	for (int i = 0; i < size; i++) {
		// Right border
		putchar('|');
		// Middle
		for (int j = 0; j < size; j++) {
			putchar(' ');
			switch (b->spots[i][j]) {
			case EMPTY:
				if (is_star_point(size, i, j)) {
					putchar('+');
				} else {
					putchar('.');
				}
				break;
			case BLACK:
				putchar('X');
				break;
			case WHITE:
				putchar('O');
				break;
			}
		}
		// Left border
		puts(" |");
	}
	// Bottom border
	putchar('+');
	repeat_char('-', 2 * size + 1);
	puts("+");
}

int main() {
	int size = 19;
	BoardState board;
	board.size = size;
	board.spots = malloc(board.size * sizeof(SPOT_STATE *));
	for (int i = 0; i < board.size; i++) {
		board.spots[i] = malloc(board.size * sizeof(SPOT_STATE));
	}

	board.size = 9;
	render_board(&board);
	board.size = 13;
	render_board(&board);
	board.size = 19;
	render_board(&board);

	return 0;
}
