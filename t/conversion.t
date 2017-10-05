#!/bin/bash

set -uo pipefail

main() {
  count=0

  comment CLI smoke test

  expect_code "no args exits ok" 0

  comment Object tests

  expect "empty object operator" "oe" "{}"
  expect "empty object" "oo oc" "{}"
  expect "single key" "oo oc" "{}"
}


comment() {
  echo "#" $@
}

fail() {
  count=$(( $count + 1 ))
  echo "not ok $count" $@
}

pass() {
  count=$(( $count + 1 ))
  echo "ok $count" $@
}

expect() {
  local msg=$1
  local input=$2
  local expected=$3

  output=$(./qj $input)

  assert_equal "$msg" "$output" "$expected"
}

assert_equal() {
  local msg=$1
  local actual=$2
  local expected=$3

  if [[ "$actual" == "$expected" ]]; then
    pass $msg
  else
    fail $msg - expected "'"$expected"'" got "'"$actual"'"
  fi
}

expect_code() {
  local msg=$1
  local expected=$2

  op=$(./qj 2>&1 $input)

  assert_equal "$msg" $? $expected
}

main
