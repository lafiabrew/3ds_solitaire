# Solitaire 3DS (`3ds_solitaire`)

<p align="center">
  <img src="docs/icon.png" width="72" height="72" alt="3ds_solitaire icon" />
  &nbsp;&nbsp;&nbsp;
  <img src="docs/banner.png" width="256" height="128" alt="3ds_solitaire 3DS banner" />
</p>

<p align="center">
  <img src="docs/screenshot.png" width="360" alt="3ds_solitaire dual-screen 3DS preview" />
</p>

> **Klondike & Auto-Solver (`CTR-P-SOLI`, UniqueId `0x0D3EB`)** — Part of the [**lafiabrew**](https://github.com/orgs/lafiabrew/repositories) Nintendo 3DS Homebrew Suite.

Tactile Klondike card solitaire with alternating-color tableau validation, 4 foundation piles, and 1-button auto-foundation solver.

---

## Architecture & Data Flow

```mermaid
flowchart LR
    DEAL[Seeded 52-Card Fisher-Yates Deal] --> TAB[Bottom 7 Tableau Columns]
    TAB -->|A / Touch| FND[Top 4 Suit Foundations]
    TAB -->|X Auto-Sweep| FND
```

---

## Controls & Touch Ergonomics

| Button / Input | Action |
|---|---|
| `D-Pad Left / Right` (or Touch Column) | Select tableau column `1..7` |
| `A` | Promote top card to Foundation or move card stack between columns |
| `X` | Auto-sweep all eligible cards to Foundations |
| `Y` | Deal new seeded hand |

---

## Build, Verify & Install

```bash
make test
make 3ds && make verify-3ds
make qr
```
