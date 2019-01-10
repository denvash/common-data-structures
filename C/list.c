#include "list.h"
#include <stdlib.h>
#include <assert.h>

typedef struct node {
    ListElement element;
    struct node *next;
} *Node;

struct list_t {
    Node iterator;
    Node head;
    CopyListElement copy_element;
    FreeListElement free_element;
};

//======================= Static Functions Prototypes =======================||
/**
 * Swaps two elements given.
 * @param a target
 * @param b target
 */
static void swapElements(ListElement *a, ListElement *b);

//=========================== 'Public' Functions ===========================||

List listCreate(CopyListElement copyElement, FreeListElement freeElement) {
    if (!copyElement || !freeElement) {
        return NULL;
    }
    List new_list = malloc(sizeof(*new_list));
    if (!new_list) {
        return NULL;
    }
    new_list->iterator = NULL;
    new_list->head = NULL;
    new_list->copy_element = copyElement;
    new_list->free_element = freeElement;
    return new_list;
}

List listCopy(List list) {
    if (!list) {
        return NULL;
    }
    List copy_list = listCreate(list->copy_element, list->free_element);
    if (!copy_list) {
        return NULL;
    }
    Node temp = list->iterator;
    Node iterator_rememeber = NULL;
    Node new_cell = NULL;
    LIST_FOREACH(ListElement, iter, list) {
        new_cell = malloc(sizeof(*new_cell));
        if (!new_cell) {
            return NULL;
        }
        if (list->iterator == temp) {
            iterator_rememeber = new_cell;
        }
        if (!copy_list->head) {
            new_cell->element = list->copy_element(list->iterator->element);
            new_cell->next = NULL;
            copy_list->head = new_cell;
            copy_list->iterator = new_cell;
            continue;
        }
        copy_list->iterator->next = new_cell;
        new_cell->element = list->copy_element(list->iterator->element);
        new_cell->next = NULL;
        copy_list->iterator = new_cell;
    }
    list->iterator = temp;
    copy_list->iterator = iterator_rememeber;
    return copy_list;
}

int listGetSize(List list) {
    if (!list) {
        return -1;
    }
    Node temp = list->iterator;
    int size = 0;
    LIST_FOREACH(ListElement, iter, list) {
        size++;
    }
    list->iterator = temp;
    return size;
}

ListElement listGetFirst(List list) {
    if (!list || !list->head) {
        return NULL;
    }
    list->iterator = list->head;
    return list->head->element;
}

ListElement listGetNext(List list) {
    if (!list || !list->iterator) {
        return NULL;
    }
    list->iterator = list->iterator->next;
    if (!list->iterator) {
        return NULL;
    }
    return list->iterator->element;
}

ListElement listGetCurrent(List list) {
    if (!list || !list->iterator) {
        return NULL;
    }
    return list->iterator->element;
}

ListResult listInsertFirst(List list, ListElement element) {
    if (!list) {
        return LIST_NULL_ARGUMENT;
    }
    assert(element);
    Node new_head = malloc(sizeof(*new_head));
    if (!new_head) {
        return LIST_NULL_ARGUMENT;
    }
    new_head->element = list->copy_element(element);
    if (new_head->element == NULL) {
        free(new_head);
        return LIST_OUT_OF_MEMORY;
    }
    new_head->next = list->head;
    list->head = new_head;
    return LIST_SUCCESS;
}

ListResult listInsertLast(List list, ListElement element) {
    if (!list) {
        return LIST_NULL_ARGUMENT;
    }
    assert(element);
    if (!list->head) {
        listInsertFirst(list, element);
        return LIST_SUCCESS;
    }
    Node new_last = malloc(sizeof(*new_last));
    if (!new_last) {
        return LIST_NULL_ARGUMENT;
    }
    new_last->next = NULL;
    new_last->element = list->copy_element(element);
    if (!new_last->element) {
        free(new_last);
        return LIST_OUT_OF_MEMORY;
    }
    Node temp = list->iterator;

    LIST_FOREACH(ListElement, iter, list) {
        if (!list->iterator->next) {
            break;
        }
    }
    list->iterator->next = new_last;
    list->iterator = temp;
    return LIST_SUCCESS;
}

ListResult listInsertBeforeCurrent(List list, ListElement element) {
    if (!list) {
        return LIST_NULL_ARGUMENT;
    }
    assert(element);
    if (!list->iterator) {
        return LIST_INVALID_CURRENT;
    }
    if (list->iterator == list->head) {
        listInsertFirst(list, element);
        return LIST_SUCCESS;
    }
    Node new_current = malloc(sizeof(*new_current));
    if (!new_current) {
        return LIST_OUT_OF_MEMORY;
    }

    new_current->element = list->copy_element(element);
    if (!new_current->element) {
        free(new_current);
        return LIST_OUT_OF_MEMORY;
    }
    Node temp = list->iterator;
    LIST_FOREACH(ListElement, iter, list) {
        if (list->iterator->next == temp) {
            break;
        }
    }

    list->iterator->next = new_current;
    new_current->next = temp;
    list->iterator = temp;
    return LIST_SUCCESS;
}

ListResult listInsertAfterCurrent(List list, ListElement element) {
    if (!list) {
        return LIST_NULL_ARGUMENT;
    }
    assert(element);
    if (!list->iterator) {
        return LIST_INVALID_CURRENT;
    }
    Node new_current = malloc(sizeof(*new_current));
    if (!new_current) {
        return LIST_OUT_OF_MEMORY;
    }

    new_current->element = list->copy_element(element);
    if (!new_current->element) {
        free(new_current);
        return LIST_OUT_OF_MEMORY;
    }
    new_current->next = list->iterator->next;
    list->iterator->next = new_current;
    return LIST_SUCCESS;
}

ListResult listRemoveCurrent(List list) {
    if (!list) {
        return LIST_NULL_ARGUMENT;
    }
    if (!list->iterator) {
        return LIST_INVALID_CURRENT;
    }

    Node temp = list->iterator;
    if (list->iterator == list->head) {
        list->head = list->iterator->next;
    } else {
        LIST_FOREACH(ListElement, iter, list) {
            if (list->iterator->next == temp) {
                list->iterator->next = temp->next;
            }
        }
    }
    list->free_element((ListElement) temp->element);
    free(temp);
    list->iterator = NULL;
    return LIST_SUCCESS;
}

ListResult listSort(List list, CompareListElements compareElement) {
    if (!list || !compareElement) {
        return LIST_NULL_ARGUMENT;
    }
    Node ptr_dynamic = list->iterator;
    Node ptr_static = list->head;
    LIST_FOREACH(ListElement, iter, list) {
        if (compareElement(ptr_static->element, list->iterator->element) > 0) {
            swapElements(&ptr_static->element, &list->iterator->element);
        }
        if (!list->iterator->next) {
            if (!ptr_static->next) {
                break;
            }
            ptr_static = ptr_static->next;
            list->iterator = ptr_static;
        }
    }
    list->iterator = ptr_dynamic;
    return LIST_SUCCESS;
}

List listFilter(List list, FilterListElement filterElement, ListFilterKey key) {

    if (!list || !filterElement) {
        return NULL;
    }

    assert(key);
    bool flag = false;
    Node ptr_remember = list->iterator;
    List filtered_list = listCreate(list->copy_element, list->free_element);

    LIST_FOREACH(ListElement, iter, list) {
        if (filterElement(list->iterator->element, key)) {
            Node new_cell = malloc(sizeof(*new_cell));
            if (!new_cell) {
                listDestroy(filtered_list);
                return NULL;
            }
            if (!flag) {
                filtered_list->head = new_cell;
                flag = true;
            } else {
                filtered_list->iterator->next = new_cell;
            }
            new_cell->element = list->copy_element(list->iterator->element);
            new_cell->next = NULL;
            filtered_list->iterator = new_cell;
        }
    }
    listGetFirst(filtered_list);
    list->iterator = ptr_remember;
    return filtered_list;
}

ListResult listClear(List list) {
    if (!list) {
        return LIST_NULL_ARGUMENT;
    }
    listGetFirst(list);
    Node temp = list->iterator;
    while (temp) {
        temp = list->iterator->next;
        listRemoveCurrent(list);
        if (temp) {
            list->iterator = temp;
        }
    }
    return LIST_SUCCESS;
}

void listDestroy(List list) {
    if (!list) {
        return;
    }
    listClear(list);
    free(list->iterator);
    free(list->head);
    free(list);
}

//****************************************************************************
//============================ Static Functions ============================||

static void swapElements(ListElement *a, ListElement *b) {
    ListElement t = *a;
    *a = *b;
    *b = t;
}
