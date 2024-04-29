# Programming language "57Lang" (README AND COMPILER FOR LANGUAGE WILL BE AVAIBLE BEFORE 20th MAY 2024)

## How to code on this language
Someone may find my language similar with c++, but there are some big differences.

### Numbers
One of main features is that you need to enter all numbers in letters.

0 = zero
1 = one
2 = two
3 = three
4 = four
5 = five
6 = six
7 = seven
8 = eight
9 = nine

Maybe it would become more clear, when you'd see examples:

1. 57   = five_seven
2. 179  = one_seven_nine
3. -228 = -two_two_eight

At this moment my language can detect only integrer numbers, but it can contain non-integrer expression result in variable.

### Keywords
So, as you can see my language use letters to detect numbers, so, you need to use digits, to type commands.

1. if         = 57?
2. while      = 1000_7
3. assignment = :=
4. sinus      = $1#
5. sqrt       = 57#
6. cosinus    = <0$
7. input      = 57>>
8. output     = 57<<
9. int        = 57::
10. return    = ~57
11. end       = @57@
12. block_end = .57

You also need to use numbers, to name variables. You can name variables like: "3", "57_23", "57$$@__&23321"

For greater clarity I can translate some C code, and show how it looks on 57Lang.

C:
```
int x = 0;
scanf("%d", &x);    // input
if (x == 1337)
{
  while (x)
  {
    x = x - 1;
  }
  printf("%d", x * 10);
}
```

57Lang:
```
57::(1337 := zero)
57>> 1337        // input
57? (1337 == one_three_three_seven)
  1000_7 (1337)
    1337 := 1337 - one
  .57
  57<< 1337 * one_zero
.57
@57@
```

(You need @57@ sing in end of every program, for successful compilation)

You can check other code examples in "examples" folder in repository. It contains working code of factorial calculating program, and program that can solve quadratic equation.

### Grammar
If you understand how recursive descent algorithm


## How does it work
There are three stages of calculating code result

### Frontend
First stage of code processing.

Program is doing text analysis using recursive descent algorithm. It builds tree, that contains in text file (assets folder).

Example of this tree:

### Middleend

Second stage is math optimization of containing tree.

Program reads tree from private file, then it makes some optimization. After all processes it rewrites new, optimized tree in the same private file.

Optimization examples:

1. 1 + 1  -> 2
2. 0 * x  -> 0
3. 1 * x  -> x
4. x / x  -> 1

And other similar expressions

### Backend

Third (final) stage of code processing.

Program translates tree, that contains in private file, and then translates it to assembler commands. This is the moment, when SPU part starts.

Assembler reads asm code, built by backend program part. After that asm code calculates by spu. User can get the code result fron the console. To get more information about my software processing unit you can check one of my github repositories.


## Installation and run
First of all, you nees to install directory from github, then you can compile this program by typing in terminal
```
make all
```
