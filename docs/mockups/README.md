# Mockups

Design references for the HellzGate C5 UI. These are **mockups**, not the shipped
UI — they capture layout and content decisions.

- `oled_glance.svg` — the fixed always-on OLED glance screen: cluster temp + fan
  icon (filled = running; would blink on a suspected fan fault), WiFi/BLE unique
  counts, total hits, GPS/SD status, and nine node dots (filled = alive, hollow =
  dropped, position = slot ESP-1..9). Open in any browser.
- `webui_console.html` — the web console: live count cards, cluster temp/fan,
  and the scan-node table with friendly names (MAC secondary), per-node band /
  last-seen / temperature, and a scan-mode selector. The real UI will be one
  responsive page served from the master. Open in any browser.
