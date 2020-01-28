#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

void repeat_char(char c, uint8_t n) {
	for (uint8_t i = 0; i < n; i++) {
		putchar(c);
	}
}

void print_coord_chars(uint8_t size)
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
		// Iterate over adjacent positions (y+1, x+1, x-1, y-1)
		for (uint8_t i = 0; i < 4; i++) {
			Coord adjacent = { curr.x +     i % 2 - i / 2,
					   curr.y + (i+1) % 2 - i / 2 };
			// Check for the edge of the board
			if (adjacent.x >= board->size ||
			    adjacent.y >= board->size) {
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

int main() {
	uint8_t size;
	printf("Pick a board size: ");
	scanf("%"SCNu8, &size);
	if (size != 9 && size != 13 && size != 19) {
		printf("Invalid size!\n");
		exit(1);
	}

	uint8_t high_line = size > 9 ? 4 : 3;

	// Create board
	BoardState board;
	init_board_state(&board, size);

	// Variables for storing user input
	char input_command_buff[1024];
	CoordCharNum coord;

	Color color_playing_turn = BLACK;
	while (true) {
		// --- Render board ---
		printf("--Captures--\n");
		printf("Black: %3d      White: %3d\n",
		       board.captures[0], board.captures[1]);

		print_coord_chars(size);
		// Top border
		repeat_char(' ', size > 9 ? 4 : 3);
		putchar('+');
		repeat_char('-', 2 * size + 1);
		puts("+");
		// Middle
		for (uint8_t i = 0; i < size; i++) {
			printf("%*d ", size > 9 ? 3 : 2, size - i);
			// Right border
			putchar('|');
			// Middle
			for (uint8_t j = 0; j < size; j++) {
				putchar(' ');
				switch (board.spots[size-i-1][j]) {
				case EMPTY:
					if ((i == high_line - 1 ||
					     i == size/2 && size % 2 == 1 &&
					     (size > 13 || i == j) ||
					     i == size - high_line) &
					    (j == high_line - 1 ||
					     j == size/2 && size % 2 == 1 &&
					     (size > 13 || j == i) ||
					     j == size - high_line)) {
						// Star point
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
		putchar('\n');
skip_board_render:
		// --- Read the move ---
		printf("Pick a move ('x' to exit): ");
		fgets(input_command_buff, sizeof(input_command_buff), stdin);
		scanf("%c", &coord.c);
		if (coord.c == 'x') {
			break;
		}
		if (!scanf("%"SCNu8, &coord.n)) {
			printf("Invalid coordinate.\n");
			goto skip_board_render;
		}
		if (!is_valid_coord(coord, size)) {
			printf("Invalid coordinate.\n");
			goto skip_board_render;
		}
		Coord coord_new_move = CoordCharNum_to_Coord(coord);
		if (board.spots[coord_new_move.y][coord_new_move.x] != EMPTY) {
			printf("Invalid move: spot is occupied.\n");
			goto skip_board_render;
		}
		// --- Play the move ---
		// Try to capture groups surrounding the new move
		board.spots[coord_new_move.y]
			   [coord_new_move.x] = color_playing_turn;
		Color opponents_color = OPPOSITE_COLOR(color_playing_turn);
		uint16_t captures = 0;
		// Iterate over adjacent positions (y+1, x+1, x-1, y-1)
		for (uint8_t i = 0; i < 4; i++) {
			Coord adjacent = { coord_new_move.x, coord_new_move.y };
			adjacent.x +=     i % 2 - i / 2;
			adjacent.y += (i+1) % 2 - i / 2;
			// Check for the edge of the board
			if (adjacent.x >= board.size ||
			    adjacent.y >= board.size) {
				continue;
			}
			Stack group;
			if (board.spots[adjacent.y][adjacent.x] ==
			     opponents_color &&
			    !group_has_liberties(adjacent, &group, &board)) {
				// Remove group
				while (!is_empty(&group)) {
					Coord dead_stone = pop(&group);
					// Remove current stone
					board.spots[dead_stone.y]
						   [dead_stone.x] = EMPTY;
					captures++;
				}
			}
		}
		// Check if it's a suicide move
		if (!group_has_liberties(coord_new_move, NULL, &board)) {
			board.spots[coord_new_move.y][coord_new_move.x] = EMPTY;
			printf("Invalid move: suicide.\n");
			goto skip_board_render;
		}
		// Accept the move into the record
		memcpy(&board.previous_move, &coord_new_move,
		       sizeof(coord_new_move));
		add_captures(captures, color_playing_turn, &board);
		color_playing_turn = OPPOSITE_COLOR(color_playing_turn);
	}

	free_board_state(&board);
}
