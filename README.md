# Язык программирования "57Lang" + компилятор для архитектуры x86-64

## Описание
57Lang - полный по Тьюрингу язык программирования.

![](readme_assets/logo.jpg)

Сама программа состоит из трёх частей:
1. Frontend - обработка входного файла и построение дерева операций.
2. Middleend - оптимизация AST-дерева.
3. Backend - преобразование AST-дерева в бинарный исполняемый файл при помощи компилятора под архитектуру x86-64.

## Синтаксис языка
Синтаксис может чем-то напоминать язык C++, но на самом деле между ними имеется большая разница.
1. В языке присутствует поддержка функций, рекурсии, циклов и блоков условий.
2. Все переменные имеют один тип. В переменной может храниться не целое знаковое число, но присвоить ей можно только целое значение.
3. Каждая функция обязана возвращать значение.
4. В программе должна присутствовать функция с названием ```_0_``` (альтернатива ```main``` в языке Си).

### Числа
Одной из основных идей при написании языка было поменять роль цифр и букв в обычных языках между собой. Таким образом все числа в этом языке мы должны задавать словами.

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

Возможно это станет понятней, если показать примеры таких чисел.

1. 57   = five_seven
2. 179  = one_seven_nine
3. -228 = -two_two_eight

### Ключевые слова
Так как вместо цифр мы используем буквы для представления чисел, мы будем представлять ключевые слова благодаря цифрам и специальным знакам.

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

Название переменной также является числом со специальными символами. Примеры корректных названий: ```3```, ```57_23```, ```57$$@_&23321```.

Для большей понятности приведу пример одного и того же рабочего кода на Си и на 57Lang.

C:
```C
int x = 0;
scanf("%d", &x);        // input
if (x == 1337)
{
  while (x)
  {
    x = x - 1;
  }
  printf("%d", x * 10); // output
}
```

57Lang:
```js
57::(1337 := zero)
57>> 1337               // input
57? (1337 == one_three_three_seven)
  1000_7 (1337)
    1337 := 1337 - one
  .57
  57<< 1337 * one_zero  //output
.57
```

В конце каждой программы должен стоять знак ```@57@```. (Задел на будущее, для возможности создания нескольких программ из одного текстового файла).

Другие примеры кода можно найти в папке репозитория с названием ```examples```. Там содержится программа, считающая факториал числа и программа, решающая квадратное уравнение.

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
