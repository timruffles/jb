"use strict";

let s;
const NUMBER_RE = /[-+]?(\d+(\.\d+)?|\d+e\d+)/;

class State {
    constructor(tokens) {
        this.tokens = tokens || [];
        this.index = 0;
        // how many levels are we inside a nested JSON context?
        this.quoteDepth = 0;
        // if we're concatting, we buffer output till close
        this.concat = null;
        // is a concat the parent of the current string context?
        // if so, we don't want to quote stringValues
        this.isConcatLeaf = false;
    }
}

class Concat {
    constructor(parent) {
        this.output = [];
        this.depth = parent ? parent.depth + 1 : 1;
    }
}

function d(...args) {
    console.error(...args)
}

function debug(msg) {
}

function concatQuote(string) {
    for(let depth = s.concat.depth; depth; depth--) {
        string = string
            .replace(/\\/g,"\\")
            .replace(/"/g,'\\"');
    }
    return string;
}

function output(string) {
    assert(typeof string === 'string', `got non string '${string}'`);
    if(s.concat) {
        s.concat.output.push(concatQuote(string));
    } else {
        process.stdout.write(string);
    }
    return true;
}


function outputString(string) {
    if(s.concat) {
        return output(`"${concatQuote(string)}"`);
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

function concatLeafEnter() {
    s.isConcatLeaf = true;
}
function concatLeafExit() {
    s.isConcatLeaf = false;
}

function concat() {
    if(!take('co')) return;
    const concat = new Concat(s.concat);
    s.concat = concat;
    while(true) {
        concatLeafEnter();
        if(take('cc')) break;
        if(!expression()) break;
    }

    s.concat = concat.parent;
    outputString(concat.output.join(""))
    concatLeafExit();
    return true;
}

function key() {
    return unary()
        || concat()
        || number()
        || primitive()
        || string();
}

function array() {
    if(!take('ao')) return;
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
    if(!take('oo')) return;
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
    return false;
}

function string() {
    if(peek()) {
        return s.isConcatLeaf
            ? output(takeOne())
            : outputString(takeOne());
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
}

function main() {
    const tokens = process.argv.slice(2)
    if(tokens.length === 0) {
        return;
    }
    debug("parsing", tokens)
    parse(tokens);
}

main();
