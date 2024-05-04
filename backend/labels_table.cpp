#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

#include "labels_table.h"

static const size_t WORD_LEN     = 100;
static const size_t UNKNOWN_ELEM = 0;

labels_table_t* LabelsTableCtor()
{
    labels_table_t* table = (labels_table_t*) calloc(1, sizeof(labels_table_t));
    assert(table);

    return table;
}

// -------------------------------------------------------------

void  LabelsTableDtor(labels_table_t* table)
{
    free(table);

    if (table->root == NULL)
        return;

    table_elem_t* cur = table->root;

    while (cur != NULL)
    {
        table_elem_t* next = cur->next;
        TableElemDtor(cur);
        cur = next;
    }
}

// -------------------------------------------------------------

table_elem_t* TableElemCtor(const char* name, const size_t index, table_elem_t* next)
{
    assert(name);

    table_elem_t* elem = (table_elem_t*) calloc(1, sizeof(table_elem_t));
    assert(elem);

    char* inserted_name = strndup(name, WORD_LEN);
    assert(inserted_name);

    elem->name  = inserted_name;
    elem->index = index;
    elem->next  = next;

    return elem;
}

// -------------------------------------------------------------

void TableElemDtor(table_elem_t* elem)
{
    free(elem->name);

    free(elem);
}

// -------------------------------------------------------------

void InsertNameInLabelsTable(labels_table_t* table, const char* name, const size_t index)
{
    assert(name);

    table_elem_t* cur = table->root;

    while (cur != NULL)
    {
        if (!strncmp(cur->name, name, WORD_LEN))
            return;

        cur = cur->next;
    }

    table_elem_t* new_elem = TableElemCtor(name, index, table->root);
    table->root = new_elem;

    return;
}

// -------------------------------------------------------------

size_t GetElemIndexFromLabelsTable(labels_table_t* table, const char* name)
{
    assert(name);

    table_elem_t* cur = table->root;

    while (cur != NULL)
    {
        if (!strncmp(cur->name, name, WORD_LEN))
            return cur->index;

        cur = cur->next;
    }

    return UNKNOWN_ELEM;
}

// -------------------------------------------------------------

void DumpLabelsTable(labels_table_t* table)
{
    assert(table);

    table_elem_t* cur = table->root;

    while (cur != NULL)
    {
        printf("%s %lu\n", cur->name, cur->index);

        cur = cur->next;
    }
}
