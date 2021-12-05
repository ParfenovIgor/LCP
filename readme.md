# LCP Version 1.0
## Lambda Calculus Programming
### By Igor Parfenov

## Program

Program supports both interactive and source code modes.

Run program without arguments to execute it in interactive mode.

Run program with path to file to execute it with corresponding source code. You can optionally enter **+** as second argument to enable showing reductions.

## Theory

The following definition of lambda expression is used:

**/\\ ::= V | (/\\)/\\ | \\V./\\**

**V ::= [A-za-z0-9]+**

The definitions mush be written in following format:

**name ::= function**

Example:

**inc ::= \n.\s.\z.((n)s)(s)z**

The program renames variables in order to remove name collisions. Therefore, the result can look weird.

You can use functions in definitions of other functions (even same, if the resulting function can be resolved).

## Interactive mode

Enter the definition in the same form, as it was written above.

It there was already definition with this name, it will be overwritten.

Enter **!** sign and file name, to open definitions from this file. You can't use in this file function main, otherwise it will be overwritten next query.

Enter **?** sign and lambda term, the program will apply reductions to it and show the result.

Enter **+** sign to enable showing reductions.

Enter **-** sign to disable showing reductions.

## Source code mode

Enter the definition in the same form, as it was written above.

There has to be **main** function in source code.

Only one function with given name has to be defined, another functions will be overwritten.

## Stdin

You can apply **read** to expression to call input. Example:

**(inc)read**

If you enter **\a.\b.b**, you will get as result **\a.\b.(a)b** (you will not get exactly this expression, but they will be equivalent).

You can enter function by their name. Example:

**((\x.x)read)\a.\b.b**

If you enter **inc**, you will get as result **\a.\b.(a)b** (you will not get exactly expression term, but they will be equivalent).
