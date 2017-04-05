/*
 * Copyright (c) 2017 Pierre Biermann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
