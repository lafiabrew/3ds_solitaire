#include "solitaire.h"

#include <string.h>

bool sol_is_red(uint8_t suit) {
    return (suit == 1u || suit == 3u);
}

bool sol_can_stack_tableau(sol_card_t moving, sol_card_t dest_top) {
    if (!moving.face_up || !dest_top.face_up) return false;
    if (moving.rank < 1 || moving.rank > 13 || dest_top.rank < 1 || dest_top.rank > 13) return false;
    return (sol_is_red(moving.suit) != sol_is_red(dest_top.suit)) &&
           ((int)moving.rank + 1 == (int)dest_top.rank);
}

void sol_init(sol_state_t *st, uint32_t seed) {
    if (st == NULL) return;
    memset(st, 0, sizeof(*st));
    st->selected_col = -1;

    sol_card_t deck[52];
    int idx = 0;
    for (uint8_t s = 0; s < 4; s++) {
        for (uint8_t r = 1; r <= 13; r++) {
            deck[idx].suit = s;
            deck[idx].rank = r;
            deck[idx].face_up = false;
            idx++;
        }
    }

    uint32_t rng = seed ? seed : 0x20263D55u;
    for (int i = 51; i > 0; i--) {
        rng = rng * 1664525u + 1013904223u;
        int j = (int)((rng >> 16) % (uint32_t)(i + 1));
        sol_card_t tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }

    int di = 0;
    for (int c = 0; c < SOL_COLS; c++) {
        for (int r = 0; r <= c; r++) {
            sol_card_t card = deck[di++];
            card.face_up = (r == c);
            st->tableau[c][r] = card;
        }
        st->tab_len[c] = c + 1;
    }
    st->stock_len = 0;
    while (di < 52 && st->stock_len < 24) {
        sol_card_t card = deck[di++];
        card.face_up = true;
        st->stock[st->stock_len++] = card;
    }
}

bool sol_move_tableau_to_foundation(sol_state_t *st, int col) {
    if (st == NULL || col < 0 || col >= SOL_COLS || st->tab_len[col] <= 0) {
        return false;
    }
    sol_card_t top = st->tableau[col][st->tab_len[col] - 1];
    if (!top.face_up || top.suit >= 4) return false;
    if (top.rank != (uint8_t)(st->foundation[top.suit] + 1u)) {
        return false;
    }
    st->foundation[top.suit] = top.rank;
    st->tab_len[col]--;
    if (st->tab_len[col] > 0) {
        st->tableau[col][st->tab_len[col] - 1].face_up = true;
    }
    st->moves++;
    st->score += 15u;
    st->won = (st->foundation[0] == 13 && st->foundation[1] == 13 &&
               st->foundation[2] == 13 && st->foundation[3] == 13);
    return true;
}

bool sol_move_tableau_to_tableau(sol_state_t *st, int from_col, int to_col) {
    if (st == NULL || from_col < 0 || from_col >= SOL_COLS ||
        to_col < 0 || to_col >= SOL_COLS || from_col == to_col) {
        return false;
    }
    if (st->tab_len[from_col] <= 0 || st->tab_len[to_col] >= SOL_MAX_PILE) {
        return false;
    }
    sol_card_t moving = st->tableau[from_col][st->tab_len[from_col] - 1];
    if (st->tab_len[to_col] == 0) {
        if (moving.rank != 13) return false; /* Only Kings on empty column */
    } else {
        sol_card_t dest = st->tableau[to_col][st->tab_len[to_col] - 1];
        if (!sol_can_stack_tableau(moving, dest)) return false;
    }
    st->tableau[to_col][st->tab_len[to_col]++] = moving;
    st->tab_len[from_col]--;
    if (st->tab_len[from_col] > 0) {
        st->tableau[from_col][st->tab_len[from_col] - 1].face_up = true;
    }
    st->moves++;
    st->score += 5u;
    return true;
}

int sol_auto_foundation_sweep(sol_state_t *st) {
    if (st == NULL) return 0;
    int moved = 0;
    bool progress = true;
    while (progress) {
        progress = false;
        for (int c = 0; c < SOL_COLS; c++) {
            if (sol_move_tableau_to_foundation(st, c)) {
                moved++;
                progress = true;
            }
        }
    }
    return moved;
}
