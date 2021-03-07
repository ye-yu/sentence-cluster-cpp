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
#include <stdlib.h>
#include <stdio.h>
#include "lancaster.h"
#include "string.h"

int main(int argc, char **argv) {
  puts("Stemming command line arguments...");
  for(int i = 1; i < argc; ++i) {
    char *buffer = malloc(sizeof(char) * strlen(argv[i]));
    lancaster_stemmer_c(buffer, argv[i]);
    printf("%s -> %s\n", argv[i], buffer);
    free(buffer);
  }
}
