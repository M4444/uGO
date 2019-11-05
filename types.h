#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdlib.h>

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
	char c;
	int n;
} CoordCharNum;

bool is_valid_coord(CoordCharNum coord, int size) {
	return (coord.c >= 'a' && coord.c < 'a' + 1 + size && coord.c != 'i' ||
		coord.c >= 'A' && coord.c < 'A' + 1 + size && coord.c != 'I') &&
	       coord.n > 0 && coord.n <= size;
}

typedef struct {
	int x;
	int y;
} Coord;

typedef struct {
	int size;
	SPOT_STATE **spots;
	Coord last_move;
} BoardState;

void init_board_state(BoardState *board, int size) {
	board->size = size;
	board->spots = malloc(size * sizeof(SPOT_STATE *));
	for (int i = 0; i < size; i++) {
		board->spots[i] = malloc(size * sizeof(SPOT_STATE));
	}
	board->last_move.x = -1;
	board->last_move.y = -1;
}

void free_board_state(BoardState *board) {
	for (int i = 0; i < board->size; i++) {
		free(board->spots[i]);
	}
	free(board->spots);
	board->size = 0;
	board->last_move.x = -1;
	board->last_move.y = -1;
}

#endif
