#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_list linked_list;

linked_list* llCreate(void);
void llDestroy(linked_list* list);

int llPushFront(linked_list* list, void* data);
int llPushBack(linked_list* list, void* data);

int llPopBack(linked_list* list, void** pdata);
int llPopFront(linked_list* list, void** pdata);

// int llInsert(linked_list* list, void* data, int index);

void llPrint(linked_list* list);

#endif // !LINKED_LIST_H