#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>


int         construct_url           (char *buf, size_t buflen,
                                     char const *input,
                                     char const *platform);
int         construct_path          (char *buf, size_t buflen, char const *home,
                                     char const *input, char const *platform);
int         parse_tldrpage          (char const *input);
int         print_tldrpage          (char const *input, char const *platform);
int         print_localpage         (char const *path);

#endif /* PARSER_H */
