#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>

#define RMOPT_IGNORE_NOFILE     (0x1)

double      rround              (double arg);
int         rm                  (char const *path, int options);
int         unzip               (char const *path, char const *outpath);
char const *gethome             (void);
int         sstrncat            (char *dest, size_t *pos, size_t max,
                                 char const *src, size_t len);

#endif /* UTILITIES_H */
