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
	EMPTY = 0,
	BLACK = 1,
	WHITE = 2
} SPOT_STATE;

typedef SPOT_STATE Color;

#define OPPOSITE_COLOR(color) ((color) % 2 + 1)

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
	int captures[2];
} BoardState;

void add_captures(int num, Color color, BoardState *board) {
	board->captures[color - 1] += num;
}

void init_board_state(BoardState *board, int size) {
	board->size = size;
	board->spots = malloc(size * sizeof(SPOT_STATE *));
	for (int i = 0; i < size; i++) {
		board->spots[i] = malloc(size * sizeof(SPOT_STATE));
	}
	board->last_move.x = -1;
	board->last_move.y = -1;
	board->captures[0] = 0;
	board->captures[1] = 0;
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

// Stack
typedef struct {
	Coord *buff;
	Coord *last;
	Coord *end;
} Stack;

void init_stack(Stack *stack) {
	stack->buff = NULL;
	stack->last = NULL;
	stack->end = NULL;
}

void free_stack(Stack *stack) {
	free(stack->buff);
	stack->buff = NULL;
	stack->last = NULL;
	stack->end = NULL;
}

bool is_empty(Stack *stack) {
	return stack->last == stack->buff;
}

bool contains(Coord *element, Stack *stack) {
	Coord *curr = stack->buff;
	while (curr != stack->last) {
		if (element->x == curr->x && element->y == curr->y) {
			return true;
		}
		curr++;
	}
	return false;
}

int count(Stack *stack) {
	return (stack->last - stack->buff) / sizeof(Coord);
}

void *push(Coord *element, Stack *stack) {
	if (stack->buff == NULL || stack->last == stack->end) {
		int old_stack_size = stack->last - stack->buff;
		int new_stack_size = old_stack_size == 0 ? 16 * sizeof(Coord) :
							   2 * old_stack_size;
		stack->buff = realloc(stack->buff, new_stack_size);
		stack->last = stack->buff + old_stack_size;
		stack->end = stack->buff + new_stack_size;
	}
	memcpy(++stack->last, element, sizeof(Coord));
}

Coord pop(Stack *stack) {
	if (stack->last == stack->buff) {
		Coord ret = { -1, -1 };
		return ret;
	}
	return *stack->last--;
}

#endif
