

## Parser

Since the language has no operators, do we need precendence?

    et = eat next token, use as escape
    so = eat until sc
    oo = eat pairs until oo, output
    co = concatenate string values until cc



e.g

    oo  co et nl toe fish cc thing  another pair  third ao 1 2 3 ac  oc
    {"\ntoefish":"thing","another":"pair","third":[1,2,3]}

## String output

Concat = buffer, and quote one level
Object = output a complete string
Top level = output a complete string
Array = output a complete string
