#include "node.h"
#include <string.h>



static t_node* makeNode(char* name, int type) {
  if (!name)
    return NULL;
  t_node* node = NULL;
  node = calloc(1, sizeof(*node));
  if (!node)
    return NULL;
  static size_t id = 0;
  node->data.name = strdup(name);
  node->data.type = type;
  node->data.id = ++id;
  return node;
}


// -1 missing name
// -2 no list provide
// -3 faild to make a new node
// else return len of list
t_node* makeNodeLast(char* name, int type, t_node** list) {
  if (!list) // look for list first 
    return NULL;
  if (!name)
    return NULL;
  int size = 0;
  t_node* head = *list;
  t_node* made = NULL;
  while (head) {
    size++;
    if (!head->next)
      break ;
    head = head->next;
  }
  // make head
  if (!size) {
    *list = makeNode(name, type);
    if (!(*list))
      return NULL;
    made = *list;
  }
  else {
    // add tale
    head->next = makeNode(name, type);
    if (!head->next)
      return NULL;
    made = head->next;
  }
  return made;
}


int freeNode(t_node** list) {
  if (!list) {
    return -1;
  }
  int size = 0;
  t_node* head = *list;
  while (head) {
    if (head->child) {
      size += freeNode(&head->child);
    }
    t_node* next = head->next;
    free(head->data.name);
    free(head);
    head = next;
    size++;
  }
  *list = NULL;
  return size;
}

size_t  getNodeLen(t_node* head) {
  size_t len = 0;
  //
  if (!head)
    return 0;
  for (; head; head = head->next) { len++; }
  return len;
}

void* dellNode(t_node* n) {
  free(n->data.name);
  free(n);
  return NULL;
}

int deledEmty(t_node** list) {
  if (!list && !*list) {
    return 0;
  }
  int dell = 0;
  HEADDELL: // first time using it
  while (*list && IS_FOLDER_EMPTY(((t_node*)*list))) {
    t_node* tmp = (*list)->next;
    t_node* tooFree = *list;
    dellNode(tooFree);
    *list = tmp;
    dell++;
  }
  t_node* tmp = *list;
  while (tmp) {
    if (tmp->next && IS_FOLDER_EMPTY(tmp->next)) {
      t_node* tooFree = tmp->next;
      t_node* next = tooFree->next;
      tmp->next = next;
      dellNode(tooFree);
      tmp = *list;
      dell++;
      continue ;
    }
    if (tmp->data.type == folder) {
      tmp->data.fsize -= deledEmty(&tmp->child);
      if (tmp->data.fsize == 0) {
        goto HEADDELL;
      }
    }
    tmp = tmp->next;
  }
  return dell;
}

static bool isSwap(t_node* n) {
  if (n && n->next) {
    if (n->data.type != folder && n->next->data.type == folder)
      return true;
  }
  return false;
}

static void swapData(t_node* n) {
  t_node_data tmp = n->data;
  n->data = n->next->data;
  n->next->data = tmp;
  n->child = n->next->child;
  n->next->child = NULL;
}

void moveFolderUp(t_node** list) {
  t_node* tmp = *list;
  while (tmp) {
    if (isSwap(tmp)) {
      swapData(tmp);
      tmp = *list;
      continue ;
    }
    if (tmp->data.type == folder) {
      moveFolderUp(&tmp->child);
    }
    tmp = tmp->next;
  }
}