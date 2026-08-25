# include "flags.h"
# include "utils.h"
# include <string.h>
# include <strings.h>


static t_flagValue* fv_add(int32_t flag, const char* value) {
  t_flagValue* f = calloc(1, sizeof(*f));
  if (f) {
    f->flag = flag;
    memset(f->name, 0, FLAG_NAME_LEN * sizeof(char));
    f->value = d__strdup(value);
    if (!f->value) {
      perror("calloc");
      free(f);
      f = NULL;
    }
  } else {
    perror("calloc");
    f = NULL;
  }
  return f;
}

int fv_add_last(t_flagValue** list, int32_t flag, const char* value) {
  if (!list)
    return -1;
  if (!(*list)) {
    *list = fv_add(flag, value);
    if (!(*list))
      return -2;
  }
  else {
    t_flagValue *tmp = (*list);
    for (; tmp && tmp->next; tmp = tmp->next) { }
    tmp->next = fv_add(flag, value);
    if (!tmp->next)
      return -2;
  }
  return 0;
}

int fv_free(t_flagValue** list) {
  if (!list)
    return -1;
  for (t_flagValue* tmp = (*list); tmp; ) {
    t_flagValue *t = tmp->next;
    free(tmp->value);
    free(tmp);
    tmp = t;
  }
  return 0;
}

int fv_set_name(t_flagValue* node, const char* name) {
  if (!node)
    return 1;
  const size_t l = strlen(name);
  memcpy(node->name, name, l > FLAG_NAME_LEN - 1 ? FLAG_NAME_LEN -1 : l);
  node->name[FLAG_NAME_LEN - 1] = 0;
  return 0;
}

bool fv_is_flag(t_flagValue* list, int32_t flag) {
  for (; list; list = list->next) {
    if (list->flag == flag) {
      return true;
    }
  }
  return false;
}

bool fv_is_flag_name(t_flagValue* list, char name[FLAG_NAME_LEN]) {
  for (; list; list = list->next) {
    if (strcmp(name, list->name) == 0) {
      return true;
    }
  }
  return false;
}

const char* fv_get_value(t_flagValue* list, int32_t flag) {
  for (; list; list = list->next) {
    if (list->flag == flag) {
      return list->value;
    }
  }
  return NULL;
}

const char* fv_get_value_name(t_flagValue* list, char name[FLAG_NAME_LEN]) {
  for (; list; list = list->next) {
    if (strcmp(name, list->name)) {
      return list->value;
    }
  }
  return NULL;
}

# include <stdio.h>
void fv_print(t_flagValue* list) {
  for ( ; list; list = list->next) {
    printf("[%d]%s\n", list->flag, list->value);
  }
}