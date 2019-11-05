#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

void repeat_char(char c, int n) {
	for (int i = 0; i < n; i++) {
		putchar(c);
	}
}

void print_coord_chars(int size)
{
	repeat_char(' ', size > 9 ? 6 : 5);
	for (char c = 'A'; c < 'A' + 1 + size; c++) {
		if (c == 'I') {
			c++;
		}
		printf("%c ", c);
	}
	putchar('\n');
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

	print_coord_chars(size);
	// Top border
	repeat_char(' ', size > 9 ? 4 : 3);
	putchar('+');
	repeat_char('-', 2 * size + 1);
	puts("+");
	// Middle
	for (int i = 0; i < size; i++) {
		printf("%*d ", size > 9 ? 3 : 2, size - i);
		// Right border
		putchar('|');
		// Middle
		for (int j = 0; j < size; j++) {
			putchar(' ');
			switch (b->spots[size-i-1][j]) {
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
		printf(" |%*d\n", size > 9 ? 3 : 2, size - i);
	}
	// Bottom border
	repeat_char(' ', size > 9 ? 4 : 3);
	putchar('+');
	repeat_char('-', 2 * size + 1);
	puts("+");
	print_coord_chars(size);
}

void play_move(CoordCharNum move, BoardState *board) {
	bool blacks_turn;
	if (board->last_move.x < 0 ||
	    board->spots[board->last_move.y][board->last_move.x] == WHITE) {
		blacks_turn = true;
	} else {
		blacks_turn = false;
	}
	if (move.c >= 'A' && move.c <= 'Z') {
		move.c = move.c - 'A' + 'a';
	}
	Coord new_move = { move.c - 'a', move.n - 1 };
	if (move.c > 'h') {
		new_move.x--;
	}
	board->spots[new_move.y][new_move.x] = blacks_turn ? BLACK : WHITE;
	memcpy(&board->last_move, &new_move, sizeof(new_move));
}

int main() {
	int size;
	printf("Pick a board size: ");
	scanf("%d", &size);
	if (size != 9 && size != 13 && size != 19) {
		printf("Invalid size!\n");
		exit(1);
	}

	// Create board
	BoardState board;
	board.size = size;
	board.spots = malloc(board.size * sizeof(SPOT_STATE *));
	for (int i = 0; i < board.size; i++) {
		board.spots[i] = malloc(board.size * sizeof(SPOT_STATE));
	}
	board.last_move.x = -1;
	board.last_move.y = -1;

	render_board(&board);
	putchar('\n');
	while (true) {
		char buff[1024];
		CoordCharNum coord;
		printf("Pick a move: ");
		fgets(buff, sizeof(buff), stdin);
		scanf("%c", &coord.c);
		if (coord.c == 'q') {
			break;
		}
		if (!scanf("%d", &coord.n)) {
			printf("Invalid move.\n");
			continue;
		}
		if (!is_valid_coord(coord, size)) {
			printf("Invalid move.\n");
			continue;
		}
		play_move(coord, &board);
		render_board(&board);
		putchar('\n');
	}

	free(board.spots);
	return 0;
}
