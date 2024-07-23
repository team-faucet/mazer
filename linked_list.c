#include "linked_list.h"

#include <stdlib.h>

typedef struct ll_node {
    void* data;
    struct ll_node* next;
} ll_node;

const ll_node LL_TAIL_INITIALIZER = {NULL, NULL};

struct linked_list {
    ll_node head;
    ll_node* tail;
};

linked_list* llCreate(void) {
    linked_list* list = malloc(sizeof(*list));
    if (NULL != list) {
        list->tail = &list->head;
        *list->tail = LL_TAIL_INITIALIZER;
    }

    return list;
}

void llDestroy(linked_list* list) {
    while (list->tail != &list->head) {
        ll_node* next = list->head.next;
        list->head.data = next->data;
        list->head.next = next->next;
        free(next);
    }
    free(list);
}

int llPushFront(linked_list* list, const void* data) {
    ll_node* node = malloc(sizeof(*node));
    if (NULL != node) {
        *node = *list->head.next;
        list->head.data = data;
        list->head.next = node;

        return 0;
    }

    return -1;
}

int llPushBack(linked_list* list, const void* data) {
    ll_node* node = malloc(sizeof(*node));
    if (NULL != node) {
        *node = LL_TAIL_INITIALIZER;
        list->head.data = data;
        list->tail->next = node;
        list->tail = node;

        return 0;
    }

    return -1;
}

int llPopFront(linked_list* list, void** pdata) {
    if (list->tail != &list->head) {
        ll_node* next = list->head.next;
        *pdata = list->head.data;
        list->head.data = next->data;
        list->head.next = next->next;
        free(next);

        return 0;
    }

    return -1;
}

int llPopBack(linked_list* list, void** pdata) {
    if (list->tail != &list->head) {
        ll_node* prev = &list->head;
        while (prev->next != list->tail) {
            prev = prev->next;
        }
        *pdata = prev->data;
        *prev = *list->tail;
        free(list->tail);
        list->tail = prev;

        return 0;
    }

    return -1;
}

void llPrint(linked_list* list) {
    ll_node* node = list->head.next;
    while (node != &list->head) {
        if (printf("%p\n", node->data) < 0)
            abort();
        node = node->next;
    }
}