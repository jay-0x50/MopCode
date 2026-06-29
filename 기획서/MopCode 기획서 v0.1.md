# MopCode 기획서 v0.1

## 1. 프로젝트 개요

MopCode는 **Memory Opcode**를 압축한 이름의 게임 개발용 스크립트 언어이다.

MopCode는 게임 오브젝트의 상태 변화, 이벤트 처리, AI 상태머신, 전투 로직 등을 간단한 스크립트로 작성하기 위해 설계한다.

이 프로젝트는 공식 Verse 호환 컴파일러가 아니다.
MopCode는 독립적인 게임 개발용 스크립트 언어이며, C++로 Lexer, Parser, AST, Interpreter 구조를 직접 구현하는 것을 목표로 한다.

초기 버전은 인터프리터 방식으로 실행한다.
이후에는 바이트코드 컴파일러와 VM 구조로 확장할 수 있도록 설계한다.

---

## 2. 기본 설정

```txt
Project Name: MopCode
Meaning: Memory Opcode
Language Type: Game Development Scripting Language
Implementation Language: C++17
File Extension: .mopc
CLI Name: mopc
Execution Type: Interpreter first, Bytecode VM later
```

---

## 3. CLI 명령어 규칙

MopCode의 기본 실행 명령어는 다음과 같다.

```bash
mopc run main
```

이 명령어는 내부적으로 다음 파일을 실행한다.

```txt
main.mopc
```

즉, 사용자가 확장자를 입력하지 않으면 자동으로 `.mopc` 확장자를 붙인다.

---

## 4. CLI 실행 규칙

다음 명령어들을 지원해야 한다.

### 기본 실행

```bash
mopc run
```

동작:

```txt
main.mopc 실행
```

---

### 확장자 없는 파일명 실행

```bash
mopc run main
```

동작:

```txt
main.mopc 실행
```

---

### 확장자를 포함한 파일명 실행

```bash
mopc run main.mopc
```

동작:

```txt
main.mopc 실행
```

---

### 경로 포함 실행

```bash
mopc run scripts/boss
```

동작:

```txt
scripts/boss.mopc 실행
```

---

### 경로와 확장자 포함 실행

```bash
mopc run scripts/boss.mopc
```

동작:

```txt
scripts/boss.mopc 실행
```

---

## 5. CLI 파일 해석 규칙

파일명 처리 규칙은 다음과 같다.

```txt
1. mopc run
   → main.mopc 실행

2. mopc run main
   → main.mopc 실행

3. mopc run main.mopc
   → main.mopc 실행

4. mopc run scripts/boss
   → scripts/boss.mopc 실행

5. mopc run scripts/boss.mopc
   → scripts/boss.mopc 실행
```

파일명에 확장자가 없으면 `.mopc`를 자동으로 붙인다.
파일명에 이미 `.mopc` 확장자가 있으면 그대로 사용한다.

---

## 6. 향후 CLI 명령어 계획

초기 버전에서는 `run`만 구현한다.
이후 다음 명령어를 추가할 수 있도록 구조를 열어둔다.

```bash
mopc run
mopc run main
mopc check main
mopc build main
mopc help
mopc version
```

각 명령어의 역할은 다음과 같다.

```txt
mopc run
- 기본 main.mopc 파일 실행

mopc run main
- main.mopc 파일 실행

mopc check main
- 문법 검사만 수행

mopc build main
- 나중에 바이트코드로 컴파일

mopc help
- 도움말 출력

mopc version
- MopCode 버전 출력
```

v0.1에서는 `mopc run`, `mopc run main`, `mopc run main.mopc`만 구현한다.

---

## 7. v0.1 목표

v0.1의 목표는 매우 작게 잡는다.

복잡한 Lexer, Parser, AST를 처음부터 완성하지 않는다.
우선 `.mopc` 파일을 읽고, `Print("...")` 형태의 코드를 찾아 콘솔에 출력하는 최소 실행기를 만든다.

v0.1에서 실행할 수 있어야 하는 예제:

```mopc
function main(): void {
    Print("Hello MopCode")
}
```

실행 명령어:

```bash
mopc run main
```

출력:

```txt
Hello MopCode
```

---

## 8. v0.1 기능 범위

v0.1에서 구현할 기능은 다음과 같다.

```txt
1. C++17 프로젝트 생성
2. CMake 빌드 설정
3. CLI 인자 처리
4. .mopc 파일 읽기
5. 확장자 자동 보정
6. 기본 파일 main.mopc 처리
7. Print("text") 형태 감지
8. 문자열 콘솔 출력
9. 기본 에러 메시지 출력
10. examples/main.mopc 예제 파일 생성
```

---

## 9. v0.1에서 아직 구현하지 않을 것

다음 기능은 v0.1에서 구현하지 않는다.

```txt
Lexer 전체 구현
Parser 전체 구현
AST 전체 구현
변수
함수 호출
타입 시스템
if / else
loop
entity
component
event
state
bytecode
VM
```

단, 폴더 구조는 나중에 Lexer, Parser, AST, Interpreter를 추가하기 쉽게 설계한다.

---

## 10. 추천 폴더 구조

```txt
MopCode/
├─ README.md
├─ CMakeLists.txt
├─ examples/
│  └─ main.mopc
├─ src/
│  ├─ main.cpp
│  ├─ Cli.h
│  ├─ Cli.cpp
│  ├─ FileResolver.h
│  ├─ FileResolver.cpp
│  ├─ Interpreter.h
│  └─ Interpreter.cpp
└─ tests/
```

v0.1에서는 최소 구조만 사용한다.

나중에 확장할 구조:

```txt
MopCode/
├─ src/
│  ├─ Token.h
│  ├─ Lexer.h
│  ├─ Lexer.cpp
│  ├─ Parser.h
│  ├─ Parser.cpp
│  ├─ AST.h
│  ├─ Interpreter.h
│  ├─ Interpreter.cpp
│  ├─ Value.h
│  ├─ Environment.h
│  └─ ErrorReporter.h
```

---

## 11. 예제 파일

파일 경로:

```txt
examples/main.mopc
```

내용:

```mopc
function main(): void {
    Print("Hello MopCode")
}
```

---

## 12. 에러 처리 규칙

파일을 찾을 수 없으면 다음과 같이 출력한다.

```txt
MopCode Error: file not found: main.mopc
```

지원하지 않는 명령어를 입력하면 다음과 같이 출력한다.

```txt
MopCode Error: unknown command
Usage:
  mopc run
  mopc run <file>
```

Print 문법이 잘못되면 다음과 같이 출력한다.

```txt
MopCode Error: invalid Print syntax
```

---

## 13. README 기본 내용

README.md에는 다음 내용을 포함한다.

````md
# MopCode

MopCode, short for Memory Opcode, is an experimental scripting language designed for game development.

It is built with C++ and focuses on gameplay logic such as entities, events, states, and components.

MopCode uses the `.mopc` file extension.

## Example

```mopc
function main(): void {
    Print("Hello MopCode")
}
````

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

````

---

## 14. Codex 작업 지시

C++17로 MopCode라는 게임 개발용 스크립트 언어 프로젝트를 시작해줘.

기본 설정은 다음과 같아.

```txt
프로젝트명: MopCode
의미: Memory Opcode
확장자: .mopc
CLI 이름: mopc
기본 실행 명령어: mopc run main
빌드 시스템: CMake
구현 언어: C++17
실행 방식: 초기에는 인터프리터
````

v0.1 목표는 다음과 같아.

```txt
function main(): void {
    Print("Hello MopCode")
}
```

위 코드가 들어있는 `.mopc` 파일을 읽고, 콘솔에 `Hello MopCode`를 출력하게 해줘.

CLI 규칙은 다음과 같아.

```txt
mopc run
→ main.mopc 실행

mopc run main
→ main.mopc 실행

mopc run main.mopc
→ main.mopc 실행

mopc run scripts/boss
→ scripts/boss.mopc 실행

mopc run scripts/boss.mopc
→ scripts/boss.mopc 실행
```

파일명에 확장자가 없으면 `.mopc`를 자동으로 붙여줘.
파일명에 `.mopc`가 이미 있으면 그대로 사용해줘.

v0.1에서는 복잡한 Lexer, Parser, AST를 구현하지 말고, 최소 실행기만 만들어줘.

다만 이후 확장을 고려해서 다음 파일 구조로 만들어줘.

```txt
src/
  main.cpp
  Cli.h
  Cli.cpp
  FileResolver.h
  FileResolver.cpp
  Interpreter.h
  Interpreter.cpp

examples/
  main.mopc

tests/
README.md
CMakeLists.txt
```

README에는 MopCode가 Memory Opcode의 줄임말이며, C++ 기반 게임 개발용 실험적 스크립트 언어라는 설명을 적어줘.

또한 MopCode는 공식 Verse 호환 컴파일러가 아니라 독립적인 언어 프로젝트라는 점도 명시해줘.
