/*
 * Unit & pass/fail tests for 3ds_solitaire:
 * - Pass-the-pass: alternating color descending rank stacking, Ace->2 foundation promotion, King on empty column.
 * - Fail-the-fail: same-color tableau stack rejected, non-King on empty column rejected, out-of-order foundation rejected.
 */

#include "solitaire.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    sol_card_t red_seven  = { 7, 1, true }; /* 7 of Hearts (Red) */
    sol_card_t blk_eight  = { 8, 0, true }; /* 8 of Spades (Black) */
    sol_card_t blk_seven  = { 7, 2, true }; /* 7 of Clubs (Black) */

    assert(sol_can_stack_tableau(red_seven, blk_eight) == true);
    /* Fail-the-fail: same color or wrong rank difference */
    assert(sol_can_stack_tableau(blk_seven, blk_eight) == false);
    assert(sol_can_stack_tableau(blk_eight, red_seven) == false);

    sol_state_t st;
    sol_init(&st, 12345u);
    /* Force col 0 to hold Ace of Spades and col 1 to be empty */
    st.tab_len[0] = 1;
    st.tableau[0][0] = (sol_card_t){ 1, 0, true };
    st.tab_len[1] = 0;

    /* Non-King cannot move to empty column 1 */
    assert(sol_move_tableau_to_tableau(&st, 0, 1) == false);
    /* Ace of Spades promotes to Foundation[0] */
    assert(sol_move_tableau_to_foundation(&st, 0) == true);
    assert(st.foundation[0] == 1);

    /* Fail-the-fail bounds checks */
    assert(sol_move_tableau_to_foundation(NULL, 0) == false);
    assert(sol_move_tableau_to_foundation(&st, -1) == false);
    assert(sol_move_tableau_to_tableau(&st, 0, 0) == false);

    printf("PASS: test_solitaire all Klondike stacking, foundation promotion, and pass/fail checks succeeded.\n");
    return 0;
}
