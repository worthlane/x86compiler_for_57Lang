#ifndef _TREE_OUTPUT_H_
#define _TREE_OUTPUT_H_

#include "tree.h"
#include "visual.h"

void PrintPrefixTree(FILE* fp, const tree_t* tree);

// ======================================================================
// EXPRESSION TREE NODES
// ======================================================================

int   NodeDump(FILE* fp, const void* dumping_node, const char* func, const char* file, const int line);

#ifdef DUMP_NODE
#undef DUMP_NODE
#endif
#define DUMP_NODE(node)     do                                                              \
                            {                                                               \
                                LogDump(NodeDump, (node), __func__, __FILE__, __LINE__);    \
                            } while(0)

void  PrintNodeData(FILE* fp, const tree_t* tree, const Node* node);
void  PrintSyncNodeData(FILE* fp, const tree_t* tree, const Node* node);

// ======================================================================
// EXPRESSION STRUCT
// ======================================================================

int    TreeDump(FILE* fp, const void* nodes, const char* func, const char* file, const int line);

#ifdef DUMP_TREE
#undef DUMP_TREE
#endif
#define DUMP_TREE(tree)         do                                                                 \
                                {                                                               \
                                    LogDump(TreeDump, (tree), __func__, __FILE__, __LINE__);    \
                                } while(0)

#endif
