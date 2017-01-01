/*
 * linkedList.h
 *
 *  Created on: Jul 15, 2016
 *      Author: math
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

/*
 * embed inside a custom struct to contain the element pointer and the next/previous pointers.
 */
struct linkedList_node {
    void * element;
    struct linkedList * list;
    struct linkedList_node * next;
    struct linkedList_node * previous;
};

struct linkedList {
    int length;
    struct linkedList_node head;
};

void linkedList_initList(struct linkedList * list);
void linkedList_initNode(struct linkedList_node * node, void * element);
void linkedList_addAfter(struct linkedList * list, struct linkedList_node * newNode,
        struct linkedList_node * position);
void linkedList_addBefore(struct linkedList * list, struct linkedList_node * newNode,
        struct linkedList_node * position);
void linkedList_addOrdered(struct linkedList * list, struct linkedList_node * newNode,
        int (*compare)(void *, void *));
void linkedList_remove(struct linkedList_node * node);

#endif /* LINKEDLIST_H_ */
