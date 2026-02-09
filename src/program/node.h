#ifndef  NODE_H 
# define NODE_H


# include <stdlib.h> // calloc free

typedef struct  {
  char*  name;
  int    type;
  size_t fsize;
} t_node_data;

typedef struct s_node {
  struct s_node* next;
  struct s_node* child;
  t_node_data    data;
} t_node;

size_t       getId(int mode);
int          freeNode(t_node** list);
t_node*      makeNodeLast(char* name, int type, t_node** list);
size_t       getNodeLen(t_node* head);
void*        dellNode(t_node* n);

#endif