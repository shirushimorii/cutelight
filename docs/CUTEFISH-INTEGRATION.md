# Cutefish integration notes

## What was verified

The live target session exposes:

- `org.cutefish.Shortcuts1` on `/org/cutefish/Shortcuts1`
  - methods: `accelerators`, `addAccelerator`, `removeAccelerator`,
    `replaceAccelerator`, `restoreDefault`, `restoreAllDefaults`, `describeKey`
  - the accelerators are a fixed list of compositor actions such as
    `toggle-app-list`, `toggle-overview`, `lock-screen`, and media controls.
- `org.cutefish.AppRuntime` on `/AppRuntime` for launching applications and commands.
- `org.cutefish.Launchpad` on `/Launchpad` with `show`, `hide`, and `toggle`.
- `org.cutefish.HotCorners1` on `/org/cutefish/HotCorners1`.
- `org.cutefish.WindowDebug1` on `/org/cutefish/WindowDebug1` for live window
  geometry and activation inspection.
- `com.canonical.AppMenu.Registrar` for the native menu bar installed while the
  search field is focused.
- `org.kde.StatusNotifierWatcher` on `/StatusNotifierWatcher` through the
  toolkit's `StatusItem`, used for the persistent system-tray icon.

## Global shortcut limitation

`org.cutefish.Shortcuts1` only modifies accelerators for actions that the
compositor itself owns. There is no verified API to register a new action such
as “launch an arbitrary executable” or to receive `Super+Space` in a third-party
Wayland client. Ordinary Wayland keyboard events are delivered only to the
focused window, so an application cannot listen for a global hotkey itself.

Therefore CuteLight does not claim to provide `Super+Space` automatically.

## Supported options

1. Launch CuteLight from the desktop entry, the dock, or the terminal.
2. Assign an existing unused compositor accelerator to an action that can open
   the desktop search experience, if the user wants to reconfigure the desktop.
   This changes a system action and is intentionally left to the user.
3. CuteLight exports `com.mori.CuteLight` on `/org/mori/CuteLight` with
   `Show`, `Hide`, `Toggle`, and `ShowSettings`. A small root helper service
   (`cutelight-shortcut.service`) reads the saved shortcut from
   `~/.config/cutelight/cutelight.conf` and runs `cutelight --toggle` when the
   key is pressed. `cutelight --toggle` starts the app when needed, otherwise it
   calls the running instance's `Toggle` method. `--show`, `--hide`, and
   `--settings` work the same way. The installer adds a **Toggle search** desktop
   action for launchers that prefer desktop actions. This keeps the original
   Cutefish launcher untouched.

The installer prints this limitation so that the desktop configuration is never
changed silently.

## Search and background behaviour

Installed applications are discovered from XDG desktop entries through
`cutefish::base::ApplicationRegistry`. Arithmetic expressions and a small fixed
set of currency rates are evaluated locally; their result is copied with the
toolkit clipboard when activated. If no app or computed result matches, the
query is offered as a Google search and opened with
`cutefish::base::mimeapps::openUri`, which honours the user's default browser.

The menu bar is attached through `Application::setMainMenu()` only while the
search field has focus, and detached when focus leaves it. The tray icon remains
available while the process runs so `Escape` can close the search window without
quitting the application. The dock menu uses the same `MainMenu` instance at all
times.
