#include <stdint.h>
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
    struct list_head *tmp =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!tmp) {
        return NULL;
    }
    INIT_LIST_HEAD(tmp);
    return tmp;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        list_del_init(&entry->list);
        q_release_element(entry);
    }
    free(l);
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
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    int length = strlen(s);
    node->value = (char *) malloc(length + 1);
    if (!node->value) {
        q_release_element(node);
        return false;
    }
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
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    int length = strlen(s);
    node->value = (char *) malloc(length + 1);
    if (!node->value) {
        q_release_element(node);
        return false;
    }
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
    element_t *node = list_first_entry(head, element_t, list);
    list_del_init(head->next);
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
    element_t *node = list_last_entry(head, element_t, list);
    list_del_init(head->prev);
    memset(sp, '\0', bufsize);
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
    if (!head || list_empty(head)) {
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
    int size = q_size(head) / 2;

    while (size--) {
        node = node->next;
    }

    list_del_init(node);
    q_release_element(list_entry(node, element_t, list));
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
    if (!head) {
        return false;
    }
    struct list_head *node = head->next;
    while (node->next != head) {
        element_t *tmp = list_entry(node->next, element_t, list);
        if (strcmp(list_entry(node, element_t, list)->value, tmp->value) == 0) {
            list_del_init(node->next);
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
    while (node->next != head && node != head) {
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
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    struct list_head *node;
    list_for_each (node, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
}

struct list_head *mergetwoqueues(struct list_head *left,
                                 struct list_head *right)
{
    struct list_head *head = NULL;
    struct list_head **ptr = &head, **node = NULL;

    for (; left && right; *node = (*node)->next) {
        node = (strcmp(list_entry(left, element_t, list)->value,
                       list_entry(right, element_t, list)->value) < 0)
                   ? &left
                   : &right;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);
    return head;
}

struct list_head *mergesort_list(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }

    struct list_head *slow = head, *right;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;

    right = slow->next;
    slow->next = NULL;

    head = mergesort_list(head);
    right = mergesort_list(right);
    return mergetwoqueues(head, right);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *node = head->next;
    head->prev->next = NULL;
    node = mergesort_list(node);

    head->next = node;
    for (node = head; node->next; node = node->next) {
        node->next->prev = node;
    }
    node->next = head;
    head->prev = node;
}
