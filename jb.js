"use strict";

let s;

class State {
    constructor(tokens) {
        this.tokens = tokens || [];
        this.index = 0;
        // how many levels are we inside a nested JSON context?
        this.quoteDepth = 0;
        // if we're concatting, we buffer output till close
        this.concat = null;
    }
}

class Concat {
    constructor(parent) {
        this.output = [];
        this.depth = parent ? parent.depth + 1 : 0;
    }
}

function debug(msg) {
}

function output(string) {
    if(s.concat) {
        s.concat.push(string);
    } else {
        process.stdout.write(string);
    }
    return true;
}

function quote(string, depth) {
    while(depth--) {
        string = string
            .replace(/\\/g,"\\")
            .replace(/"/g,'\\"');
    }
    return string;
}

function outputString(string) {
    if(s.concat) {
        return output(`"${quote(string, s.concat.depth)}"`);
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
        throw Error(msg);
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


function concat() {
    if(!nextIs('co')) return;
    const concat = new Concat(s.concat);
    s.concat = concat;
    zeroOrMore(expression);

    s.concat = concat.parent;
    outputString(concat.output.join(""))
    return true;
}

function key() {
    return unary()
        || number()
        || primitive()
        || concat()
        || string();
}

function pair() {
    if(!key()) return;
    output(":");
    const v = expression();
    assertSyntaxCorrect(v, "missing pair");
    if(!take("oc")) {
        output(",")
    }
    return true;
}

function object() {
    if(!take('oo')) return;
    debug("in object")
    output("{");
    zeroOrMore(pair);
    output("}");
    return true;
}

function paired() {
    return object();
}

function unary() {
    return false;
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
    return;
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
}

function main() {
    const tokens = process.argv.slice(2)
    debug("parsing", tokens)
    parse(tokens);
}

main();
