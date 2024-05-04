#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

static const size_t WORD_LEN     = 100;
static const size_t UNKNOWN_ELEM = 0;

list_t* ListCtor()
{
    list_t* table = (list_t*) calloc(1, sizeof(list_t));
    assert(table);

    return table;
}

// -------------------------------------------------------------

void  ListDtor(list_t* table)
{
    free(table);

    if (table->root == NULL)
        return;

    list_elem_t* cur = table->root;

    while (cur != NULL)
    {
        list_elem_t* next = cur->next;
        ListElemDtor(cur);
        cur = next;
    }
}

// -------------------------------------------------------------

list_elem_t* ListElemCtor(const char* name, const size_t index, list_elem_t* next)
{
    assert(name);

    list_elem_t* elem = (list_elem_t*) calloc(1, sizeof(list_elem_t));
    assert(elem);

    char* inserted_name = strndup(name, WORD_LEN);
    assert(inserted_name);

    elem->name  = inserted_name;
    elem->index = index;
    elem->next  = next;

    return elem;
}

// -------------------------------------------------------------

void ListElemDtor(list_elem_t* elem)
{
    free(elem->name);

    free(elem);
}

// -------------------------------------------------------------

void InsertNameInList(list_t* table, const char* name, const size_t index)
{
    assert(name);

    list_elem_t* cur = table->root;

    while (cur != NULL)
    {
        if (!strncmp(cur->name, name, WORD_LEN))
            return;

        cur = cur->next;
    }

    list_elem_t* new_elem = ListElemCtor(name, index, table->root);
    table->root = new_elem;

    return;
}

// -------------------------------------------------------------

size_t GetElemIndexFromList(list_t* table, const char* name)
{
    assert(name);

    list_elem_t* cur = table->root;

    while (cur != NULL)
    {
        if (!strncmp(cur->name, name, WORD_LEN))
            return cur->index;

        cur = cur->next;
    }

    return UNKNOWN_ELEM;
}

// -------------------------------------------------------------

void DumpList(list_t* table)
{
    assert(table);

    list_elem_t* cur = table->root;

    while (cur != NULL)
    {
        printf("%s %lu\n", cur->name, cur->index);

        cur = cur->next;
    }
}
