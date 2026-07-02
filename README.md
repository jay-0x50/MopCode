# MopCode

MopCode, short for Memory Opcode, is an experimental scripting language designed for game development.

It is a small scripting language project for learning how game-oriented languages are built, starting from a simple interpreter written in C++.

MopCode uses the `.mopc` file extension.

## Goal

MopCode is designed for gameplay scripting features such as:

- game object state changes
- event handling
- AI state machines
- combat logic
- entity and component-style gameplay code

The early versions are intentionally small.
MopCode v0.3 can run simple user functions, execute built-in functions, and tokenize `.mopc` source code for debugging.

## Example

```mopc
function main(): void {
    Print("Hello MopCode")
}
```
## Build
If CMake is installed:
```bash
cmake -S . -B build
cmake --build build
```
If you want to test directly with g++:
```bash
g++ -std=c++17 -Isrc src/main.cpp src/Cli.cpp src/FileResolver.cpp src/Interpreter.cpp src/Lexer.cpp -o mopc.exe
```
## Run
From the project root:
```bash
./mopc run examples/main
```
On Windows PowerShell:
```powershell
.\mopc.exe run examples/main
```
You can also run a script from inside the examples folder:
```powershell
cd examples
..\mopc.exe run main
```
This runs:
```txt
main.mopc
```
If no file is provided, MopCode runs main.mopc by default:
```bash
mopc run
```
## CLI Rules
```txt
mopc run
→ main.mopc

mopc run main
→ main.mopc

mopc run main.mopc
→ main.mopc

mopc run scripts/boss
→ scripts/boss.mopc

mopc run scripts/boss.mopc
→ scripts/boss.mopc

mopc tokens
→ tokenizes main.mopc

mopc tokens main
→ tokenizes main.mopc

mopc tokens main.mopc
→ tokenizes main.mopc
```
If the file name has no extension, MopCode automatically adds .mopc.

## Tokens
Use the `tokens` command to print the token list for a `.mopc` file:

```powershell
.\mopc.exe tokens examples/main
```

Example output:

```txt
FUNCTION function
IDENTIFIER main
LEFT_PAREN (
RIGHT_PAREN )
COLON :
VOID void
LEFT_BRACE {
IDENTIFIER Print
LEFT_PAREN (
STRING Hello MopCode
RIGHT_PAREN )
RIGHT_BRACE }
EOF
```

## Built-in Functions

MopCode currently supports a small set of built-in functions:

- `Print(value)`
- `Log(value)`
- `RandomInt(min, max)`
- `RandomFloat(min, max)`
- `DeltaTime()`
- `Spawn("Name")`
- `SetState("StateName")`

Example:

```mopc
function main(): void {
    Print("MopCode Gameplay Test")
    StartBossFight()
    Print(RandomInt(1, 10))
}

function StartBossFight(): void {
    Log("Boss fight started")
    Spawn("Boss")
    SetState("Attack")
}
```

## Current Status
MopCode v0.3 supports:
- basic CLI command handling
- .mopc file loading
- automatic .mopc extension handling
- minimal Print("text") execution
- Lexer-based token scanning
- `mopc tokens` debug output
- no-parameter user function calls
- basic built-in functions for debug and gameplay-style scripting
- basic error messages

Not implemented yet:
- AST
- variables
- function parameters
- types
- control flow
- bytecode
- VM
## Note
MopCode is not a Verse-compatible compiler.
<br><br>
It is an independent language project created to study programming language implementation and game scripting systems.
