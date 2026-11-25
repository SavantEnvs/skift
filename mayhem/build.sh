#!/usr/bin/env bash
#
# mayhem/build.sh — build skift's fuzz harnesses (and the karm unit-test runner) with cutekit.
#
# skift is built by cutekit (C++26 named modules). The harnesses are cutekit `exe` components
# kept in mayhem/fuzz/<name>/; cutekit only discovers components under a project's src/, so
# they are copied to src/mayhem-fuzz/ (untracked, regenerated on every run). The libraries
# they reach (karm-*) come from the externs project.lock pins, which the Dockerfile cloned to
# /opt/toolchains/deps/skift-externs/src; .cutekit/externs points there, so nothing is fetched.
# async=epoll selects karm's epoll scheduler instead of io_uring, which container seccomp
# profiles (docker's default included) refuse.
#
# Instrumentation goes through the compiler commands cutekit reads from $CC/$CXX/$LD (the
# host target's tool definitions), so every object — the karm libraries included — is
# compiled with $SANITIZER_FLAGS $DEBUG_FLAGS; the `fuzz` mixin adds -fsanitize=fuzzer(-no-link).
set -euo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH

: "${SANITIZER_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer}"
: "${DEBUG_FLAGS:=-g -gdwarf-3}"
: "${MAYHEM_JOBS:=$(nproc)}"
: "${SRC:=/mayhem}"
export CUTEKIT_ALLOW_ROOT=1
cd "$SRC"

FUZZERS="image json elf zlib url md template icc idl ttf dtb luna av vte"
EXTERNS=/opt/toolchains/deps/skift-externs/src
OUT="$SRC/mayhem/out"            # gitignored by mayhem/.gitignore
SCRATCH="$SRC/.cutekit/mayhem"   # .cutekit/ is gitignored upstream

# 1) Point cutekit at the in-image externs (never the network).
mkdir -p .cutekit "$SCRATCH" "$OUT"
if [ ! -L .cutekit/externs ]; then rm -rf .cutekit/externs; fi
ln -sfn "$EXTERNS" .cutekit/externs

# 2) Expose the harness components to cutekit.
rm -rf src/mayhem-fuzz
mkdir -p src/mayhem-fuzz
for f in $FUZZERS; do cp -r "mayhem/fuzz/$f" "src/mayhem-fuzz/$f"; done

# 3) Objects linked into every binary: the LSan switch-off and (standalone only) the run-once driver.
clang $SANITIZER_FLAGS $DEBUG_FLAGS -c mayhem/lsan_off.cc -o "$SCRATCH/lsan_off.o"
clang $SANITIZER_FLAGS $DEBUG_FLAGS -c "$STANDALONE_FUZZ_MAIN" -o "$SCRATCH/standalone_main.o"

ck() { # <mode> <component>
  local mode=$1 comp=$2 mixins=fuzz extra="$SCRATCH/lsan_off.o"
  if [ "$mode" = standalone ]; then mixins=o1; extra="$extra $SCRATCH/standalone_main.o"; fi
  CC="clang $SANITIZER_FLAGS $DEBUG_FLAGS" \
  CXX="clang++ $SANITIZER_FLAGS $DEBUG_FLAGS" \
  LD="clang++ -fuse-ld=lld $SANITIZER_FLAGS $DEBUG_FLAGS $extra" \
    cutekit build --target=host-x86_64 --props:async=epoll --mixins="$mixins" "$comp"
}
newest_bin() { ls -t .cutekit/build/host-x86_64-*/"$1"/__bin__/"$1".out | head -1; }

# 4) libFuzzer binaries, then the standalone reproducers (separate cutekit build dir).
for f in $FUZZERS; do
  ck fuzz "fuzz-$f"
  install -m 0755 "$(newest_bin "fuzz-$f")" "$OUT/fuzz-$f"
done
for f in $FUZZERS; do
  ck standalone "fuzz-$f"
  install -m 0755 "$(newest_bin "fuzz-$f")" "$OUT/fuzz-$f-standalone"
done

# 5) The karm unit-test runner (karm-test.main, with every `__tests__` library injected), built
#    with the project's normal flags and no sanitizers; mayhem/test.sh runs it.
CC=clang CXX=clang++ LD="clang++ -fuse-ld=lld" cutekit build --target=host-x86_64 --props:async=epoll karm-test.main
install -m 0755 "$(newest_bin karm-test.main)" "$OUT/karm-tests"

ls -l "$OUT"
