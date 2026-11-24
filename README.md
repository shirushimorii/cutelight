# CuteLight (Cutefish OS only)

CuteLight is a native Spotlight-style desktop search application for Cutefish OS.  
It is built directly on **Cutefish Toolkit** and runs as a regular Wayland client. It does not replace or patch the compositor, launcher, dock, or any other system component.

> **Important:** This project targets **Cutefish OS only**. Other Linux desktop environments and operating systems are out of scope.

[日本語 README](README.ja.md)

## Features

- macOS Sequoia / Tahoe inspired glass search window
- Frameless floating Wayland window
- `760×76` collapsed and `760×430` expanded (logical pixels), clamped to the screen
- Positioned 37% down from the top of the work area; the search row stays fixed
- Compositor glass/blur with a light tint and 20 px corners
- Search row: search icon / native text field / clear button shown only while typing
- Native Unicode input (caret, selection, paste, IME)
- Quiet open/close and expand animations
- Follows the light/dark theme
- Hides on `Escape` or an outside click when background mode is enabled
- Installed application search
- Calculator expressions, basic currency conversion, and Google search fallback
- Settings UI (autostart, keep running, shortcut, position, appearance, tint, placeholder)
- Changes are staged until **Save and Close**; **Close** discards them
- System tray and menu bar integration
- D-Bus control: `Show` / `Hide` / `Toggle` / `ShowSettings`

## Requirements

- **Cutefish OS (Debian 13 / Trixie based) only**
- `cutefish-toolkit` development headers and CMake package
- CMake 3.25+
- Ninja (recommended) or Make
- A C++23 compiler
- `git`, `pkg-config`

## One-command install

```bash
curl -fsSL https://raw.githubusercontent.com/shirushimorii/cutelight/main/install.sh | bash
```

The installer may ask for your `sudo` password to set up the global shortcut helper service.

### What gets installed

- Executable: `~/.local/opt/cutelight/cutelight`
- Desktop entry: `~/.local/share/applications/com.mori.cutelight.desktop`
- Autostart entry: `~/.config/autostart/com.mori.cutelight.desktop`
- Configuration: `~/.config/cutelight/cutelight.conf`
- Shortcut helper: `/usr/local/lib/cutelight/shortcut-helper.py`
- systemd service: `cutelight-shortcut.service`

The installer removes leftovers from the old Spotlight prototype and installs the new build. Existing CuteLight settings are preserved where possible.

## Manual build and install

```bash
git clone https://github.com/shirushimorii/cutelight.git
cd cutelight
./build.sh Release
cd builds/buildN   # N is the latest build number
./install.sh
```

`./build.sh [Debug|Release]` always creates a new `builds/buildN` directory and never overwrites or deletes an earlier attempt.

## Usage

### Basics

- Typing in the search bar expands the window downwards.
- Results are compact single rows. Activate them with the mouse or the arrow keys and `Enter`.
- Installed applications launch directly.
- Calculator and currency results can be copied to the clipboard.
- With no local match, a Google search opens in your default browser.
- Clearing the input collapses the window again.
- Press `Escape` or click outside the search window to hide it.
- When "keep running in the background" is enabled, the app stays resident after hiding.

### Opening settings

- `Ctrl+,` inside the window
- The **Settings** action of the desktop entry
- The tray icon menu
- `cutelight --settings`
- D-Bus: `ShowSettings`

The settings window can be dragged by its title bar. Use **Save and Close** to apply changes or **Close** to discard them.

### Global shortcut

- The default shortcut is **Super+Space**.
- Enable "Open with shortcut" in settings and click the shortcut button to record a new one. Press a modifier first, then the key (for example `Ctrl`, then `Space`). A modifier alone such as `Super` is also supported. `Escape` cancels recording.
- The window opens with the search field focused, including when it is opened from the shortcut.
- Cutefish does not expose a public API for third-party global shortcuts, so CuteLight uses a small helper service (`cutelight-shortcut.service`) to detect the configured key combination.
- The helper reads `~/.config/cutelight/cutelight.conf` and runs `cutelight --toggle` when the shortcut is pressed.

### Command line

```bash
cutelight --toggle     # Toggle visibility (reuses a running instance)
cutelight --show       # Show
cutelight --hide       # Hide
cutelight --settings   # Show settings
```

## Project layout

```text
src/application/  application lifetime and coordination
src/window/       Wayland window, placement, animation, shortcuts
src/ui/           design tokens, search row, results, settings UI
src/search/       app search, calculator, currency conversion, web fallback
src/integration/  D-Bus, menu bar, system tray
src/settings/     settings persistence, autostart
packaging/        desktop entry and installer template
docs/             design and integration notes
builds/           numbered local build attempts (not tracked by Git)
```

## License

See [LICENSE](LICENSE).
