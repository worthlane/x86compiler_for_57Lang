#ifndef _LABELS_TABLE_H_
#define _LABELS_TABLE_H_

struct list_elem_t
{
    size_t index;
    char*  name;

    list_elem_t* next;
};

struct list_t
{
    list_elem_t* root;
};

list_t*         ListCtor();
void            ListDtor(list_t* table);

list_elem_t* ListElemCtor(const char* name, const size_t index, list_elem_t* next);
void         ListElemDtor(list_elem_t* elem);

void DumpList(list_t* table);

size_t GetElemIndexFromList(list_t* table, const char* name);
void   InsertNameInList(list_t* table, const char* name, const size_t index);




#endif
