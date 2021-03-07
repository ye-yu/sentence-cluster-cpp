/**
  * YeYu Simple English Sentence Clustering Tool
  * Copyright (C) 2020 Ye-Yu rafolwen98@gmail.com
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <string.h>
#include <stdlib.h>
#include "cargs.h"
#define ALLOC_SIZE 8

typedef enum {
  MIXED_ARG,
  UNDEFINED_ARG,
  STR_ARG_SHORTS,
  STR_ARG,
  FLAG_ARG_SHORTS,
  FLAG_ARG_LONG
} combination_type;

static flag_arg *flag_args = NULL;
static int flag_arg_len = 0;
static int flag_arg_allocated = 0;

static str_arg *str_args = NULL;
static int str_arg_len = 0;
static int str_arg_allocated = 0;

static char** array_args = NULL;
static int array_arg_len = 0;
static int array_arg_allocated = 0;

static char* error[2] = { "", "" };

flag_arg *define_flag_arg(char short_name, const char *long_name, int def_val) {
  if (!flag_arg_allocated) {
    flag_arg_allocated = ALLOC_SIZE;
    flag_args = malloc(sizeof(flag_arg) * flag_arg_allocated);
  } else if ((flag_arg_len + 1.0) / flag_arg_allocated > 0.75) {
    flag_arg *temp = flag_args;
    flag_arg_allocated *= 2;
    flag_args = malloc(sizeof(flag_arg) * flag_arg_allocated);
    if (flag_args == NULL) return NULL;
    for (size_t i = 0; i < flag_arg_allocated; i++)
    {
      if (i < flag_arg_len) {
        flag_args[i] = temp[i];
      } else {
        flag_args[i].short_name = 0;
        flag_args[i].long_name = NULL;
        flag_args[i].value = 0;
      }
    }
  }
  flag_args[flag_arg_len].short_name = short_name;

  const int lname_len = (strlen(long_name == NULL ? "" : long_name) + 1);
  char *lname_buf = malloc(sizeof(char *) * lname_len);
  for (size_t i = 0; i < lname_len; i++) lname_buf[i] = '\0';
  strcpy(lname_buf, long_name == NULL ? "" : long_name);

  flag_args[flag_arg_len].long_name = lname_buf;
  flag_args[flag_arg_len].value = def_val;
  flag_arg_len += 1;
  return flag_args + flag_arg_len - 1;
}

str_arg *define_str_arg(char short_name, const char *long_name, const char *def_val) {
  if (!str_arg_allocated) {
    str_arg_allocated = ALLOC_SIZE;
    str_args = malloc(sizeof(str_arg) * str_arg_allocated);
  } else if ((str_arg_len + 1.0) / str_arg_allocated > 0.75) {
    str_arg *temp = str_args;
    str_arg_allocated *= 2;
    str_args = malloc(sizeof(str_arg) * str_arg_allocated);
    if (str_args == NULL) return NULL;
    for (size_t i = 0; i < str_arg_allocated; i++)
    {
      if (i < str_arg_len) {
        str_args[i] = temp[i];
      } else {
        str_args[i].short_name = 0;
        str_args[i].long_name = NULL;
        str_args[i].value = 0;
      }
    }
  }
  str_args[str_arg_len].short_name = short_name;
  {
    const int lname_len = (strlen(long_name == NULL ? "" : long_name) + 1);
    char *lname_buf = malloc(sizeof(char *) * lname_len);
    for (size_t i = 0; i < lname_len; i++) lname_buf[i] = '\0';
    strcpy(lname_buf, long_name == NULL ? "" : long_name);
    
    str_args[str_arg_len].long_name = lname_buf;
  }

  {
    const int dval_len = (strlen(def_val == NULL ? "" : def_val) + 1);
    char *dval_buf = malloc(sizeof(char *) * dval_len);
    for (size_t i = 0; i < dval_len; i++) dval_buf[i] = '\0';
    strcpy(dval_buf, def_val == NULL ? "" : def_val);
    
    str_args[str_arg_len].value = dval_buf;
  }
  str_arg_len += 1;
  return str_args + str_arg_len - 1;
}

/* >>> all linear searches >>> */

flag_arg* find_bool_rule_short(char c) {
  for (size_t i = 0; i < flag_arg_len; i++) if (flag_args[i].short_name == c) return flag_args + i;
  return NULL;
}

flag_arg* find_bool_rule_long(char *c) {
  for (size_t i = 0; i < flag_arg_len; i++) {
    if (flag_args[i].long_name == NULL) continue;
    if (!strcmp(c, flag_args[i].long_name)) 
      return flag_args + i;
  }
  return NULL;
}

str_arg* find_str_rule_short(char c) {
  for (size_t i = 0; i < str_arg_len; i++) if (str_args[i].short_name == c) return str_args + i;
  return NULL;
}

str_arg* find_str_rule_long(char *c) {
  for (size_t i = 0; i < str_arg_len; i++) {
    if (str_args[i].long_name == NULL) continue;
    if (!strcmp(c, str_args[i].long_name)) return str_args + i;
  }
  return NULL;
}

/* <<< all linear searches <<< */

int determine_combinations(char *arg, flag_arg **f_arg_buf, str_arg **s_arg_buf) {
  if (arg + 1 == NULL) {
    #ifdef DEBUG
    printf("Character after - is empty: %s\n", arg);
    #endif
    return UNDEFINED_ARG;
  }
  if (*(arg + 1) == '-') {
    if (!*(arg + 2)) { 
      #ifdef DEBUG
      printf("Character after -- is empty: %s\n", arg);
      #endif

      return UNDEFINED_ARG;
    }
    char *param = arg + 2;

    #ifdef DEBUG
    printf("Finding match for: %s\n", param);
    #endif

    flag_arg * f_arg_match = find_bool_rule_long(param);
    if (f_arg_match != NULL) {
      *f_arg_buf = f_arg_match;
      return FLAG_ARG_LONG;
    }
    #ifdef DEBUG
    printf("No bool rule for: %s\n", param);
    #endif

    str_arg * s_arg_match = find_str_rule_long(param);
    if (s_arg_match != NULL) {
      *s_arg_buf = s_arg_match;
      return STR_ARG;
    }

    #ifdef DEBUG
    printf("No str rule for: %s\n", param);
    #endif

    return UNDEFINED_ARG;
  }

  int has_flag = 0;
  int has_str = 0;

  int i = 1;
  while(*(arg + i)) {
    char c = *(arg + i);
    i += 1;
    flag_arg * f_arg_match = find_bool_rule_short(c);
    if (f_arg_match != NULL) {
      has_flag++;
      *f_arg_buf = f_arg_match;
      // simultaneously enable switch
      f_arg_match->value=1;

      #ifdef DEBUG
      printf("Char %c matched a bool flag. Number of flags now: %d\n", c, has_flag);
      #endif

      continue;
    }

    if (find_str_rule_short(c) != NULL) {
      has_str++;
      continue;
    }

    #ifdef DEBUG
    printf("Cannot find switch for char: %c - i: %d\n", c, i);
    #endif

    return UNDEFINED_ARG;
  }

  if (has_flag && has_str) return MIXED_ARG;

  if (has_str) {
    if (has_str > 1) return STR_ARG_SHORTS;

    str_arg *s_arg_match = find_str_rule_short(*(arg + 1));
    if (s_arg_match != NULL) {
      *s_arg_buf = s_arg_match;
      return STR_ARG;
    }
    return UNDEFINED_ARG;
  }

  /*
    Case when "./app -a false"
    then has flag == 1, treat "false" to unset -a

    Case when "./app -abc false"
    then has flag > 1, treat "false" as array arg
  */
  if (has_flag > 1) {
    *f_arg_buf = NULL;
  }

  #ifdef DEBUG
  else {
    printf("Listening to false keyword on next for flag: %c\n", (*f_arg_buf) -> short_name);
  }
  #endif

  return FLAG_ARG_SHORTS;
}

int push_array_arg(char *str) {
  if (!array_arg_allocated) {
    array_arg_allocated = ALLOC_SIZE;
    array_args = malloc(sizeof(char*) * array_arg_allocated);
  } else if ((array_arg_len + 1.0) / array_arg_allocated > 0.75) {
    char **temp = array_args;
    array_arg_allocated *= 2;
    array_args = malloc(sizeof(char*) * array_arg_allocated);
    if (array_args == NULL) return 1;
    for (size_t i = 0; i < array_arg_allocated; i++)
    {
      if (i < array_arg_len) {
        array_args[i] = temp[i];
      } else {
        array_args[i] = NULL;
      }
    }
  }
  array_args[array_arg_len++] = str;
  return 0;
}

char **get_args_array() {
  return array_args;
}

int process_args(int argc, char **argv) {
  str_arg *str_arg_target = NULL;
  flag_arg *flg_arg_target = NULL;
  for (size_t i = 0; i < argc; i++)
  {
    char *arg = argv[i];
    if (flg_arg_target != NULL) {
      if (!strcmp(arg, "false")) {
        #ifdef DEBUG
        printf("Previous flag to unset: %c\n", flg_arg_target->short_name);
        #endif

        flg_arg_target->value=0;
        flg_arg_target = NULL;
        continue;
      }
    }
    if (str_arg_target == NULL) {
      if (arg[0] != '-') {
        push_array_arg(arg);
        continue;
      }

      int combination_type = determine_combinations(arg, &flg_arg_target, &str_arg_target);
      switch (combination_type)
      {
      case MIXED_ARG:
        error[0] = "Illegal combination - Flags and string args must be separated";
        error[1] = arg;
        return 1;
        break;

      case UNDEFINED_ARG:
        error[0] = "Undefined flag";
        error[1] = arg;
        return 1;

      case STR_ARG_SHORTS:
        error[0] = "Cannot define multiple string args!";
        error[1] = arg;
        return 1;
      
      default:
        break;
      }
    } else {
      str_arg_target->value=arg;
      str_arg_target = NULL;
    }
  }
  return 0;
}

void free_args() {
  if (flag_args != NULL) {
    for (size_t i = 0; i < flag_arg_len; i++) free(flag_args[i].long_name);
    free(flag_args);
    flag_arg_len = 0;
    flag_arg_allocated = 0;
  }
  if (str_args != NULL) {
    for (size_t i = 0; i < str_arg_len; i++) free(str_args[i].long_name);
    free(str_args);
    str_arg_len = 0;
    str_arg_allocated = 0;
  }
}

char **get_arg_error_msgs() {
  return error;
}
