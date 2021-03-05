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
