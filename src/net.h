#ifndef NET_H
#define NET_H

int         download_file           (char const *url, char const *outfile,
                                     int verbose);
int         download_content        (char const *url, char **out, int verbose);

#endif /* NET_H */
