/*
 * Copyright (c) 2018 Pierre Biermann
 *
 * Permission is hereby granted and free of charge.
 * Published under the terms of the MIT license, see LICENSE file for further information.
 */

/**
 * @brief Generic double linked list with hash function support
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#ifndef _LIST_H_
#define _LIST_H_

#ifndef HASH
#define HASH(data, size) hash_sdbm(0, data, data_size)
#endif

typedef unsigned int hash_t;

struct NodeList {
  struct NodeItem *head;
  struct NodeItem *tail;
};

struct NodeItem {
  hash_t hash;
  void* data;
  size_t data_size;
  struct NodeItem *prev;
  struct NodeItem *next;
};

struct NodeItem *push_back(struct NodeList *list, void* data, size_t data_size);
struct NodeItem *insert_item(struct NodeList *list, struct NodeItem *item, void* data, size_t data_size);
struct NodeItem *push_back_hashed(struct NodeList *list, void* data, size_t data_size);
struct NodeItem *insert_item_hashed(struct NodeList *list, struct NodeItem *item, void* data, size_t data_size);
struct NodeItem *find_item(struct NodeList *list, hash_t hash);
void delete_list(struct NodeList *list);
void delete_item(struct NodeList *list, struct NodeItem *item);
hash_t hash_sdbm(hash_t hv, const char *data, size_t data_size);
hash_t hash_djb2(hash_t hv, const char *data, size_t data_size);

#endif /* _LIST_H_ */
