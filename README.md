# qj(1) -- a friendly way to author JSON in the CLI

## SYNOPSIS

qj oo key1 value1 key2 $value oc
qj oo key co Write  anything  here cc bang cc oc
qj -c < echo '{"key1":"value1","key2":"'$value'"}'

## DESCRIPTION

**qj** converts to and from the qj language - which contains no special 
characters - to JSON. It's designed for use when it is necessary to embed JSON
in another language - for instance, writing JSON in Bourne shell which is itself
wrapped inside a JSON config file:

  {
    "test": "browsers=$(qj oo browserName $BROWSER) ./test",
  }

instead of quite hard-to-read (and tricky to debug) constructions like:

  {
    "test":"browsers='{"browserName":"'$BROWSER'"}' ./test",
  }

  {
    "test":"browsers=\"{\"browserName\":\"$BROWSER\"}\" ./test",
  }

Surprisingly, though it would seem verbose at first glance, qj can often
be shorter than the equivalent JSON it produces

    oo name qj version 1.2 tags ao language cli ac oc

    {"name":"qj","version":1.2,"tags":["cli","language"]}

You can capitalise the operators if you like, and leave off object closers
to have them automatically close in reverse order at the end of the text

    > qj OO name qj version 1.2 tags AO language cli
    {"name":"qj","version":1.2,"tags":["cli","language"]}


## EXAMPLES

Since no special characters are used within qj, it makes it easy to
construct qj source code programatically. For instance, within shell code:

    > qj oo message so $(qj oo nesting so $(qj oo is easy oc) sc oc) sc oc
    {"message":"{\"nesting\":\"{\\\"is\\\":\\\"easy\\\"}\"}"}

    > export A=variables 
    > export B=ever
    > qj ao use $A how${B} you like ac
    ["use","variables","however","you","like"]

qj's operators are all two character codes, followed by a space.

### Paired operators

Operators determine how qj interprets characters that follow them.
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

    > qj ao oo so qj is a so so language sc ae oc ac
    [{"qj is a so so language":[]}]

## LANGUAGE

### TL;DR

- operators are two character sequences, which follow regular naming patterns
- whitespace is important: a single space separates expressions
- if it's not an operator, it's a literal or an operator's argument

### Operation

A qj program is a sequence of expressions. Expressions are either literals, in
the case of booleans, numbers, and strings, or operators. Operators are two characters
long, follow by a space, followed by their arguments. Each operators can interpret its
arguments in a different way, in general though a single space separates arguments.

### Operators

#### String (so, sc)

The string operator consumes all characters until the (case insensitive) characters ' sc'
are encountered.

    > qj SO this is all, like, so verbatim right? SC
    "this is all, like, so verbatim right?"

This operator is useful for any multi-word strings, and for embedding special characters
valid in the context qj is being written. To attempt to avoid syntax conflicts in any 
embedding context, qj's syntax disallows special characters outside the string operator.
 
    > qj oo email so qj@example.com sc oc
    {"email":"qj@example.com"}

If you want to be sure your special characters don't conflict with your context, use the
concat operator instead.

#### Concatenations (co, cc)

Concatenations evaluate to a string concatenating all enclosed expressions.

    > qj co special  characters  hey et question no  problem et bang et nl cc
    "special characters hey? no problem!\n"


If you embed arrays or objects within a concatenation they will be string-ified as JSON:

    > qj co oe cc 
    "{}"
    > qj co et lt ao 1 2 3 ac et gt cc
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

    > qj ut ffaa
    "\uffaa"

API:

    ut [ hexidemical character * 4 ]

To include a unicode escape, use the unicode token:

    TODO clarify JSON's encoding habits

Note: You can usually embed unicode straight into qj program text.

### Literals

#### Strings

A string literal is a sequence of characters that is > 0, != 2 in length.

### Concat

Concat has two rules: providing strings containing escape tokens, and
nesting JSON. Some examples of using escape tokens:

    > qj co wow et bang cc
    "wow!"
    > qj co hi there cc
    "hithere"
    > qj co hi et space there cc
    "hi there"

JSON is quoted to form a valid string:

    > qj co oo hi there oc cc
    "{\"hi\":\"there\"}"

TODO - should nested concats collapse? Not much of a use-case
TODO - string producing values get consumed by: concats, arrays, top-level, object. Each
       has a slightly different quoting requirement

## Limitations

- Consumes whole input before starting - shouldn't be an issue as you're
  not converting gigabytes of JSON to JQ (right?!).
