#include <stdlib.h>
#include "list.h"

struct _list_item
{
  struct _list* parent;
  struct _list_item* previous;
  struct _list_item* next;
  void* item;
};

struct _list
{
  int item_count;
  struct _list_item* first;
  struct _list_item* last;
};

void list_initialize(struct _list* list)
{
  list->item_count = 0;
  list->first = 0;
  list->last = 0;
}

void list_add(struct _list* list, void* item)
{
  struct _list_item* last = list->last;

  struct _list_item* new_item = (struct _list_item*)malloc(sizeof(struct _list_item));

  new_item->previous = last;
  new_item->next = 0;
  new_item->item = item;

  last->next = new_item;
  list->last = new_item;
}

struct _list_item* list_get_first(struct _list* list)
{
  struct _list_item* item = 0;

  if(list->item_count > 0)
  {
    item = list->first;
  }

  return item;
}

struct _list_item* list_get_next(struct _list_item* item)
{
  return item->next;
}

void* list_get_item(struct _list_item* item)
{
  return item->item;
}
