#include "pk_platform.h"
#include "solitaire.h"

#include <stdio.h>
#include <string.h>

static const char *const k_ranks[14] = {
    "-", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};
static const char *const k_suits[4] = { "S", "H", "C", "D" };

typedef struct {
    char sd_root[512];
    sol_state_t st;
    uint32_t seed;
    char status_msg[128];
} sol_app_t;

static void app_init(sol_app_t *app, const char *sd_root) {
    memset(app, 0, sizeof(*app));
    snprintf(app->sd_root, sizeof(app->sd_root), "%s",
             sd_root != NULL ? sd_root : pk_platform_default_sd_root());
    app->seed = 0x1337u;
    sol_init(&app->st, app->seed);
    sol_auto_foundation_sweep(&app->st);
    snprintf(app->status_msg, sizeof(app->status_msg),
             "KLONDIKE READY | A:SELECT/MOVE X:AUTO-FOUNDATION Y:NEW DEAL");
    pk_log_event(app->sd_root, "solitaire", "INFO", app->status_msg);
}

static void app_step(sol_app_t *app, const pk_input_t *in) {
    if (in->down & PK_BTN_DLEFT)  app->st.cursor_col = (app->st.cursor_col + SOL_COLS - 1) % SOL_COLS;
    if (in->down & PK_BTN_DRIGHT) app->st.cursor_col = (app->st.cursor_col + 1) % SOL_COLS;

    if (in->touch_just_pressed && in->touch_y >= 36 && in->touch_y < 210) {
        int col = (in->touch_x - 8) / 44;
        if (col >= 0 && col < SOL_COLS) {
            app->st.cursor_col = col;
        }
    }

    if (in->down & PK_BTN_A || in->touch_just_pressed) {
        int col = app->st.cursor_col;
        if (sol_move_tableau_to_foundation(&app->st, col)) {
            snprintf(app->status_msg, sizeof(app->status_msg),
                     "MOVED COL %d TO FOUNDATION! SCORE:%u", col + 1, (unsigned)app->st.score);
        } else if (app->st.selected_col < 0) {
            app->st.selected_col = col;
            snprintf(app->status_msg, sizeof(app->status_msg), "SELECTED COL %d", col + 1);
        } else {
            if (sol_move_tableau_to_tableau(&app->st, app->st.selected_col, col)) {
                snprintf(app->status_msg, sizeof(app->status_msg),
                         "MOVED COL %d -> COL %d", app->st.selected_col + 1, col + 1);
            } else {
                snprintf(app->status_msg, sizeof(app->status_msg), "ILLEGAL CARD STACK");
            }
            app->st.selected_col = -1;
        }
    }
    if (in->down & PK_BTN_X) {
        int n = sol_auto_foundation_sweep(&app->st);
        snprintf(app->status_msg, sizeof(app->status_msg), "AUTO-SWEPT %d CARD(S) TO FOUNDATION", n);
    }
    if (in->down & PK_BTN_Y) {
        app->seed += 7u;
        sol_init(&app->st, app->seed);
        snprintf(app->status_msg, sizeof(app->status_msg), "DEALT NEW HAND (SEED %u)", (unsigned)app->seed);
        pk_log_event(app->sd_root, "solitaire", "INFO", app->status_msg);
    }
}

static void app_render(const sol_app_t *app) {
    pk_render_begin_frame();
    pk_render_begin_target(PK_TARGET_TOP_LEFT, PK_RGBA8(14, 48, 28, 255));
    pk_draw_rect(0, 0, 400, 26, PK_RGBA8(20, 72, 42, 255));
    pk_draw_text(8, 7, 0.48f, PK_RGBA8(255, 230, 105, 255),
                 "SOLITAIRE 3DS - KLONDIKE & AUTO-SOLVER");

    char line[128];
    snprintf(line, sizeof(line), "SCORE: %u   |   MOVES: %u   |   DEAL SEED: #%u",
             (unsigned)app->st.score, (unsigned)app->st.moves, (unsigned)app->seed);
    pk_draw_text(10, 34, 0.45f, PK_RGBA8(215, 250, 225, 255), line);

    /* 4 Foundation Piles on Top Screen */
    for (int s = 0; s < 4; s++) {
        float fx = 40.0f + (float)s * 84.0f;
        pk_draw_rect(fx, 64.0f, 68.0f, 92.0f, PK_RGBA8(238, 242, 248, 255));
        pk_draw_rect_outline(fx, 64.0f, 68.0f, 92.0f, 2.0f, PK_RGBA8(220, 180, 65, 255));
        snprintf(line, sizeof(line), "%s:%s", k_suits[s], k_ranks[app->st.foundation[s]]);
        pk_draw_text(fx + 14.0f, 102.0f, 0.52f,
                     sol_is_red((uint8_t)s) ? PK_RGBA8(215, 40, 50, 255) : PK_RGBA8(25, 30, 45, 255),
                     line);
    }

    pk_draw_rect(0, 216, 400, 24, PK_RGBA8(18, 60, 34, 255));
    pk_draw_text(8, 222, 0.44f, PK_RGBA8(130, 255, 175, 255), app->status_msg);

    /* Bottom Touch Screen (320x240): 7 Tableau Columns */
    pk_render_begin_target(PK_TARGET_BOTTOM, PK_RGBA8(12, 40, 24, 255));
    for (int c = 0; c < SOL_COLS; c++) {
        float cx = 8.0f + (float)c * 44.0f;
        bool cur = (c == app->st.cursor_col);
        bool sel = (c == app->st.selected_col);
        pk_draw_rect_outline(cx, 12.0f, 40.0f, 190.0f, 1.0f,
                             sel ? PK_RGBA8(255, 225, 75, 255)
                                 : (cur ? PK_RGBA8(110, 240, 255, 255) : PK_RGBA8(30, 85, 50, 255)));

        for (int r = 0; r < app->st.tab_len[c] && r < 8; r++) {
            sol_card_t cd = app->st.tableau[c][r];
            float cy = 16.0f + (float)r * 20.0f;
            uint32_t card_bg = cd.face_up ? PK_RGBA8(242, 245, 250, 255) : PK_RGBA8(45, 95, 175, 255);
            pk_draw_rect(cx + 2.0f, cy, 36.0f, 22.0f, card_bg);
            if (cd.face_up) {
                snprintf(line, sizeof(line), "%s%s", k_ranks[cd.rank], k_suits[cd.suit]);
                pk_draw_text(cx + 6.0f, cy + 5.0f, 0.42f,
                             sol_is_red(cd.suit) ? PK_RGBA8(215, 38, 48, 255) : PK_RGBA8(22, 28, 40, 255),
                             line);
            }
        }
    }
    pk_draw_text(8, 218, 0.42f, PK_RGBA8(205, 240, 215, 255),
                 "TAP COL OR D-PAD+A | X:AUTO-SOLVE | Y:NEW DEAL");
    pk_render_end_frame();
}

int main(int argc, char **argv) {
    const char *sd_root = NULL;
    bool one_shot = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--sd-root") == 0 && i + 1 < argc) sd_root = argv[++i];
        else if (strcmp(argv[i], "--once") == 0) one_shot = true;
    }
    if (!pk_platform_init()) return 1;
    sol_app_t app;
    app_init(&app, sd_root);
    while (pk_platform_main_loop()) {
        pk_input_t in;
        pk_platform_poll_input(&in);
        if (in.down & PK_BTN_START) break;
        app_step(&app, &in);
        app_render(&app);
        if (one_shot) break;
    }
    pk_platform_shutdown();
    return 0;
}
