# pshell

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
A minimal, from-scratch shell interpreter for POSIX, written in C without any dependencies — no parser generators, no libreadline, no tutorial followed. Built layer by layer: tokenizer → recursive-descent parser → tree-walking interpreter. the project is meant to be an educational purpose for don't expect too much from it.

```
$ pshell
user@/home> echo hello | tr a-z A-Z
HELLO
user@/home> foo=bar ls > out.txt && echo done
done
```

## Features

### Language

- **Pipelines** — `cmd1 | cmd2 | cmd3`, implemented with `pipe()`/`dup2()`, fork-all-then-wait-all sequencing
- **Conditional chaining** — `cmd1 && cmd2` (short-circuits on failure)
- **Redirection** — `>` (truncate), `>>` (append), `<` (input), in either prefix or suffix position
- **Variable assignment** — `foo=bar`, scoped correctly by context:
  - standalone → persists in the shell's own environment
  - as a command prefix (`foo=bar cmd`) → scoped to that command only
  - as a pipeline element (`foo=bar | cmd`) → persists in the shell's environment 
  - unlike bash, envs can be used everywhere eg.: `foo=bar $foo=echo baz=$foo.txt $bar $foo > $baz` -> `echo bar > bar.txt` 
- **`$VAR` expansion** in arguments
- **Quoting** — `'...'` and `"..."`, folded into a single word token regardless of embedded whitespace
- **Line continuation** — trailing `\` at end of line, in both scripts and the REPL
- **`;` and newline** treated as equivalent statement separators

### Builtins

| Command | Description |
|---|---|
| `cd [dir]` | Change the shell's working directory (runs in-process, not forked) |
| `pwd` | Print the current working directory |
| `exit [code]` | Exit the shell |

### Interfaces

- Interactive REPL
- Script file execution: `pshell script.psh`
- Shebang support: `#!/usr/bin/env pshell` at the top of an executable script
- `pshell --version` / `pshell --help`

## Architecture

```
input (stdin / file)
      │
      ▼
  tokenizer.c   →  Token stream (word, pipe, gt, lt, andd, newline, ...)
      │
      ▼
   parser.c     →  Recursive-descent parser, builds a SyntaxNode AST
      │
      ▼
 interpreter.c  →  Tree-walking executor (fork/exec, pipes, redirects, env)
      │
      ▼
  builtins.c    →  cd / pwd / exit, dispatched before forking
```

The grammar:

```
script      → statement (NEWLINE statement)* NEWLINE?
statement   → pipeline (ANDD pipeline)*
pipeline    → command (PIPE command)*
command     → prefix* word suffix* | prefix+
prefix      → assignment | redirect
suffix      → argument | redirect
assignment  → WORD                 (WORD containing '=', split at parse time)
argument    → WORD
redirect    → (GT | DGT | LT) WORD
```

Buffers (`Buffer`) throughout the codebase are growable, realloc-backed byte arrays — no fixed-size, truncation-prone `char[N]` buffers for anything user-supplied.

## Building

```bash
make debug      # -O0, AddressSanitizer + UBSan, assertions enabled
make release    # -O2, LTO, -DNDEBUG
make test       # runs the test suite (built with debug flags)
```

Build outputs land in `build/debug/`, `build/release/`, and `build/test/` respectively — nothing is written alongside the source tree.

## Installing

```bash
make release
sudo make install                    # installs to /usr/local by default
make install PREFIX=$HOME/.local     # or, per-user, no sudo needed
```

This installs the `pshell` binary and its man page (`man pshell`).

To uninstall:

```bash
sudo make uninstall
```

### From RPM

```bash
sudo dnf install pshell-<version>.rpm
```

See [`pshell.spec`](./pshell.spec) for the packaging definition.

## Development

### LSP / editor support

A `compile_flags.txt` at the project root provides `clangd` (or any Clang-based tooling) with the correct include paths and defines — no extra setup needed beyond an editor with clangd configured.

### Sanitizers

The debug build compiles with `-fsanitize=address,undefined`. Run the debug binary or test suite normally; any memory-safety or undefined-behavior issue will produce an immediate, precise crash report rather than silent corruption.

### Project layout

```
.
├── include/
│   └── pshell.h
├── src/
│   ├── main.c
│   ├── tokenizer.c
│   ├── parser.c
│   ├── interpreter.c
│   ├── builtins.c
│   ├── buffer.c
│   └── runner.c
├── tests/
│   └── *.test.c
├── man/
│   └── pshell.1
├── Makefile
├── pshell.spec
└── compile_flags.txt
```

## Known limitations

- No `||` (or-conditional)
- No control structures (`if`, `while`, `for`)
- No command history / line editing (no arrow-key recall yet)
- No `2>` (stderr redirection)
- Single-quoted content is still subject to `$VAR` expansion — should be fully literal

## License

MIT — see [`LICENSE`](./LICENSE).
