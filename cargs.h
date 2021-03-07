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

extern void free_args();

extern char **get_arg_error_msgs();
#ifdef __cplusplus
}
#endif
#endif
