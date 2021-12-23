# LCP Version 2.0
## Lambda Calculus Programming
### By Igor Parfenov

## Table of Contents
* [Changes from Previous Version](#changes-from-previous-version)
* [Program](#program)
* [Theory](#theory)
* [Flags](#flags)
* [Interactive Mode](#interactive-mode)
* [Source Code Mode](#source-code-mode)
* [Stdin](#stdin)
* [Stdout](#stdout)

## Changes from Previous Version

* Allow **outermost** reduction strategy in addition to **innermost**
* Faster **reductions** (lower degree of polynom of complexity)
* More flexible Settings
* More complex `prelude.lcp` library
    * Modules
    * More Logic Functions
    * Decrement and Substraction
* Examples on recursive `factorial` definitions for both reduction strategies

## Program

Program allows to use **innermost** and **outermost** reductions strategies.

Program supports both **interactive** and **source code** modes.

Run program without arguments to execute it in **interactive** mode.

Run program with path to file to execute it with corresponding **source code**. You can optionally enter **flags**, which will be listed in corresponding section.

## Theory

The following definition of **lambda expression** is used:

`/\\ ::= V | (/\\)/\\ | \\V./\\`

`V ::= [A-Za-z0-9]+`

Program allows to use **innermost** and **outermost** reductions strategies.
By default the **innermost** reduction strategy is enabled.

**Innermost** reduction strategy applies the **redex** with the ==most left ending==.
**Outermost** reduction strategy applies the **redex** with the ==most left beginning==.

The *definitions* must be written in following format:
`name ::= function`

Example:
`INC ::= \n.\s.\z.((n)s)(s)z`

The *definitions* and *lambda expressions* are ==case sensitive==.

The *definitions* should be written in uppercase, and *variables* in lowercase, but it is not obligatory.

The program renames variables in order to remove name collisions. Therefore, the result may look weird.

You can use *functions* in *definitions* of other *functions*, as long as all *definitions* can be unwrapped.

## Flags

These *flags* can be used in both modes, where they mean the same.

Enter `+` sign to enable showing *reductions*.

Enter `^` sign to enable showing number of *variables* after every *reduction*.

Enter `-` sign to disable showing *reductions* and number of *variables*.

Enter '<' sign to switch to **innermost** reduction strategy. By default this strategy is used.

Enter '>' sign to switch to **outermost** reduction strategy.

## Interactive Mode

Enter the *definition* in the same form, as it was written above.
It there was already *definition* with this name, it will be overwritten.

Enter `!` sign and file name, to open definitions from this *file*. You can't use from this *file* function `MAIN`, since it will be overwritten by next *query*.

Enter `?` sign and *lambda expression*, the program will apply reductions to it and show the result. If there are defined *functions*, which are equavalent to the result, they will be showed.

Enter any *flag* to apply it.

## Source Code Mode

Enter the *definition* in the same form, as it was written above.
There has to be `MAIN` function in program.
Only one *function* with given name has to be defined.

Enter `!` sign and file name to import *definitions* from this file. You can do recursive importing. The **importer** howerer, will import only once every file by single command.

## Stdin

You can apply `READ` to *expression* to call *input*. The `READ` is **case insensitive**. The *input* will be applied to appropriate argument.

Example:
`(INC)READ`
If you enter
`\a.\b.b`
, you will get as result
`\a.\b.(a)b`
(you will not get exactly this expression, but they will be equivalent).

You can enter functions by their name.

Example:
`((\x.x)read)\a.\b.b`
If you enter
`INC`
, providing, that it was defined, you will get as result
`\a.\b.(a)b`
(you will not get exactly expression term, but they will be equivalent).

## Stdout

You can apply `WRITE` to *expression* to call *output*. The `WRITE` is **case insensitive**. The `WRITE` will be applied to appropriate argument, and then the result will be written.

Example:
`(\x.\y.y)WRITE`
will write `\y.y`.
(you will not get exactly this expression, but they will be equivalent).

You can write functions by their name.

Example:
`(\x.INC)WRITE`
and
`(\x.\n.\s.\z.((n)s)(s)z)WRITE`
, providing, that `INC` was defined, both will write `INC`.
