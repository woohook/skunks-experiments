#include <stdlib.h>
#include "list.h"

struct _list_item
{
  struct _list* parent;
  struct _list_item* previous;
  struct _list_item* next;
  void* value;
};

struct _list
{
  int item_count;
  struct _list_item* first;
  struct _list_item* last;
};

struct _list* list_create()
{
  struct _list* list = (struct _list*)malloc(sizeof(struct _list));

  list->item_count = 0;
  list->first = 0;
  list->last = 0;

  return list;
}

void list_release(struct _list* list)
{
  struct _list_item* item = list->first;
  for(int i=0; i < list->item_count; i++)
  {
    free(item);
  }
  free(list);
}

int list_get_size(struct _list* list)
{
  return list->item_count;
}

void list_add_value(struct _list* list, void* value)
{
  struct _list_item* last = list->last;

  struct _list_item* new_item = (struct _list_item*)malloc(sizeof(struct _list_item));

  new_item->previous = last;
  new_item->next = 0;
  new_item->value = value;

  if(list->item_count > 0)
  {
    last->next = new_item;
  }

  if(list->item_count == 0)
  {
    list->first = new_item;
  }
  list->last = new_item;
  list->item_count++;
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

struct _list_item* list_get_last(struct _list* list)
{
  struct _list_item* item = 0;

  if(list->item_count > 0)
  {
    item = list->last;
  }

  return item;
}

struct _list_item* list_item_get_next(struct _list_item* item)
{
  return item->next;
}

void* list_item_get_value(struct _list_item* item)
{
  return item->value;
}

void* list_get_value(struct _list* list, int item_index)
{
  struct _list_item* next_item = list->first;

  while(item_index > 0)
  {
    next_item = next_item->next;
    item_index--;
  }

  return list_item_get_value(next_item);
}
