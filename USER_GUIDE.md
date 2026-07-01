# HellzGate C5 — Getting Started & Web Dashboard Guide

This guide covers how HellzGate C5 works day-to-day and how to use its web
dashboard. It does not cover hardware assembly or internal board design —
see the project's other documentation for that.

HellzGate C5 is forked from [Piglet](https://github.com/Hamspiced/piglet)
by Hamspiced / Midwest Gadgets LLC, used and adapted with permission.

---

## How It Works

HellzGate C5 is a passive wireless survey system built around two roles:

- **Master** — the unit you interact with. It aggregates data, tags it
  with GPS coordinates, logs it to an SD card, and runs the web
  dashboard covered in this guide.
- **Scan nodes** — additional units that listen for nearby wireless
  networks and forward what they find back to the Master over their own
  private wireless link (not your WiFi, not the internet).

You never need to connect to the scan nodes directly — everything you
need is accessed through the Master.

**No internet connection is required or used.** The Master creates its
own private WiFi network. Connecting your phone or laptop to that network
and opening the dashboard is the entire process — there's no router, no
cloud account, no app store download involved.

---

## First-Time Setup

1. **Power on the Master.** Its small screen will briefly show the
   HellzGate startup animation, then move to a status display.
2. **Connect to its WiFi network.** On your phone or laptop, open WiFi
   settings and look for a network named `HellzGate-WARDRIVE` (this name
   can be changed later — see Configuration below). Connect to it like
   any normal WiFi network.
3. **Open the dashboard.** In any web browser, go to:

   ```
   http://192.168.4.1/
   ```

   This loads the HellzGate C5 dashboard directly from the Master —
   nothing needs to be installed.

That's the whole setup. From here, everything else is done through the
dashboard.

---

## Using the Web Dashboard

### Status

The top section shows live status at a glance:

| Indicator | Meaning |
|---|---|
| **Scan** | Whether active wireless scanning is currently running |
| **SD** | Whether the SD card is present and working |
| **GPS** | Whether a GPS position lock has been acquired |
| **STA** | Whether the Master is also connected to a separate home/known WiFi network (used for automatic upload — see below) |
| **WiGLE** | Whether the device is ready to upload logs to WiGLE.net |

Below the status indicators are live counts of networks found, and
**Start Scan** / **Stop Scan** buttons to control scanning manually.

### Configuration

This section holds all the settings that control how the device behaves.
Key fields explained:

- **WiGLE Basic Token / WDGoWars API Key** — credentials that let the
  device automatically upload your collected data to those services.
  Leave blank to disable automatic uploads to either one.
- **Device Name** — a label to identify this specific unit, useful if
  you run more than one.
- **GPS Baud Rate** — leave at the default (9600) unless instructed
  otherwise for a specific GPS module.
- **Home SSID / Home PSK** — an existing WiFi network the device can
  connect to (in addition to broadcasting its own), typically used so
  it can auto-upload collected data once it's back in range of that
  network.
- **Wardriver SSID / Wardriver PSK** — the name and password of the
  network the Master itself broadcasts (what you connected to in step 2
  above). Change the password from the default before relying on this
  for anything you don't want others accessing.
- **ESP-NOW Mesh Key** — a shared password that encrypts communication
  between the Master and its scan nodes. This must be set identically
  on every node, or they won't be able to talk to the Master at all.
  Change this from the default before real use.
- **Scan Mode** — Aggressive scans faster but uses more power;
  Power Saving trades scan speed for longer battery life.
- **Mesh Mode on Boot** — controls what role this unit takes on
  automatically at startup:
  - *None* — normal standalone operation (default)
  - *Node* — this unit becomes a scan node reporting to a Master
  - *Core* — this unit becomes a Master, ready to receive from nodes
- **Speed Units, Rotate Screen, Max Files to Auto-Upload, Auto-Start
  After Uploads** — cosmetic/behavioral preferences; safe to leave at
  defaults unless you have a specific reason to change them.

Click **Save Config** to apply changes, or **Save & Reboot** if the
change requires a restart to take effect (the dashboard will indicate
this with a countdown).

### SD Card Files

Lists everything logged to the SD card, with options to:
- **Download All** — grabs everything as compressed files in one action
- **Delete All** — clears the card
- Individual files can also be downloaded, deleted, or uploaded to
  WiGLE one at a time from their own row in the list

---

## Coming Soon

These are planned but not yet available in the current firmware:

- **Fan control** — manual/automatic cooling fan control from the
  dashboard
- **Live node list** — a real-time view of connected scan nodes and
  their status, directly in the dashboard
- **Per-node scan type selection** — choosing whether individual scan
  nodes listen for WiFi, Bluetooth, or both

---

## Getting Help

If something isn't behaving as described here, check the Status section
first — most issues (no GPS fix, SD card not detected, etc.) show up
there before anything else. For anything else, reach out through the
WDGW community.
