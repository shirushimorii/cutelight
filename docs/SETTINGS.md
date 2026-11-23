# Settings

CutefishSearch stores user preferences in `~/.config/cutefish-search/cutefish-search.conf` unless
`XDG_CONFIG_HOME` is set. The settings window can be opened with `Ctrl+,` while
the search window is focused, from the desktop entry's Settings action, with
`cutefish-search --settings`, or through D-Bus.

Edits are staged in the settings window. Drag the title bar at the top to move
the window. Press **Save and Close** to write the configuration, update the
autostart entry, and apply the changes to the running search window. **Close**
dismisses the window without saving pending edits.

The search box starts with the hint text **Cutelight Search**.

## Available preferences

- **Open CutefishSearch when I sign in** — writes or removes the user-level autostart
  entry.
- **Keep CutefishSearch running in the background** — when disabled, hiding the
  search window exits the process.
- **Open with a shortcut** and **Shortcut** — the key combination is captured
  from the keyboard and persisted. When enabled, CutefishSearch's small background
  helper listens for that shortcut and runs `cutefish-search --toggle`, so it works
  even when another app is focused.
- **Where it appears** — `Top`, `Top middle`, `Centre`, `Bottom middle`, or
  `Bottom`. The search row stays horizontally centred and the window grows
  downward.
- **Glass style** — `Tinted` uses a stronger tint and stronger refraction;
  `Glassy` keeps the near-clear macOS-style glass.
- **Tint strength** — controls tint strength for both material modes.
- **Text when the box is empty** — the hint shown in an empty search box. It is
  applied to the live search row when you press **Save and Close**.

## Installer behaviour

Each new installer removes the old Spotlight prototype files and autostart
entry, installs the new binary, writes a fresh CutefishSearch autostart entry pointing
at that binary, sets `autostart=true` in the existing CutefishSearch configuration,
and preserves all other settings. Numbered builds in the source tree are never
deleted.
