#ifndef _LABELS_TABLE_H_
#define _LABELS_TABLE_H_

struct table_elem_t
{
    size_t index;
    char*  name;

    table_elem_t* next;
};

struct labels_table_t
{
    table_elem_t* root;
};

labels_table_t* LabelsTableCtor();
void            LabelsTableDtor(labels_table_t* table);

table_elem_t* TableElemCtor(const char* name, const size_t index, table_elem_t* next);
void          TableElemDtor(table_elem_t* elem);

void DumpLabelsTable(labels_table_t* table);

size_t GetElemIndexFromLabelsTable(labels_table_t* table, const char* name);
void InsertNameInLabelsTable(labels_table_t* table, const char* name, const size_t index);




#endif
