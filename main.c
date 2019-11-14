#include <stdbool.h>
#include <stdio.h>
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
        x == size/2 && size % 2 == 1 && (size > 13 || x == y) ||
        x == size - high_line) &
       (y == high_line - 1 ||
        y == size/2 && size % 2 == 1 && (size > 13 || y == x) ||
        y == size - high_line);
}

void render_board(BoardState *b)
{
	printf("--Captures--\n");
	printf("Black: %3d      White: %3d\n", b->captures[0], b->captures[1]);

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

// If position has no liberties, fill group (if it's non-NULL) and return false.
// If position has liberties, free group and return true.
bool group_has_liberties(Coord position, Stack *group, BoardState *board) {
	Stack *processed_stones = group ? group : malloc(sizeof(Stack));
	Stack unprocessed_stones;
	init_stack(processed_stones);
	init_stack(&unprocessed_stones);

	Color group_color = board->spots[position.y][position.x];
	push(&position, &unprocessed_stones);
	while (!is_empty(&unprocessed_stones)) {
		Coord curr = pop(&unprocessed_stones);
		if (contains(&curr, processed_stones)) {
			continue;
		}
		for (int i = 0; i < 4; i++) {
			Coord adjacent = { curr.x +     i % 2 - i / 2,
					   curr.y + (i+1) % 2 - i / 2 };
			// Check for the edge of the board
			if (adjacent.x < 0 || adjacent.x >= board->size ||
			    adjacent.y < 0 || adjacent.y >= board->size) {
				continue;
			}
			SPOT_STATE state = board->spots[adjacent.y][adjacent.x];
			if (state == EMPTY) {
				free_stack(processed_stones);
				return true;
			} else if (state == group_color) {
				push(&adjacent, &unprocessed_stones);
			}
		}
		push(&curr, processed_stones);
	}
	if (!group) {
		free_stack(processed_stones);
	}
	return false;
}

void remove_group(Stack *group, BoardState *board) {
	Color group_color = 0;
	while (!is_empty(group)) {
		Coord curr = pop(group);
		if (!group_color) {
			group_color = board->spots[curr.y][curr.x];
		}
		// Remove current stone
		board->spots[curr.y][curr.x] = EMPTY;
		add_captures(1, OPPOSITE_COLOR(group_color), board);
	}
	free_stack(group);
}

bool play_move(CoordCharNum move, BoardState *board) {
	// Find out whose turn it is
	Color move_color = board->last_move.x < 0 ||
			   board->spots[board->last_move.y][board->last_move.x]
			    == WHITE ? BLACK : WHITE;
	// Transform coordinates
	if (move.c >= 'A' && move.c <= 'Z') {
		move.c = move.c - 'A' + 'a';
	}
	Coord new_move = { move.c - 'a', move.n - 1 };
	if (move.c > 'h') {
		new_move.x--;
	}
	// Check legality of the move
	if (board->spots[new_move.y][new_move.x] != EMPTY ||
	    new_move.x == board->last_move.x &&
	    new_move.y == board->last_move.y) {
		return false;
	}
	// Capture groups surrounding the new move
	board->spots[new_move.y][new_move.x] = move_color;
	for (int i = 0; i < 4; i++) {
		Coord adjacent = { new_move.x, new_move.y };
		adjacent.x +=     i % 2 - i / 2;
		adjacent.y += (i+1) % 2 - i / 2;
		// Check for the edge of the board
		if (adjacent.x < 0 || adjacent.x >= board->size ||
		    adjacent.y < 0 || adjacent.y >= board->size) {
			continue;
		}
		Stack group;
		if (board->spots[adjacent.y][adjacent.x] ==
		     OPPOSITE_COLOR(move_color) &&
		    !group_has_liberties(adjacent, &group, board)) {
			remove_group(&group, board);
		}
	}
	// Check if it's a suicide move
	if (!group_has_liberties(new_move, NULL, board)) {
		board->spots[new_move.y][new_move.x] = EMPTY;
		return false;
	}
	// Accept the move into the record
	memcpy(&board->last_move, &new_move, sizeof(new_move));
	return true;
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
	init_board_state(&board, size);

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
			printf("Invalid coordinate.\n");
			continue;
		}
		if (!is_valid_coord(coord, size)) {
			printf("Invalid coordinate.\n");
			continue;
		}
		if (!play_move(coord, &board)) {
			printf("Invalid move.\n");
			continue;
		}
		render_board(&board);
		putchar('\n');
	}

	free_board_state(&board);
	return 0;
}
