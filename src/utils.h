#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>

double      rround             (double arg);
int         rm                 (char const *path);
int         unzip              (char const *path, char const *outpath);
char const *gethome            (void);
int         sstrncat           (char *dest, size_t *pos, size_t max,
                                char const *src, size_t len);
int         copyfile           (const char *src, const char *dest, mode_t perms);
int         copytree           (char const *src, char const *dest);

#endif /* UTILITIES_H */
