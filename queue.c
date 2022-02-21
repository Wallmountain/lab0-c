#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    element_t *tmp = (element_t *) malloc(sizeof(element_t));
    if (!tmp) {
        return NULL;
    }
    tmp->value = NULL;
    INIT_LIST_HEAD(&tmp->list);
    return &tmp->list;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    element_t *node;
    while (!list_empty(l)) {
        list_del(l);
        node = list_entry(l, element_t, list);
        l = l->next;
        q_release_element(node);
    }
    node = list_entry(l, element_t, list);
    q_release_element(node);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    int length = strlen(s);
    element_t *node = (element_t *) malloc(sizeof(element_t));
    node->value = (char *) malloc(length + 1);
    for (int i = 0; i < length; i++) {
        node->value[i] = s[i];
    }
    node->value[length] = '\0';
    list_add(&node->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    int length = strlen(s);
    element_t *node = (element_t *) malloc(sizeof(element_t));
    node->value = (char *) malloc(length + 1);
    for (int i = 0; i < length; i++) {
        node->value[i] = s[i];
    }
    node->value[length] = '\0';
    list_add_tail(&node->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp) {
        return NULL;
    }
    struct list_head *tmp = head->next;
    list_del(head->next);
    element_t *node = list_entry(tmp, element_t, list);
    memset(sp, '\0', bufsize);
    strncpy(sp, node->value, bufsize - 1);
    return node;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp) {
        return NULL;
    }
    struct list_head *tmp = head->prev;
    list_del(head->prev);
    element_t *node = list_entry(tmp, element_t, list);
    strncpy(sp, node->value, bufsize - 1);
    return node;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }
    struct list_head *node;
    int size = 0;
    list_for_each (node, head) {
        size++;
    }
    return size;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
// https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *node = head->next;
    int size = q_size(head);
    size = size / 2;

    while (size--) {
        node = node->next;
    }

    list_del(node);
    element_t *del_node = list_entry(node, element_t, list);
    q_release_element(del_node);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *node = head->next;
    while (node->next != head) {
        char *fstr = list_entry(node, element_t, list)->value;
        element_t *tmp = list_entry(node->next, element_t, list);
        if (strcmp(fstr, tmp->value) == 0) {
            list_del(node->next);
            q_release_element(tmp);
        } else {
            node = node->next;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
// https://leetcode.com/problems/swap-nodes-in-pairs/
void q_swap(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *node = head->next;
    while (node->next != head) {
        // swap
        node->next->prev = node->prev;
        node->prev->next = node->next;
        node->next->next->prev = node;
        node->prev = node->next;
        node->next = node->next->next;
        node->prev->next = node;

        node = node->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *node = head;
    while (node->next != head) {
        struct list_head *tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;

        node = node->prev;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || head->next == head->prev) {
        return;
    }
    struct list_head *left, *right, *pivot;
    left = q_new();
    right = q_new();

    pivot = head->next;
    list_del(pivot);

    // partition
    while (!list_empty(head)) {
        struct list_head *tmp, *t_head;
        tmp = head->next;
        list_del(tmp);
        t_head = (strcmp(list_entry(pivot, element_t, list)->value,
                         list_entry(tmp, element_t, list)->value) < 0)
                     ? right
                     : left;

        list_add_tail(tmp, t_head);
    }

    q_sort(left);
    q_sort(right);

    list_add_tail(pivot, left);
    list_splice_tail(right, left);
    q_release_element(list_entry(right, element_t, list));
}
