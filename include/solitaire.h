#ifndef SOLITAIRE_H
#define SOLITAIRE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SOL_COLS 7
#define SOL_MAX_PILE 20

typedef struct {
    uint8_t rank; /* 1 (Ace) .. 13 (King) */
    uint8_t suit; /* 0=Spades(B), 1=Hearts(R), 2=Clubs(B), 3=Diamonds(R) */
    bool face_up;
} sol_card_t;

typedef struct {
    sol_card_t tableau[SOL_COLS][SOL_MAX_PILE];
    int tab_len[SOL_COLS];
    uint8_t foundation[4]; /* top rank 0..13 per suit */
    sol_card_t stock[24];
    int stock_len;
    int waste_idx;
    int cursor_col;
    int selected_col;
    uint32_t moves;
    uint32_t score;
    bool won;
} sol_state_t;

void sol_init(sol_state_t *st, uint32_t seed);
bool sol_is_red(uint8_t suit);
bool sol_can_stack_tableau(sol_card_t moving, sol_card_t dest_top);
bool sol_move_tableau_to_foundation(sol_state_t *st, int col);
bool sol_move_tableau_to_tableau(sol_state_t *st, int from_col, int to_col);
int sol_auto_foundation_sweep(sol_state_t *st);

#endif /* SOLITAIRE_H */
