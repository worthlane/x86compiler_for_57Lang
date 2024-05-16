#include <stdlib.h>
#include <time.h>

#include "visual.h"
#include "common/logs.h"

static size_t      IMG_CNT        = 1;
static const char* IMG_FOLDER_DIR = "src/assets/img/";

//---------------------------------------------------------------------------------------

void EndGraph(FILE* dotf)
{
    fprintf(dotf, "}");
}

//---------------------------------------------------------------------------------------

void StartGraph(FILE* dotf)
{
    fprintf(dotf, "digraph structs {\n"
	              "rankdir=TB;\n"
	              "node[color=\"black\",fontsize=14];\n"
                  "nodesep = 0.1;\n"
	              "edge[color=\"darkblue\",fontcolor=\"yellow\",fontsize=12];\n");
}

//---------------------------------------------------------------------------------------

void MakeImgFromDot(const char* dot_file)
{
    char img_name[MAX_IMG_FILE_LEN] = "";
    snprintf(img_name, MAX_IMG_FILE_LEN, "%simg%lu_%lu.png", IMG_FOLDER_DIR, IMG_CNT++, clock());

    char dot_command[MAX_CMD_LEN] = "";
    snprintf(dot_command, MAX_CMD_LEN, "dot %s -T png -o %s", dot_file, img_name);
    system(dot_command);

    PrintLog("<img src=\"%s\"><br>", img_name);
}

//---------------------------------------------------------------------------------------

void EndGraphic(FILE* gnuf)
{
    fprintf(gnuf, "set out\n");
}

//---------------------------------------------------------------------------------------

void StartGraphic(FILE* gnuf, const char* img_name)
{
    assert(img_name);

    fprintf(gnuf, "#! /opt/homebrew/Cellar/gnuplot -persist\n"
                  "reset\n"
                  "set terminal png size 800, 600\n"
                  "set output '%s'\n"
                  "set xlabel \"X\"\n"
                  "set ylabel \"Y\"\n"
                  "set yrange[-2:2]\n"
                  "set grid\n", img_name);
}

//---------------------------------------------------------------------------------------

char* GenImgName()
{
    char* img_name = (char*) calloc(MAX_IMG_FILE_LEN, sizeof(char));
    if (!img_name)
        return nullptr;

    snprintf(img_name, MAX_IMG_FILE_LEN, "%simg%lu_%lu.png", IMG_FOLDER_DIR, IMG_CNT++, clock());

    return img_name;
}

//---------------------------------------------------------------------------------------

void MakeImgFromGpl(FILE* fp, const char* gpl_file, const char* img_name)
{
    assert(gpl_file);
    assert(img_name);

    char gnu_command[MAX_CMD_LEN] = "";
    snprintf(gnu_command, MAX_CMD_LEN, "gnuplot '%s'", gpl_file);
    system(gnu_command);

    PrintLog("\n<img src=\"%s\"><br>\n", img_name);
    fprintf(fp, "\n\\includegraphics[scale = 0.5]{%s}\n", img_name);

}

