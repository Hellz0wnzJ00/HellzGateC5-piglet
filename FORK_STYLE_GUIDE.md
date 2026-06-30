# Fork Change Tracking — Style Guide

HellzGate C5 is a fork of [Piglet](https://github.com/Hamspiced/piglet) by
Hamspiced / Midwest Gadgets LLC, used and adapted with permission. This
project tracks fork-specific changes separately from upstream so it's
always clear what's original Piglet work vs. HellzGate-specific work — for
contributors, for future-you, and for anyone diffing against upstream later.

## 1. Inline code tag

Whenever a change diverges from upstream Piglet, mark it with this exact
comment block directly above the changed code:

```cpp
// === HELLZGATE FORK CHANGE — see CHANGELOG.md "HellzGate C5 Fork Changes" vX.X ===
// Was: <one-line description of the original behavior>
// Now: <one-line description of the new behavior>
// ===================================================================
```

- Keep the opening line (`=== HELLZGATE FORK CHANGE`) **exactly** the same
  every time — it's a grep target, not prose.
- Reference the CHANGELOG version where the full explanation lives; keep
  the inline comment itself short (1-2 lines for Was/Now).
- Find every fork change in the codebase at any time with:

  ```bash
  grep -rn "HELLZGATE FORK CHANGE" .
  ```

## 2. CHANGELOG.md

- All fork-specific entries live under the **"HellzGate C5 Fork Changes"**
  heading at the top of `CHANGELOG.md`, in their own `vX.X (date)` entries.
- Upstream Piglet's changelog stays below it, untouched, under "Upstream
  Piglet Changelog" — never edit those entries directly.
- Fork version numbers (`v0.1`, `v0.2`, ...) are independent of Piglet's
  own version numbers. Don't try to keep them in sync.
- Use the same subsection style as upstream for consistency: `#### New
  Features`, `#### Bug Fixes`, `#### Improvements`, etc.

## 3. New files

Any file that's wholly new to HellzGate (not a modified Piglet file) gets
a header comment instead of an inline tag, e.g.:

```cpp
// ============================================================
// HellzGate C5 — <short description of what this file does>
// Forked from Piglet by Hamspiced / Midwest Gadgets LLC
// https://github.com/Hamspiced/piglet — used with permission
// ============================================================
```

## 4. Commit messages

No strict format required, but a consistent prefix makes `git log
--oneline` scannable:

```
fix(mesh): balance channel-split remainder across nodes
feat(theme): add HellzGate OLED splash + pulse animation
fork: <anything fork-structural, e.g. README/CREDITS updates>
```

Mention in the commit body if a change is fork-specific and not intended
to be upstreamed, same as the CHANGELOG entries.

## Why this matters

Piglet is actively maintained upstream. Keeping fork changes clearly
tagged and logged means:
- You can pull `upstream/main` later without losing track of what's yours
- Credit to Hamspiced/Piglet stays unambiguous wherever the code came from
- Anyone else who joins WDGW work on HellzGate can ramp up by reading one
  CHANGELOG section and one grep command, instead of a full diff
