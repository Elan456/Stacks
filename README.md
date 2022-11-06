# Stacks

A stack is a linear collection of tiles.

## Tiles
### Types
#### Activators
* `>`
Activates all to the right until `_`
* `<`
Activates all to the left until `_`
* `}`
Activates what's immediately to the right
and blocks other activators
* `{`
Activates what's immediately to the left
and blocks other activators
* `.`
Activates what's immediately to the right on start
* `,`
Activates what chuckpoint it points to
* `?`
Activates all until `_` in the direction that
has a higher value (By consecutive units or substack value).

#### Deactivators
* `(`
Deactivates all to the right until `_`
* `)`
Deactivates all to the left until `_`

#### Killers
* `/`
Kills what is on its right
* `\`
Kills what is on its left
* `-`
Kills what is to the left of the chuckpoint it points to
or removes what is at the end of a substack

#### Chuckers
* `^`
Chucks (makes a copy and moves) the tile on its left to
the left of the chuckpoint it points to or to the end of
a substack
* `+`
Chucks a `|` (unit) tile to the left of the chuckpoint it points to
or to the end of a substack
#### Input/Output
* `@`
Outputs the length of the substack it points to as an integer to standard output  
In a called stack, it will add its value to the output list.
* `$`
Outputs the length of the substack as an ASCII character to standard output
* **`**  
  Takes input from standard in.
In a called stack, it will use the passed inputs instead.
* `;`
Outputs a new line to standard output

#### Substack
* `*`
Acts as the value of the substack (length) it points to. Used for `?` comparisons and stack calling
* `~`
Removes everything from the substack it points to
* `&`
Runs another stacks program. Values `*` to the left are used as inputs and values `*` to the right
are used to hold output values.

#### Misc
* `_`
Blocks all activations and deactivations
* `=`
Cannot be killed by the killers
* `|`
Does nothing and acts as a single unit
	
