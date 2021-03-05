#ifndef LANCASTER_H
#define LANCASTER_H

#ifdef  __cplusplus
#include "string"

extern std::string lancaster_stemmer(std::string value);

extern "C" {
#endif
void lancaster_stemmer_c(char *dest, const char *source);
#ifdef  __cplusplus
}
#endif
#endif
