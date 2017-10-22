#!/bin/bash

set -uo pipefail

main() {
  count=0

  comment CLI smoke test

  expect_code "no args exits ok" 0

  comment Object tests

  expect "empty object operator" "oe" "{}"
  expect "empty object" "oo oc" "{}"
  expect "single key" "oo foo bar oc" '{"foo":"bar"}'
  expect "objects with array, string, number" \
    "oo  id 10  tags ao 1 2 3 ac  key value  oc" \
    '{"id":10,"tags":[1,2,3],"key":"value"}'
  expect "quoting in concats" "co et quote hello there et quote cc" '"\"hello there\""'
  expect "nested concats" "co oo some co oo json value oc cc oc cc" \
    '{"some":"{\"json\":\"value\"}"}'
  expect "deep nesting" "co oo some co oo nested oo json value oc oc cc oc cc" \
    '{"some":"{\"nested\":\"{\\\"json\\\":\\\"value\\\"}\"}"}'

  expect "concat" "co hi there cc" '"hithere"'
  expect "concat nesting" "co ao 1 2 3 ac another cc" '"\"[1,2,3]\"another"'
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
