#!/bin/bash

GREP=${GREP:-}

STDERR_FILE=''

set -uo pipefail

main() {
  count=0
  cmd=${TEST_CMD:-./qj}

  STDERR_FILE=$(mktemp)

  smoke_tests
  object_tests
  concat_tests
  edge_case_tests

}

smoke_tests() {
  comment "CLI smoke test"

  expect "single key" "oo foo bar oc" '{"foo":"bar"}'
  expect_code "no args exits ok" '' 0
  expect "html demo" \
    "oo html co et lt img et space src= et quote things et quote et gt cc oc" \
    '{"html":"<img src=\"things\">"}'
}

concat_tests() {
  comment "Concat tests"

  expect "concat" "co hi there cc" '"hithere"'
  expect "concat nesting 1" "co co inside cc cc" '"\"inside\""'
  expect "concat nesting 2" "co co A cc co B cc cc" '"\"A\"\"B\""'

  expect "quoting in concats" "co et quote hello there et quote cc" '"\"hellothere\""'

  expect "nested concats" "co word co foo bar cc another cc" \
    '"word\"foobar\"another"'

  expect "nested concats and objects" "co oo some co oo json value oc cc oc cc" \
    '"{\"some\":\"{\\\"json\\\":\\\"value\\\"}\"}"'

  expect "deep nesting" "co oo some co oo nested oo json value oc oc cc oc cc" \
    '"{\"some\":\"{\\\"nested\\\":{\\\"json\\\":\\\"value\\\"}}\"}"'

  op=$($cmd 2>&1 co oo some co oo nested oo json value oc oc cc oc cc)
  read=$(node -p 'JSON.parse(JSON.parse('$op').some).nested.json')
  assert_equal "serialized correctly" "$read" "value"

  expect "concat json" "co ao 1 2 3 ac another cc" '"[1,2,3]another"'
}

object_tests() {
  comment "Object tests"

  expect "empty object operator" "oe" "{}"
  expect "empty object" "oo oc" "{}"
  expect "single key" "oo foo bar oc" '{"foo":"bar"}'
  expect "objects with array, string, number" \
    "oo  id 10  tags ao 1 2 3 ac  key value  oc" \
    '{"id":10,"tags":[1,2,3],"key":"value"}'

  expect_error "unclosed" 'oo' "missing object close"
  expect_error "unopened" 'oc' "no open object"
  expect_error "unbalanced" 'oo key oo oc' "missing object close"
}

edge_case_tests() {
  comment "Edge-case tests"

  expect "numeric literal" "1" "1"
  expect "float literal" "1.0" "1.0"
  expect "negative number" "-1" "-1"
  expect "large number" "17000342" "17000342"
  expect "small number" "0.0002001" "0.0002001"

  expect "single word" "hello" '"hello"'
  expect "empty" "" ''
  expect "whitespace" "               " ''

  expect_error "multiple strings" 'hi there' "Invalid input - should evaluation to a single JSON value"

  local tab=$'\t'
  expect "tabs only - should be empty" "$tab$tab" ''
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

expect() {
  local msg=$1
  local input=$2
  local expected=$3

  if ! should_run "$msg"; then
    return
  fi

  output=$($cmd 2>$STDERR_FILE $input )
  if [[ $? != 0 ]]; then
    fail "${msg}: '${input}' caused an error, with output '${output}'"
  fi
  if [[ -s $STDERR_FILE ]]; then
    fail "${msg}: '${input}' unexpectly wrote to stderr, with '$(cat $STDERR_FILE)'"
  fi

  assert_equal "$msg" "$output" "$expected"
}

expect_error() {
  local msg=$1
  local input=$2
  local error_expected=$3

  if ! should_run "$msg"; then
    return
  fi

  output=$($cmd 2>$STDERR_FILE $input )
  if [[ $? == 0 ]]; then
    fail "${msg}: expected '${input}' to cause an error, but no exit code occured. Instead got '${output}'"
  fi

  assert_equal "$msg" "$(cat $STDERR_FILE)" "$error_expected"
}

expect_code() {
  local msg=$1
  local input=$2
  local expected=$3


  if ! should_run "$msg"; then
    return
  fi

  op=$($cmd 2>&1 $input)

  assert_equal "$msg" "$?" $expected
}

should_run() {
  local test_name=$1

  # if empty, or matches
  if [[ -z $GREP ]] || [[ "$test_name" =~ "$GREP" ]]; then
    return 0
  else
    return 1
  fi
}

main
