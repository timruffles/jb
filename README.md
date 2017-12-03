# jb(1) -- a friendly way to author JSON in the CLI

## SYNOPSIS

```sh
> export value=hi
> jb oo key1 value1 key2 $value oc
{"key1":"value1","key2":"hi"}
> jb oo name jb version 1.2 tags ao language cli ac oc
{"name":"jb","version":1.2,"tags":["language","cli"]}
```

## DESCRIPTION

**jb** converts to and from the jb language - which contains no special 
characters - to JSON. It's designed for use when it is necessary to embed JSON
in another language - for instance, writing JSON in Bourne shell which is itself
wrapped inside a JSON config file:

    {
      "test": "browsers=$(jb oo browserName $BROWSER) ./test",
    }

instead of quite hard-to-read (and tricky to debug) constructions like:

    {
      "test":"browsers='{"browserName":"'$BROWSER'"}' ./test",
    }

    {
      "test":"browsers=\"{\"browserName\":\"$BROWSER\"}\" ./test",
    }

Surprisingly, though it would seem verbose at first glance, jb can often
be shorter than the equivalent JSON it produces

    oo name jb version 1.2 tags ao language cli ac oc

    {"name":"jb","version":1.2,"tags":["cli","language"]}



## EXAMPLES

Since no special characters are used within jb, it makes it easy to
construct jb source code programatically. For instance, within shell code:

    > jb oo message so $(jb oo nesting so $(jb oo is easy oc) sc oc) sc oc
    {"message":"{\"nesting\":\"{\\\"is\\\":\\\"easy\\\"}\"}"}

    > export A=variables 
    > export B=ever
    > jb ao use $A how${B} you like ac
    ["use","variables","however","you","like"]

jb's operators are all two character codes, followed by a space.

### Paired operators

Operators determine how jb interprets characters that follow them.
They follow a naming scheme, e.g 'sc'

    s                c

    string           close

    kind             action    

So, for instance, ao is 'array open'. The standard actions are:

- o - open
- c - close
- e - empty

The kinds are:

- a - array
- o - object
- s - string
- c - concat: concats evaluate to a string formed by concatenating the string 
      values formed by the code that it encloses

So:

    > jb ao oo so jb is a so so language sc ae oc ac
    [{"jb is a so so language":[]}]


## LANGUAGE

### TL;DR

- operators are two character sequences, which follow regular naming patterns
- whitespace is important: a single space separates expressions
- if it's not an operator, it's a literal or an operator's argument

### Operation

A jb program is a sequence of expressions. Expressions are either literals, in
the case of booleans, numbers, and strings, or operators. Operators are two characters
long, follow by a space, followed by their arguments. Each operators can interpret its
arguments in a different way, in general though a single space separates arguments.

### Operators

#### String (so, sc)

The string operator consumes all characters until the (case insensitive) characters ' sc'
are encountered.

    > jb SO this is all, like, so verbatim right? SC
    "this is all, like, so verbatim right?"

This operator is useful for any multi-word strings, and for embedding special characters
valid in the context jb is being written. To attempt to avoid syntax conflicts in any 
embedding context, jb's syntax disallows special characters outside the string operator.
 
    > jb oo email so jb@example.com sc oc
    {"email":"jb@example.com"}

If you want to be sure your special characters don't conflict with your context, use the
concat operator instead.

#### Concatenations (co, cc)

Concatenations evaluate to a string concatenating all enclosed expressions.

    > jb co special  characters  hey et question no  problem et bang et nl cc
    "special characters hey? no problem!\n"


If you embed arrays or objects within a concatenation they will be string-ified as JSON:

    > jb co oe cc 
    "{}"
    > jb co et lt ao 1 2 3 ac et gt cc
    "<[1,2,3]>"

TODO - good idea? Or have js operator? Takes jq text and stringifies?


#### Escape token - et

To embed special characters with named mnemonics, you can use the escape token.

Named special characters:

- and: &
- asterisk: *
- backslash: \
- backtick: `
- bang: !
- colon: :
- comma: ,
- crlf: \r\n
- dquote: "
- dollar: $
- equals: =
- feed: \f
- hash: #
- minus: -
- newline | nl: \n
- null: \0
- percent: %
- period: .
- pipe: |
- plus: +
- question: ?
- quote: '
- return | cr: \r
- semicolon: ;
- slash: /
- tab: \t - you can also simply include these in program text, though they're hard to see
- tilde: ~

- l(eft)brace: {
- r(ight)brace: }
- l(eft)bracket: [
- r(ight)bracket: ]
- l(eft)paren: (
- r(ight)paren: )

- lessthan | lt: <
- greatherthan | gt: >


#### Unicode token - ut

TODO - support non BMP directly, unlike JSON with surrogate pairs.

Example:

    > jb ut ffaa
    "\uffaa"

API:

    ut [ hexidemical character * 4 ]

To include a unicode escape, use the unicode token:

    TODO clarify JSON's encoding habits

Note: You can usually embed unicode straight into jb program text.

### Literals

#### Strings

A string literal is a sequence of characters that is > 0, != 2 in length.

### Concat

Concat has two rules: providing strings containing escape tokens, and
nesting JSON. Some examples of using escape tokens:

    > jb co wow et bang cc
    "wow!"
    > jb co hi there cc
    "hithere"
    > jb co hi et space there cc
    "hi there"

JSON is quoted to form a valid string:

    > jb co oo hi there oc cc
    "{\"hi\":\"there\"}"

TODO - should nested concats collapse? Not much of a use-case
TODO - string producing values get consumed by: concats, arrays, top-level, object. Each
       has a slightly different quoting requirement

## Limitations

- Consumes whole input before starting - shouldn't be an issue as you're
  not converting gigabytes of JSON to JQ (right?!).
