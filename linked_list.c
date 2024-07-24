#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct ll_node {
  void *data;
  struct ll_node *prev;
  struct ll_node *next;
} ll_node;

struct linked_list {
  ll_node sentinel;
};

linked_list *llCreate(void) {
  linked_list *list = malloc(sizeof(*list));
  if (NULL != list) {
    list->sentinel = (ll_node){NULL, &list->sentinel, &list->sentinel};
  }

  return list;
}

void llDestroy(linked_list *list) {
  for (ll_node *it = list->sentinel.next; list->sentinel.next != &list->sentinel; ) {
    list->sentinel.next = it->next;
    free(it);
  }
  free(list);
}

int llPushFront(linked_list *list, void *data) {
  ll_node *node = malloc(sizeof(*node));
  if (NULL != node) {
    *node = (ll_node){data, &list->sentinel, list->sentinel.next};
    
    list->sentinel.next->prev = node;
    list->sentinel.next = node;

    return 0;
  }

  return -1;
}

int llPushBack(linked_list *list, void *data) {
  ll_node *node = malloc(sizeof(*node));
  if (NULL != node) {
    *node = (ll_node){data, list->sentinel.prev, &list->sentinel};

    list->sentinel.prev->next = node;
    list->sentinel.prev = node;

    return 0;
  }

  return -1;
}

int llPopFront(linked_list *list, void **pdata) {
  if (list->sentinel.next != &list->sentinel) {
    ll_node *front = list->sentinel.next;
    *pdata = front->data;
    list->sentinel.next = front->next;
    front->next->prev = &list->sentinel;
    free(front);

    return 0;
  }

  return -1;
}

int llPopBack(linked_list *list, void **pdata) {
  if (list->sentinel.prev != &list->sentinel) {
    ll_node *back = list->sentinel.prev;
    *pdata = back->data;
    list->sentinel.prev = back->prev;
    back->prev->next = &list->sentinel;
    free(back);

    return 0;
  }

  return -1;
}

void llPrint(linked_list *list) {
  for (ll_node *it = list->sentinel.next; it != &list->sentinel; it = it->next) {
    if (printf("%p\n", it->data) < 0)
      abort();
  }
  if (printf("\n") < 0)
    abort();
}
