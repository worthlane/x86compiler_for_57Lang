/*! \file
* \brief contains color library
*/

#define BLACK   "\x1b[30;1m"
#define RED     "\x1b[31;1m"
#define GREEN   "\x1b[32;1m"
#define YELLOW  "\x1b[33;1m"
#define BLUE    "\x1b[34;1m"
#define MAGENTA "\x1b[35;1m"
#define CYAN    "\x1b[36;1m"
#define WHITE   "\x1b[37;1m"
#define STD     "\x1b[0m"

#define PrintBlackText(fp, string, ...)   (fprintf(fp, BLACK   string STD, __VA_ARGS__))

#define PrintRedText(fp, string, ...)     (fprintf(fp, RED     string STD, __VA_ARGS__))

#define PrintGreenText(fp, string, ...)   (fprintf(fp, GREEN   string STD, __VA_ARGS__))

#define PrintYellowText(fp, string, ...)  (fprintf(fp, YELLOW  string STD, __VA_ARGS__))

#define PrintBlueText(fp, string, ...)    (fprintf(fp, BLUE    string STD, __VA_ARGS__))

#define PrintMagentaText(fp, string, ...) (fprintf(fp, MAGENTA string STD, __VA_ARGS__))

#define PrintCyanText(fp, string, ...)    (fprintf(fp, CYAN    string STD, __VA_ARGS__))

#define PrintWhiteText(fp, string, ...)   (fprintf(fp, WHITE   string STD, __VA_ARGS__))
