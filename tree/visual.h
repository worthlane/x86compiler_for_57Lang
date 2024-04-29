#ifndef __GRAPHS_H_
#define __GRAPHS_H_

#include <stdio.h>

static const size_t MAX_CMD_LEN      = 200;
static const size_t MAX_IMG_FILE_LEN = 100;

char* GenImgName();

// ======================================================================
//  GRAPHS
// ======================================================================

static const char*  TMP_DOT_FILE = "tmp.dot";

void StartGraph(FILE* dotf);
void EndGraph(FILE* dotf);

void MakeImgFromDot(const char* dot_file);

// ======================================================================
//  GRAPHICS (GNUPLOT)
// ======================================================================

static const char*  TMP_GNU_FILE = "tmp.gpl";

void EndGraphic(FILE* gnuf);
void StartGraphic(FILE* gnuf, const char* img_name);

void MakeImgFromGpl(FILE* fp, const char* gpl_file, const char* img_name);

#endif
