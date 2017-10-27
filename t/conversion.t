#!/bin/bash

set -uo pipefail

main() {
  count=0
  cmd=${TEST_CMD:-./qj}

  smoke_tests
  object_tests
  concat_tests

}

smoke_tests() {
  comment CLI smoke test

  expect "single key" "oo foo bar oc" '{"foo":"bar"}'
  expect_code "no args exits ok" '' 0
}

concat_tests() {
  comment Concat tests

  expect "concat" "co hi there cc" '"hithere"'
  expect "concat nesting 1" "co co inside cc cc" '"\"inside\""'
  expect "concat nesting 2" "co co A cc co B cc cc" '"\"A\"\"B\""'

  expect "quoting in concats" "co et quote hello there et quote cc" '"\"hello there\""'

  expect "nested concats" "co word co foo bar cc another cc" \
    '"word\"foobar\"another"'

  expect "nested concats and objects" "co oo some co oo json value oc cc oc cc" \
    '"{\"some\":\"{\\\"json\\\":\\\"value\\\"}\"}"'

  expect "deep nesting" "co oo some co oo nested oo json value oc oc cc oc cc" \
    '{"some":"{\"nested\":\"{\\\"json\\\":\\\"value\\\"}\"}"}'

  expect "concat json" "co ao 1 2 3 ac another cc" '"[1,2,3]another"'
}

object_tests() {
  comment Object tests

  expect "empty object operator" "oe" "{}"
  expect "empty object" "oo oc" "{}"
  expect "single key" "oo foo bar oc" '{"foo":"bar"}'
  expect "objects with array, string, number" \
    "oo  id 10  tags ao 1 2 3 ac  key value  oc" \
    '{"id":10,"tags":[1,2,3],"key":"value"}'
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

  output=$($cmd $input)

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
  local input=$2
  local expected=$3

  op=$($cmd 2>&1 $input)

  assert_equal "$msg" "$?" $expected
}

main
