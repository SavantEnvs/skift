#!/usr/bin/env bash
#
# mayhem/test.sh — run karm's own unit tests (the library code every harness fuzzes) through
# the karm-test runner build.sh built. Each test id in mayhem/karm-tests.lst is run on its own
# (`-g <id>`); a test passes only when the runner exits 0 AND reports it as PASS, so a neutered
# runner that prints nothing fails every test.
#
# The list is every karm test that runs in a plain container. Left out: karm-icc-srgb/sgray and
# bidiTestFile* (need bundle:// assets / Unicode data files that are not installed on a host
# build) and the few macro-generated ids that do not match their test$() string.
set -uo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${SRC:=/mayhem}"
cd "$SRC"

emit_ctrf() {
  local tool="$1" passed="$2" failed="$3" skipped="${4:-0}" pending="${5:-0}" other="${6:-0}"
  local tests=$(( passed + failed + skipped + pending + other ))
  cat > "${CTRF_REPORT:-$SRC/ctrf-report.json}" <<JSON
{
  "results": {
    "tool": { "name": "$tool" },
    "summary": {
      "tests": $tests,
      "passed": $passed,
      "failed": $failed,
      "pending": $pending,
      "skipped": $skipped,
      "other": $other
    }
  }
}
JSON
  printf 'CTRF {"results":{"tool":{"name":"%s"},"summary":{"tests":%d,"passed":%d,"failed":%d,"pending":%d,"skipped":%d,"other":%d}}}\n' \
    "$tool" "$tests" "$passed" "$failed" "$pending" "$skipped" "$other"
  [ "$failed" -eq 0 ]
}

RUNNER="$SRC/mayhem/out/karm-tests"
LIST="$SRC/mayhem/karm-tests.lst"
if [ ! -x "$RUNNER" ]; then
  echo "test runner $RUNNER missing — run mayhem/build.sh first" >&2
  emit_ctrf karm-test 0 1 0
  exit 1
fi

passed=0 failed=0
while IFS= read -r id; do
  [ -n "$id" ] || continue
  out=$("$RUNNER" -g "$id" </dev/null 2>&1); rc=$?
  if [ "$rc" -eq 0 ] && grep -q 'PASS' <<<"$out" && ! grep -q 'FAIL' <<<"$out"; then
    passed=$((passed + 1))
  else
    failed=$((failed + 1))
    echo "FAIL: $id (exit $rc)"; printf '%s\n' "$out" | tail -n 20
  fi
done < "$LIST"
echo "karm tests: $passed passed, $failed failed"
emit_ctrf karm-test "$passed" "$failed" 0
