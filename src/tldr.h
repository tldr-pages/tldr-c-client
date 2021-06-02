/*
 * tldr.h
 *
 * Copyright (C) 2016 Arvid Gerstmann
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef TLDR_H
#define TLDR_H

#include <stdlib.h>

#define STRBUFSIZ 512
#define URLBUFSIZ 1024

#define BASE_URL "https://raw.github.com/tldr-pages/tldr/main/pages"
#define BASE_URL_LEN (sizeof(BASE_URL) - 1)

#define ZIP_URL "https://github.com/tldr-pages/tldr/archive/main.zip"
#define ZIP_URL_LEN (sizeof(ZIP_URL_LEN) - 1)

#define TMP_DIR "/tmp/tldrXXXXXX"
#define TMP_DIR_LEN (sizeof(TMP_DIR) - 1)

#define TMP_FILE "/main.zip"
#define TMP_FILE_LEN (sizeof(TMP_FILE) - 1)

#define TLDR_DIR "/tldr"
#define TLDR_DIR_LEN (sizeof(TLDR_DIR) - 1)

#define TLDR_HOME "/.tldrc"
#define TLDR_HOME_LEN (sizeof(TLDR_HOME) - 1)

#define TLDR_DATE "/.tldrc/date"
#define TLDR_DATE_LEN (sizeof(TLDR_DATE) - 1)

#define TLDR_EXT "/.tldrc/tldr/pages/"
#define TLDR_EXT_LEN (sizeof(TLDR_EXT) - 1)

#define ANSI_COLOR_RESET_FG                     "\x1b[39m"
#define ANSI_COLOR_TITLE_FG                     "\x1b[39m"
#define ANSI_COLOR_EXPLANATION_FG               "\x1b[39m"
#define ANSI_COLOR_COMMENT_FG                   "\x1b[32m"
#define ANSI_COLOR_CODE_FG                      "\x1b[31m"
#define ANSI_COLOR_CODE_PLACEHOLDER_FG          "\x1b[34m"
#define ANSI_BOLD_ON                            "\x1b[1m"
#define ANSI_BOLD_OFF                           "\x1b[22m"

/* local.c */
long        check_localdate         (void);
int         update_localdate        (void);
int         has_localdb             (void);
int         update_localdb          (int verbose);
int         clear_localdb           (int verbose);
int         get_file_content        (char const *path, char **out, int verbose);

/* net.c */
int         download_file           (char const *url, char const *outfile,
                                     int verbose);
int         download_content        (char const *url, char **out, int verbose);

/* parser.c */
int         construct_url           (char *buf, size_t buflen,
                                     char const *input,
                                     char const *platform);
int         construct_path          (char *buf, size_t buflen, char const *home,
                                     char const *input, char const *platform);
int         parse_tldrpage          (char const *input);
int         print_tldrpage          (char const *input, char const *platform);
int         print_tldrlist          (char const *platform);
int         parse_tldrlist          (char const *path, char const *platform);
int         print_localpage         (char const *path);

/* utils.c */
#define RMOPT_IGNORE_NOFILE     (0x1)

double      rround              (double arg);
int         rm                  (char const *path, int options);
int         unzip               (char const *path, char const *outpath);
char const *gethome             (void);
char const *getplatform         (void);
int         sstrncat            (char *dest, size_t *pos, size_t max,
                                 char const *src, size_t len);
#endif /* TLDR_H */
