# Development

## Inspecting the environment

Useful commands on the target system:

```bash
dpkg -L cutefish-toolkit
find /usr/include -iname '*cutefish*'
find /usr/lib -iname '*cutefish*config.cmake'
busctl --user introspect org.cutefish.Shortcuts1 /org/cutefish/Shortcuts1
```

## Build loop

```bash
./build.sh Debug
cd builds/buildN
./cutelight
```

To test installation:

```bash
cd builds/buildN
./install.sh
```

Each phase should be compiled and exercised before the next one is started.
Compiler warnings are treated as actionable; new warnings should be fixed
before continuing.

## Code style

- Modern C++23, RAII, clear names.
- Source comments are written in Japanese only, and only where they add value.
- Documentation and build messages are written in English.
