code = expression+
expression = operator
    | number
    | primitive
    | string

operator = unary
    | paired

# unicode token or escape token
unary = 'et' string
    | 'ut' string

paired = array
    | object
    | concat

array = 'ao' (expression*) 'ac'
    | 'ae'

key = unary
    | number
    | primitive
    | concat
    | string

pair = key expression
object = 'oc' (pair*) 'oc'
    | 'oe'

concat = 'co' (expression*) 'cc'
    | 'ce'

null = 'null'
boolean = 'true' | 'false'
primitive = null
    | boolean

number = ...

# any non-whitespace
string = /\S/+
