# cutefish-search

Spotlight-style desktop search for the Cutefish desktop, built natively with Cutefish Toolkit.

> **Status:** proposal/upstream-preparation branch. This repository started as `cutelight` and is being reshaped to match CutefishOS conventions.

## Features

- Native Wayland search popup built with Cutefish Toolkit
- Installed application search and launch
- Calculator expressions and basic currency conversion
- Web search fallback when no local result matches
- Settings for autostart, keep running, position, appearance, tint, placeholder, and shortcut
- D-Bus control (`Show`, `Hide`, `Toggle`, `ShowSettings`)
- CLI control: `--toggle`, `--show`, `--hide`, `--settings`

## Requirements

- Cutefish OS / Cutefish desktop environment
- `cutefish-toolkit` development files/CMake package
- CMake 3.25+
- Ninja or Make
- C++23 compiler
- `pkg-config`

## Build

```bash
./build.sh Release
```

Each build attempt creates a new `builds/buildN` directory and does not overwrite previous build attempts.

## Install

```bash
cd builds/buildN
./install.sh
```

The installer installs to `~/.local/opt/cutefish-search/`, registers the desktop entry, enables autostart, and can install a user-level shortcut helper service. It may ask for `sudo` to install missing dependencies and the shortcut helper.

## One-command install

```bash
curl -fsSL https://raw.githubusercontent.com/shirushimorii/cutelight/main/install.sh | bash
```

## Usage

- Type to search installed applications.
- Press `Enter` to launch the selected result.
- Use calculator expressions like `2+2` or simple currency conversions.
- Press `Escape` or click outside to hide the window.
- `Ctrl+,` opens settings.

## Global shortcut note

Cutefish currently does not expose a public arbitrary-global-shortcut API for third-party apps. The current implementation includes an optional helper service used by the installer to make the configured shortcut work. For upstream inclusion, this should be reviewed and replaced by a compositor-supported shortcut mechanism when available.

## License

BSD-3-Clause. See [LICENSE](LICENSE).
