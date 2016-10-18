#include "tldr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>

int
construct_url(char *buf, size_t buflen, char const *input, char const *platform)
{
    size_t len;

    len = 0;
    if (sstrncat(buf, &len, buflen, BASE_URL, BASE_URL_LEN))
        return 1;
    if (sstrncat(buf, &len, buflen, "/", 1))
        return 1;
    if (sstrncat(buf, &len, buflen, platform, strlen(platform)))
        return 1;
    if (sstrncat(buf, &len, buflen, "/", 1))
        return 1;
    if (sstrncat(buf, &len, buflen, input, strlen(input)))
        return 1;
    if (sstrncat(buf, &len, buflen, ".md", 3))
        return 1;

    return 0;
}

int
construct_path(char *buf, size_t buflen, char const *home, char const *input,
               char const *platform)
{
    size_t len;

    len = 0;
    if (sstrncat(buf, &len, buflen, home, strlen(home)))
        return 1;
    if (sstrncat(buf, &len, buflen, TLDR_EXT, TLDR_EXT_LEN))
        return 1;
    if (sstrncat(buf, &len, buflen, platform, strlen(platform)))
        return 1;
    if (sstrncat(buf, &len, buflen, "/", 1))
        return 1;
    if (sstrncat(buf, &len, buflen, input, strlen(input)))
        return 1;
    if (sstrncat(buf, &len, buflen, ".md", 3))
        return 1;

    return 0;
}

int
parse_tldrpage(char const *input)
{
    char c;
    int i, len;
    int start = -1;

    len = (int)strlen(input);

    fprintf(stdout, "\n");
    for (i = 0; i < len; ++i) {
        c = input[i];
        if (start == -1) {
            switch (c) {
            case '>':
                start = i;
                fprintf(stdout, "%s", ANSI_COLOR_EXPLANATION_FG);
                continue;

            case '-':
                start = i;
                fprintf(stdout, "%s", ANSI_COLOR_COMMENT_FG);
                continue;

            case '`':
                start = i;
                fprintf(stdout, "%s", ANSI_COLOR_CODE_FG);
                fprintf(stdout, "    ");
                continue;

            case '#':
                start = i;
                fprintf(stdout, "%s", ANSI_BOLD_ON);
                fprintf(stdout, "%s", ANSI_COLOR_TITLE_FG);
                continue;
            }
        } else if (start > -1) {
            if (input[i] == '{' && input[i + 1] == '{') {
                fprintf(stdout, "%.*s", i - (start + 1), input + (start + 1));
                fprintf(stdout, "%s", ANSI_BOLD_OFF);
                fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
                fprintf(stdout, "%s", ANSI_COLOR_CODE_PLACEHOLDER_FG);

                start = i;
                for (i = i + 1; i < len; i++) {
                    if (input[i] == '}' && input[i + 1] == '}') {
                        fprintf(stdout, "%.*s", i - (start + 2),
                                input + (start + 2));
                        fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
                        fprintf(stdout, "%s", ANSI_COLOR_CODE_FG);
                        start = i + 1;
                        break;
                    }
                }

                continue;
            }
        }

        if (c == '\n' && start > -1) {
            if (input[i - 1] == '`') {
                fprintf(stdout, "%.*s", i - (start + 2), input + (start + 1));
            } else if (input[start] == '-') {
                fprintf(stdout, "\n%.*s", i - (start), input + start);
            } else if (input[start] == '>') {
                fprintf(stdout, "%.*s", i - (start + 2), input + (start + 2));
            } else {
                fprintf(stdout, "%.*s\n", i - (start + 2), input + (start + 2));
            }

            fprintf(stdout, "%s", ANSI_BOLD_OFF);
            fprintf(stdout, "%s", ANSI_COLOR_RESET_FG);
            fprintf(stdout, "\n");
            start = -1;
        }
    }

    return 0;
}

int
print_tldrpage(char const *input, char const *poverride)
{
    int islinux;
    int isdarwin;
    int issun;
    char *output;
    char url[URLBUFSIZ];
    struct utsname sys;
    char const *platform;
    char const *homedir;
    size_t len;
    char directory[STRBUFSIZ];
    struct stat sb;

    uname(&sys);
    islinux = strcmp(sys.sysname, "Linux") == 0;
    isdarwin = strcmp(sys.sysname, "Darwin") == 0;
    issun = strcmp(sys.sysname, "SunOS") == 0;

    if (poverride == NULL) {
        if (islinux)
            platform = "linux";
        else if (isdarwin)
            platform = "osx";
        else if (issun)
            platform = "sunos";
        else
            platform = "common";
    } else {
        platform = poverride;
        if (strcmp(platform, "linux") != 0 && strcmp(platform, "osx") != 0 &&
            strcmp(platform, "common") != 0 && strcmp(platform, "sunos") != 0) {
            fprintf(stderr, "Error: platform %s is unsupported\n", platform);
            fprintf(
                stderr, "Supported platforms: linux / osx / sunos / common\n");
            exit(EXIT_FAILURE);
        }
    }

    homedir = gethome();
    if (homedir == NULL)
        return 1;

    len = 0;
    if (sstrncat(directory, &len, STRBUFSIZ, homedir, strlen(homedir)))
        return 1;
    if (sstrncat(directory, &len, STRBUFSIZ, TLDR_EXT, TLDR_EXT_LEN))
        return 1;

    if (stat(directory, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        construct_path(url, URLBUFSIZ, homedir, input, platform);
        if (stat(url, &sb) == 0 && S_ISREG(sb.st_mode)) {
            if (!get_file_content(url, &output, 0)) {
                parse_tldrpage(output);
                free(output);
                return 0;
            }
        } else {
            construct_path(url, URLBUFSIZ, homedir, input, "common");
            if (stat(url, &sb) == 0 && S_ISREG(sb.st_mode)) {
                if (!get_file_content(url, &output, 0)) {
                    parse_tldrpage(output);
                    free(output);
                    return 0;
                }
            }
        }
    }

    construct_url(url, URLBUFSIZ, input, platform);

    /* make clang's static analyzer happy */
    output = NULL;
    download_content(url, &output, 0);
    if (output == NULL) {
        construct_url(url, URLBUFSIZ, input, "common");
        download_content(url, &output, 0);
        if (output == NULL)
            return 1;
    }

    parse_tldrpage(output);

    free(output);
    return 0;
}

int
print_localpage(char const *path)
{
    char *output = NULL;
    if (!get_file_content(path, &output, 0)) {
        parse_tldrpage(output);
        free(output);
        return 0;
    }

    return 0;
}

