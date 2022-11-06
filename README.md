# Stacks

A stack is a linear collection of tiles that interact with each other to do stuff.

Tiles do things when they are activated depending on their type and point.

The flow of the program is completely dictated by the use of activators.
When a tile is activated, it will be assigned an activation moment which
describes the cycle on which that tile will do something.

If no tiles are activated, the program ends because tiles are needed to
activate other tiles.

To start the program, initialization tiles `.` are used to activate the first
tiles. They are destroyed after start, leaving activators with the duty of keeping
the program running until what's desired is done. 

## First program

```
.>/|||<
```
On start, the `.` is used to activate the `>`, then the `.` is removed.

Cycle 0:
```
>/|||<
```
Because of the initialization activator, the `>` has an activation moment for cycle 1.
So, on cycle 1, `>` will activate everything to the right of it, including the killer `/` and
the activator `<`. 

This program creates a loop where `>` activates `<` back and forth. However, each 
time they activate each other, the killer `/` also gets activated and kills
one of the unit tiles on its right.

Cycle 2:
```
>/||<
```

Eventually, the unit tiles run out, and `<` is killed, breaking the loop,
causing the program to eventually end. 

## Program Writing

Programs can be written with any file extension.  

All whitespace is ignored.  

Use a `#` to start a comment. Everything is ignored until a newline.

Seperating tiles with a newline is completly optional and functions the same as putting a `=_=` to guarantee complete isolation
between lines.

Example file
```
.>,2	# Activates chuckpoint 2
|\<2	# Gets activated and then kills the unit tile
```

Will be interpreted as:
```
.>,2=_=|\<2
```


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

### Points
Every tile can be assigned a point on start by putting some numbers and letters
to the right of it.
```
.>`input1+input1@input1
```
In this example, the input tile, `+` tile and `@` tile are all pointing to the substack called `input1`.  
This will take an input from stdin, add one to it, and then output the integer value to stdout. 

### Stack calling
The `&` tile can be used to call another stack file by name.
Use value tiles (`*`) before and after the call to set the inputs and
what should be used to store the outputs
```
*input1*input2*input3 &stackname *output1 *output2
```
When the called stack activates an input tile, it will use one of the input values provided instead of
stdin.  
When the called stack activates an output tile, it will set the value of one of the ouput stacks provided
instead of outputting to stdout. 

Let there be a stack file called "addTwo" which takes a single input, adds 2 to it, and then outputs the value:  
*addTwo*
```
.>`1+1+1@1
```

Here is another file that will use "addTwo" twice to add four to a value:  
```
.>`userValue,2				# Takes in user input to `userValue` and then activates chuckpoint 2
>2*userValue &addTwo *userValue,3	# Calls "addTwo" with the value of userValue as the input and userValue as the output and then activates chuckpoint 3
>3*userValue &addTwo *userValue @userValue;	# Uses "addTwo" again and then outputs the value with a newline
```
	
