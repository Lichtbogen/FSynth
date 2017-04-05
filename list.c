/*
 * Copyright (c) 2017 Pierre Biermann
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @brief Generic double linked list with hash function support
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#include <stdlib.h>
#include <memory.h>

#include "list.h"

struct NodeItem *push_back(struct NodeList *list, void* data, size_t data_size)
{
  struct NodeItem *node = (struct NodeItem*) malloc(sizeof(struct NodeItem));
  node->hash = 0;
  node->prev = list->tail;
  node->next = NULL;
  if (list->head == NULL) {
    list->head = node;
  }
  if (list->tail != NULL) {
    list->tail->next = node;
  }
  list->tail = node;
  if (data_size > 0) {
    node->data = malloc(data_size);
    memcpy(node->data, data, data_size);
  } else {
    node->data = data; /* Store only the pointer, if data_size is 0 */
  }
  node->data_size = data_size;
  return node;
}

struct NodeItem *push_back_hashed(struct NodeList *list, void* data, size_t data_size)
{
  struct NodeItem *node = push_back(list, data, data_size);
  node->hash = HASH(data, data_size);
  return node;
}

void delete_item(struct NodeList *list, struct NodeItem *item)
{
  if (item != NULL) {
    if (item->prev != NULL) {
      item->prev->next = item->next;
    }
    if (item->next != NULL) {
      item->next->prev = item->prev;
    }
    if (list->head == item) {
      list->head = item->next;
    }
    if (list->tail == item) {
      list->tail = item->prev;
    }
    if (item->data_size > 0)
      free(item->data);
    free(item);
  }
}

struct NodeItem *insert_item(struct NodeList *list, struct NodeItem *item, void* data, size_t data_size)
{
  struct NodeItem *node = (struct NodeItem*) malloc(sizeof(struct NodeItem));
  if (item->next != NULL) {
    item->next->prev = node;
    node->next = item->next;
  } else {
    node->next = NULL;
    list->tail = node;
  }
  node->hash = 0;
  node->prev = item;
  item->next = node;
  if (data_size > 0) {
    node->data = malloc(data_size);
    memcpy(node->data, data, data_size);
  } else {
    node->data = data; /* Store only the pointer, if data_size is 0 */
  }
  node->data_size = data_size;
  return node;
}

struct NodeItem *insert_item_hashed(struct NodeList *list, struct NodeItem *item, void* data, size_t data_size)
{
  struct NodeItem *node = insert_item(list, item, data, data_size);
  node->hash = HASH(data, data_size);
  return node;
}

struct NodeItem *find_item(struct NodeList *list, hash_t hash)
{
  struct NodeItem *result = NULL, *item = list->head;
  while (item != NULL) {
    if (item->hash == hash) {
      result = item;
      break;
    }
    item = item->next;
  }
  return result;
}

void delete_list(struct NodeList *list)
{
  struct NodeItem *temp, *node = list->tail;
  while (node != NULL) {
    temp = node->prev;
    if (node->data_size > 0)
      free(node->data);
    free(node);
    node = temp;
  }
}

hash_t hash_sdbm(hash_t hv, const char *data, size_t data_size)
{
  size_t i;
  for (i = 0; i < data_size; ++i) {
    hv = data[i] + (hv << 6) + (hv << 16) - hv;
  }
  return hv;
}

hash_t hash_djb2(hash_t hv, const char *data, size_t data_size)
{
  size_t i;
  for (i = 0; i < data_size; ++i) {
    hv = ((hv << 5) + hv) + data[i];
  }
  return hv;
}
