#ifndef  __FLAGS__
# define __FLAGS__

# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>

# define FLAG_NAME_LEN 100

typedef struct s_flagValue {
  struct s_flagValue* next;
  int32_t             flag;
  char                name[FLAG_NAME_LEN];
  char*               value;
} t_flagValue;


int          fv_add_last(t_flagValue** list, int flag, const char* value);
int          fv_free(t_flagValue** list);
int          fv_set_name(t_flagValue* node, const char* name);
//
void         fv_print(t_flagValue* list);
//
bool         fv_is_flag(t_flagValue* list, int32_t flag);
bool         fv_is_flag_name(t_flagValue* list, char name[FLAG_NAME_LEN]);

const char*  fv_get_value(t_flagValue* list, int32_t flag);
const char*  fv_get_value_name(t_flagValue* list, char name[FLAG_NAME_LEN]);

#endif