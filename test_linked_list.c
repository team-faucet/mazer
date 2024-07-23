#include "linked_list.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // Test creating and destroying an empty list
    linked_list* list = llCreate();
    assert(list != NULL);
    llDestroy(list);

    // Test creating and destroying a list with one element
    list = llCreate();
    assert(list != NULL);
    int data = 42;
    llPushFront(list, &data);
    int* pdata;
    assert(llPopFront(list, (void**)&pdata) == 0);
    assert(*pdata == 42);
    llDestroy(list);

    // Test creating and destroying a list with multiple elements
    list = llCreate();
    assert(list != NULL);
    for (int i = 0; llPrint(list), (i < 5); i++) {
        int* pi = malloc(sizeof(*pi));
        *pi = i;
        llPushBack(list, pi);
    }
    for (int i = 4; llPrint(list), (i >= 0); i--) {
        int* data;
        assert(llPopBack(list, (void**)&data) == 0);
        assert(*data == i);
        free(data);
    }
    llDestroy(list); 

    return EXIT_SUCCESS;
}