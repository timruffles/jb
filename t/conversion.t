#!/bin/bash

count=0

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

  output=$(qj $input)
  if [[ "$output" == "$expected" ]]; then
    pass $msg
  else
    fail $msg - expected "'"$expected"'" got "'"$output"'"
  fi
}

qj() {
  echo foo
}

comment Object tests

expect "empty object operator" "oe" "{}"
expect "empty object" "oo oc" "{}"
expect "single key" "oo oc" "{}"

