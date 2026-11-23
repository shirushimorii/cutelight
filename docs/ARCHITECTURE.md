# Architecture

## Native stack

CutefishSearch uses the installed Cutefish Toolkit directly:

- `cutefish::ui::Application` — event loop, Wayland connection, GPU context, fonts.
- `cutefish::ui::Window` — frameless Wayland surface with compositor glass, shadow, and placement.
- `cutefish::ui::View` and layouts — retained UI tree in logical pixels.
- `cutefish::ui::TextField` — Unicode input, caret, selection, paste, and IME.
- `cutefish::ui::Animation` — frame-scheduled open, close, and expansion transitions.
- `cutefish::base::ApplicationRegistry` — cached XDG desktop-entry metadata.
- `cutefish::base::mimeapps::openUri` — opens the Google fallback in the
  user's default browser.
- `cutefish::ui::StatusItem` — native system-tray item.
- `cutefish::base::Settings` — user configuration in INI format.

Qt, KDE Frameworks, GTK, Electron, and web technologies are intentionally not
used.

## Layers

1. **Application** owns the native application, both top-level windows, the
   settings model, the D-Bus control service, the menu bar, and the tray icon.
2. **MainWindow** owns material, placement, responsive sizing, window-level
   shortcuts, the open/close/expansion animations, and the focus-loss path that
  hides the search bar when the user clicks outside it.
3. **CutefishSearchView** owns the stationary 76 px search row, its separator, and a
   real results viewport. It contains no placeholder cards or fake results.
4. **SearchBox** arranges the search icon, native text field, and clear button
   on one horizontal axis using a `Row` layout. Escape is intercepted by the
   field and forwarded as a hide request.
5. **SearchController** ranks installed desktop entries and adds calculator,
   currency-conversion, and Google-search results. Activating a result launches
   its application, copies a computed value, or opens the default browser.
6. **ResultsView** renders compact single-line rows with the name on the left
   and its description on the right, with pointer and keyboard selection.
7. **SettingsView / SettingsWindow** expose persisted preferences with native
   switches, combo boxes, text fields, a shortcut capture button, and a tint
   slider. A native `WindowHeader` makes the panel draggable. Edits are staged
   until **Save and Close**, which persists them, updates autostart, and applies
   the hint text and appearance to the live search row. **Close** discards
   pending edits.
8. **ControlService** exports the D-Bus methods used by manual launchers.
   `Application::forwardCommand()` lets `cutefish-search --toggle`, `--show`,
   `--hide`, and `--settings` reuse an already-running instance.
9. **MainMenu / TrayIcon** provide native menu-bar and tray actions. The menu
   bar is installed while the search field is focused; the tray icon stays
   present for the lifetime of the background process.

## Design tokens

All visual dimensions, timings, and fallback colours live in
`src/ui/DesignTokens.h`. UI components do not carry unrelated magic numbers.

## Build attempts

`build.sh` scans `builds/` for existing `buildN` directories and creates the
next one. It never reuses or deletes a previous attempt. Logs, assets, packaging,
and the generated installer live beside the executable in that directory.
