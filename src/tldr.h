#ifndef TLDR_H
#define TLDR_H

#define STRBUFSIZ 512
#define URLBUFSIZ 1024

#define BASE_URL "https://raw.github.com/tldr-pages/tldr/master/pages"
#define BASE_URL_LEN (sizeof(BASE_URL) - 1)

#define ZIP_URL "https://github.com/tldr-pages/tldr/archive/master.zip"
#define ZIP_URL_LEN (sizeof(ZIP_URL_LEN) - 1)

#define TMP_DIR "/tmp/tldrXXXXXX"
#define TMP_DIR_LEN (sizeof(TMP_DIR) - 1)

#define TMP_FILE "/master.zip"
#define TMP_FILE_LEN (sizeof(TMP_FILE) - 1)

#define TLDR_DIR "/tldr-master"
#define TLDR_DIR_LEN (sizeof(TLDR_DIR) - 1)

#define TLDR_HOME "/.tldrc"
#define TLDR_HOME_LEN (sizeof(TLDR_HOME) - 1)

#define TLDR_DATE "/.tldrc/date"
#define TLDR_DATE_LEN (sizeof(TLDR_DATE) - 1)

#define TLDR_EXT "/.tldrc/tldr-master/pages/"
#define TLDR_EXT_LEN (sizeof(TLDR_EXT) - 1)

#define ANSI_COLOR_RESET_FG                     "\x1b[39m"
#define ANSI_COLOR_TITLE_FG                     "\x1b[39m"
#define ANSI_COLOR_EXPLANATION_FG               "\x1b[39m"
#define ANSI_COLOR_COMMENT_FG                   "\x1b[32m"
#define ANSI_COLOR_CODE_FG                      "\x1b[31m"
#define ANSI_COLOR_CODE_PLACEHOLDER_FG          "\x1b[34m"
#define ANSI_BOLD_ON                            "\x1b[1m"
#define ANSI_BOLD_OFF                           "\x1b[22m"

#endif /* TLDR_H */
