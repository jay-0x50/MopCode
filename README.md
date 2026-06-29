# MopCode

MopCode, short for Memory Opcode, is an experimental scripting language designed for game development.

It is built with C++ and focuses on gameplay logic such as entities, events, states, and components.

MopCode uses the `.mopc` file extension.

## Example

```mopc
function main(): void {
    Print("Hello MopCode")
}
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
mopc run main
```

This runs:

```txt
main.mopc
```

You can also run:

```bash
mopc run main.mopc
```

If no file is provided, MopCode runs `main.mopc` by default.

```bash
mopc run
```

## Note

MopCode is not a Verse-compatible compiler.
It is an independent language project created to study programming language implementation and game scripting systems.
