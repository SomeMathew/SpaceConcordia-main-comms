/*
 * linkedList.c
 *
 *  Created on: Jul 15, 2016
 *      Author: math
 */

#include <stddef.h>
#include <assert.h>

#include "linkedList.h"

void linkedList_initList(struct linkedList * list) {
    assert(list != NULL);

    list->length = 0;
    list->head.next = &list->head;
    list->head.previous = &list->head;
}

void linkedList_initNode(struct linkedList_node * node, void * element) {
    assert(node != NULL);
    node->element = element;
    node->next = NULL;
    node->previous = NULL;
}

void linkedList_addAfter(struct linkedList * list, struct linkedList_node * newNode,
        struct linkedList_node * position) {
    assert(list != NULL && newNode != NULL && position != NULL);

    newNode->list = list;
    struct linkedList_node * const successor = position->next;
    newNode->previous = position;
    newNode->next = successor;
    position->next = newNode;
    successor->previous = newNode;
    list->length++;
}

void linkedList_addBefore(struct linkedList * list, struct linkedList_node * newNode,
        struct linkedList_node * position) {
    assert(newNode != NULL && position != NULL);

    newNode->list = list;
    struct linkedList_node * const predecessor = position->previous;
    newNode->previous = predecessor;
    newNode->next = position;
    position->previous = newNode;
    predecessor->next = newNode;
    list->length++;
}

void linkedList_addOrdered(struct linkedList * list, struct linkedList_node * newNode,
        int (*compare)(void *, void *)) {
    assert(list != NULL && newNode != NULL);

    struct linkedList_node * cursorNode = &(list->head);
    // find ordered position for newNode or end of list
    while (cursorNode->next != &(list->head)) {
        cursorNode = cursorNode->next;
        // position found if cursor's element is > to newNode's element
        if (compare(cursorNode->element, newNode->element) > 0) {
            cursorNode = cursorNode->previous; // Adjust cursor since instruction will be addAfter
            break;
        }
    }

    linkedList_addAfter(list, newNode, cursorNode);
}

void linkedList_remove(struct linkedList_node * node) {
    assert(node != NULL);
    struct linkedList * list = node->list;
    struct linkedList_node * predecessor = node->previous;
    struct linkedList_node * successor = node->next;

    predecessor->next = successor;
    successor->previous = predecessor;

    node->list = NULL;
    node->previous = NULL;
    node->next = NULL;

    list->length--;
}


