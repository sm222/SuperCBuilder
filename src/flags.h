#ifndef __FLAGS__
# define __FLAGS__

enum valueArg {
  e_bool,
  e_next,
  e_equal,
};

# define FLAG_NAME_LEN 100

typedef struct s_flagValue {
  struct s_flagValue* next;
  int                 flag;
  char                name[FLAG_NAME_LEN];
  char*               value;
} t_flagValue;


int          fv_add_last(t_flagValue** list, int flag, const char* value);
int          fv_free(t_flagValue** list);
int          fv_set_name(t_flagValue* node, const char* name);
void         fv_print(t_flagValue* list);

#endif