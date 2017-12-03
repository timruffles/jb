#!/usr/bin/env node
"use strict";

let s;
const NUMBER_RE = /^[-+]?(\d+(\.\d+)?|\d+e\d+)$/;

const USAGE = `
Usage: jb [ program text]

  Run 'npm help jb' to see the full README, including language
  definition.
`;

class State {
    constructor(tokens) {
        this.tokens = tokens || [];
        this.index = 0;
        // if we're concatting, we buffer output till close
        this.concat = null;
    }
}

class Concat {
    constructor(parent) {
        this.output = [];
        this.depth = parent ? parent.depth + 1 : 1;
        this.parent = parent;
        // concats take the 'string value' (basically the stuff
        // that would be between "" if output as a JSON string)
        // for expressions at their root
        this.atLeaf = true;
    }
}

function d(...args) {
    console.error(...args)
}
function debug() {
}

function concatQuote(string) {
    return string
        .replace(/\\/g,'\\\\')
        .replace(/"/g,'\\"');
}

function output(string) {
    assert(typeof string === 'string', `got non string '${string}'`);
    if(s.concat) {
        s.concat.output.push(string);
    } else {
        process.stdout.write(string);
    }
    return true;
}

function outputString(string) {
    if(atConcatLeaf()) {
        return output(string);
    } else {
        return output(`"${string}"`);
    }
}

function peek() {
    return s.tokens[s.index];
}

function take(string) {
    return nextIs(string) && s.tokens[s.index++];
}

function takeOne() {
    return s.tokens[s.index++];
}


function nextIs(value) {
    debug('next is', peek(), s.tokens);
    return peek() === value;
}

function assert(t, msg) {
    if(!t) {
      console.error(msg);
      process.exit(1);
    }
}

function assertSyntaxCorrect(t, msg) {
    return assert(t, msg)
}

function oneOrMore(fn) {
    let ok = false;
    while(fn()) {
        ok = true;
    }
    return ok;
}

function zeroOrMore(fn) {
    while(fn()) {
        // ok
    }
}

function atConcatLeaf() {
    return Boolean(s.concat && s.concat.atLeaf);
}

function concatLeafEnter() {
    if(s.concat) {
        s.concat.atLeaf = true;
    }
}
function concatLeafExit() {
    if(s.concat) {
        s.concat.atLeaf = false;
    }
}

function concat() {
    if(take('ce')) {
        outputString('')
        return true;
    }
    if(!take('co')) {
      assertSyntaxCorrect(!nextIs("cc"), "no open concat");
      return;
    }
    const concat = new Concat(s.concat);
    s.concat = concat;
    let closed = false;
    while(true) {
        concatLeafEnter();
        if(take('cc')) {
          closed = true;
          break;
        }
        if(!expression()) break;
    }

    assertSyntaxCorrect(closed, "missing concat close");

    // restore output context
    s.concat = concat.parent;
    output(`"${concatQuote(concat.output.join(''))}"`);
    return true;
}

function key() {
    return concat()
        || number()
        || primitive()
        || string();
}

function array() {
    if(take('ae')) {
        output('[]')
        return true;
    }
    if(!take('ao')) {
      assertSyntaxCorrect(!nextIs("ac"), "no open array");
      return;
    }
    output("[");
    let previous = false;
    while(true) {
        concatLeafExit();
        if(nextIs("ac")) break;
        if(previous) output(",");
        if(!expression()) break;
        previous = true;
    }
    assertSyntaxCorrect(take("ac"), "missing array close");
    output("]");
    return true;
}


function object() {
    if(take('oe')) {
        output('{}')
        return true;
    }
    if(!take('oo')) {
      assertSyntaxCorrect(!nextIs("oc"), "no open object");
      return;
    }
    debug("in object")
    output("{");
    let previous = false;
    while(true) {
        concatLeafExit();
        if(nextIs("oc")) break;
        if(previous) output(",");
        if(!key()) break;
        output(":");
        const v = expression();
        assertSyntaxCorrect(v, "missing pair");
        previous = true;
    }
    assertSyntaxCorrect(take("oc"), "missing object close");
    output("}");
    return true;
}

function paired() {
    return object()
        || array()
        || concat();
}

function unary() {
    if(!take('et')) return;
    const target = takeOne();
    outputString((() => {
        switch(target) {
            case "quote":
            case "dquote":
                return '"';
            case "squote":
                return "'";
            case "nl":
            case "newline":
                return "\n";
            case "dollar":
                return "$";
            case "space":
                return " ";
            case "tab":
                return "\t";
            case "bang":
            case "exclamation":
                return "!";
            case "bang":
            case "exclamation":
            case "question":
                return "?"
            case "gt":
                return ">";
            case "lt":
                return "<";
            case "equal":
                return "=";
            case "pipe":
                return "|";
            case "and":
                return "&";
            case "plus":
                return "+";
            case "minus":
                return "-";
            case "underscore":
                return "_";
            case "hash":
                return "#";
            case "at":
                return "@";
            default:
                throw Error(`invalid escape character '${target}'`)
        }
    })())
    return true;
}

function string() {
    if(peek()) {
        return outputString(takeOne());
    }
}

function primitive() {
    return;
}

function number() {
    if(NUMBER_RE.test(peek())) {
        output(takeOne());
        return true;
    }
}

function operator() {
    return unary() || paired();
}

function expression() {
    return operator()
        || number()
        || primitive()
        || string();
}

function bail(msg) {
    console.error(msg)
    process.exit(1);
}

function parse(input) {
    s = new State(input);

    const parsed = expression();
    if(!parsed) {
        bail("Unparsable input")
    }
    const tail = expression();
    if(tail) {
        bail("Invalid input - should evaluate to a single JSON value");
    }
}

function main() {
    const tokens = process.argv.slice(2)
    if(tokens.length === 0) {
        console.log(USAGE);
        return;
    }
    debug("parsing", tokens)
    parse(tokens);
}

main();
