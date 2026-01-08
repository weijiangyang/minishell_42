\
#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CASES_DIR="$ROOT_DIR/cases"
LOG_DIR="$ROOT_DIR/logs"

MINI="${MINI:-./minishell}"
BASH_SHELL="${BASH_SHELL:-bash}"
TIMEOUT_SEC="${TIMEOUT_SEC:-2}"

mkdir -p "$LOG_DIR"

have_cmd() { command -v "$1" >/dev/null 2>&1; }

run_with_timeout() {
  local out="$1"; shift
  local err="$1"; shift
  if have_cmd timeout; then
    timeout --preserve-status "${TIMEOUT_SEC}s" "$@" >"$out" 2>"$err"
    return $?
  fi
  "$@" >"$out" 2>"$err"
  return $?
}

normalize_stderr_prefix() {
  sed -E 's/^(bash|minishell|Minishell|MINISHELL):[[:space:]]*/SHELL: /'
}

trim() { sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//'; }

run_one_case() {
  local module="$1"
  local case_id="$2"
  local case_desc="$3"
  local input_text="$4"

  local tmp
  tmp="$(mktemp -d "${LOG_DIR}/tmp.${module}.${case_id}.XXXXXX")" || return 1

  local in_file="$tmp/in.txt"
  local bash_out="$tmp/bash.out"
  local bash_err="$tmp/bash.err"
  local mini_out="$tmp/mini.out"
  local mini_err="$tmp/mini.err"

  printf "%s" "$input_text" > "$in_file"

  if ! printf "%s" "$input_text" | grep -Eq '^[[:space:]]*exit([[:space:]]|$)'; then
    printf "\nexit\n" >> "$in_file"
  fi

  run_with_timeout "$bash_out" "$bash_err" "$BASH_SHELL" --noprofile --norc <"$in_file"
  local bash_ec=$?

  run_with_timeout "$mini_out" "$mini_err" "$MINI" <"$in_file"
  local mini_ec=$?

  normalize_stderr_prefix < "$bash_err" > "$tmp/bash.err.norm"
  normalize_stderr_prefix < "$mini_err" > "$tmp/mini.err.norm"

  diff -u "$bash_out" "$mini_out" > "$tmp/diff_out" 2>/dev/null
  local out_same=$?
  diff -u "$tmp/bash.err.norm" "$tmp/mini.err.norm" > "$tmp/diff_err" 2>/dev/null
  local err_same=$?

  local ec_same=1
  if [ "$bash_ec" -eq "$mini_ec" ]; then ec_same=0; else ec_same=1; fi

  local ok=1
  if [ "$out_same" -eq 0 ] && [ "$err_same" -eq 0 ] && [ "$ec_same" -eq 0 ]; then
    ok=0
  fi

  if [ "$ok" -eq 0 ]; then
    printf "[PASS] %s | %s\n" "$case_id" "$case_desc"
  else
    printf "[FAIL] %s | %s\n" "$case_id" "$case_desc"
    printf "  exit_code: bash=%s minishell=%s\n" "$bash_ec" "$mini_ec"
    if [ -s "$tmp/diff_out" ]; then
      printf "  --- stdout diff (bash vs minishell) ---\n"
      sed 's/^/  /' "$tmp/diff_out" | head -n 200
    fi
    if [ -s "$tmp/diff_err" ]; then
      printf "  --- stderr diff (bash vs minishell, prefix normalized) ---\n"
      sed 's/^/  /' "$tmp/diff_err" | head -n 200
    fi
  fi

  if [ "$ok" -ne 0 ]; then
    local case_dir="$LOG_DIR/${module}_${case_id}"
    mkdir -p "$case_dir"
    cp "$in_file" "$case_dir/in.txt"
    cp "$bash_out" "$case_dir/bash.out"
    cp "$bash_err" "$case_dir/bash.err"
    cp "$mini_out" "$case_dir/mini.out"
    cp "$mini_err" "$case_dir/mini.err"
    cp "$tmp/diff_out" "$case_dir/diff_out.txt" 2>/dev/null || true
    cp "$tmp/diff_err" "$case_dir/diff_err.txt" 2>/dev/null || true
  fi

  rm -rf "$tmp"
  return "$ok"
}

run_case_file() {
  local case_file="$1"
  local module_name
  module_name="$(basename "$case_file" .cases)"

  echo ""
  echo "== Module: $(basename "$case_file") =="

  local total=0
  local passed=0

  local t_id=""
  local t_desc=""
  local input_buf=""

  flush_case() {
    if [ -z "$t_id" ]; then return 0; fi
    total=$((total+1))
    if run_one_case "$module_name" "$t_id" "$t_desc" "$input_buf"; then
      passed=$((passed+1))
    fi
    t_id=""
    t_desc=""
    input_buf=""
  }

  while IFS= read -r line || [ -n "$line" ]; do
    # Fix Windows CRLF: remove trailing \r if present
    line="${line%$'\r'}"

    # IMPORTANT: parse "## ..." headers BEFORE skipping comments
    if [[ "$line" =~ ^##[[:space:]]*(.*)$ ]]; then
      local rest="${BASH_REMATCH[1]}"
      if printf "%s" "$rest" | grep -qE '[|｜]'; then
        local left right
        left="$(printf "%s" "$rest" | sed -E 's/[|｜].*$//' | trim)"
        right="$(printf "%s" "$rest" | sed -E 's/^.*[|｜][[:space:]]*//' | trim)"
        flush_case
        t_id="$left"
        t_desc="$right"
      else
        flush_case
        t_id="$(printf "%s" "$rest" | trim)"
        t_desc=""
      fi
      continue
    fi

    # Skip normal comment lines that start with single '#'
    if [[ "$line" =~ ^[[:space:]]*#[^#] ]]; then
      continue
    fi

    # ignore empty lines between sections
    if [[ "$line" =~ ^[[:space:]]*$ ]]; then
      continue
    fi

    # placeholders
    if [ "$line" = "__BLANK__" ]; then
      input_buf="${input_buf}"$'\n'
    elif [ "$line" = "__SPACES__" ]; then
      input_buf="${input_buf}    "$'\n'
    elif [ "$line" = "__TABS__" ]; then
      input_buf="${input_buf}"$'\t\t\n'
    else
      input_buf="${input_buf}${line}"$'\n'
    fi
  done < "$case_file"

  flush_case

  if [ "$total" -eq 0 ]; then
    echo "!! ERROR: parsed 0 tests from $(basename "$case_file")"
    echo "   Expected headers like: ## S01 | description"
    echo "== Result: $(basename "$case_file") : 0/0 passed =="
    return 1
  fi

  echo "== Result: $(basename "$case_file") : ${passed}/${total} passed =="
  if [ "$passed" -eq "$total" ]; then return 0; fi
  return 1
}

main() {
  local files=()
  if [ "$#" -eq 0 ]; then
    while IFS= read -r f; do files+=("$f"); done < <(find "$CASES_DIR" -maxdepth 1 -type f -name "*.cases" | sort)
  else
    files=("$@")
  fi

  if [ ! -x "$MINI" ]; then
    echo "ERROR: minishell not found or not executable: $MINI"
    echo "Tip: MINI=./minishell bash $0"
    exit 2
  fi

  local any_fail=0
  for f in "${files[@]}"; do
    if [ ! -f "$f" ]; then
      echo "ERROR: case file not found: $f"
      exit 2
    fi
    run_case_file "$f" || any_fail=1
  done

  echo ""
  if [ "$any_fail" -eq 0 ]; then
    echo "ALL PASSED ✅"
  else
    echo "SOME FAILED ❌  (see logs in $LOG_DIR)"
  fi
  exit "$any_fail"
}

main "$@"
