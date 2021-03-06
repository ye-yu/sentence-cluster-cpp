#ifndef CARGS_H
#define CARGS_H
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  char short_name;
  char *long_name;
  int value;
} flag_arg;

typedef struct {
  char short_name;
  char *long_name;
  char *value;
} str_arg;

extern flag_arg *define_flag_arg(char short_name, const char *long_name, int def_val);

extern str_arg *define_str_arg(char short_name, const char *long_name, const char *def_val);

extern char **get_args_array();

extern int process_args(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
