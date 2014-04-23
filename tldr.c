#include "tldr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <curl/curl.h>

#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_WHITE        "\x1b[37m"
#define ANSI_COLOR_RESET        "\x1b[0m"

const char *kBaseUrl = "http://raw.github.com/tldr-pages/tldr/master/pages";

struct response {
    char *str;
    size_t len;
};

void init_response(struct response *r) {
    r->len = 0;
    r->str = malloc(r->len + 1);
    if (r->str == NULL) {
        exit(EXIT_FAILURE);
    }
    r->str[0] = '\0';
}

char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);
    if (d == NULL) return NULL;
    strcpy (d, s);
    return d;
}

size_t getUrlForArgAndPlatform(char **dest, const char *arg, const char *platform) {
    int isLinux = strcmp(platform, "Linux");
    int isOSX = strcmp(platform, "Darwin");

    char *platformUrlDelimiter = malloc(6 + 1);
    if (isLinux == 0) {
        strcpy(platformUrlDelimiter, "linux");
    } else if (isOSX == 0) {
        strcpy(platformUrlDelimiter, "osx");
    } else {
        strcpy(platformUrlDelimiter, "common");
    }

    int length = (int) strlen(kBaseUrl);
    length += strlen(platformUrlDelimiter);
    length += strlen(arg);
    length += 6;

    char *buffer = (char *) malloc(length);
    if (buffer == NULL) {
        exit(EXIT_FAILURE);
    }
    strcpy(buffer, kBaseUrl);
    strcat(buffer, "/");
    strcat(buffer, platformUrlDelimiter);
    strcat(buffer, "/");
    strcat(buffer, arg);
    strcat(buffer, ".md");

    *dest = malloc(strlen(buffer) + 1);
    strcpy(*dest, buffer);

    free(platformUrlDelimiter);
    free(buffer);

    return strlen(buffer) + 1;
}

size_t getUrlForArg(char **dest, char *arg) {
    return getUrlForArgAndPlatform(dest, arg, "common");
}

size_t writeCallback(void *ptr, size_t size, size_t nmemb, struct response *r) {
    size_t new_len = r->len + (size * nmemb);
    r->str = realloc(r->str, new_len + 1);
    if (r->str == NULL) {
        exit(EXIT_FAILURE);
    }

    memcpy(r->str + r->len, ptr, size * nmemb);
    r->str[new_len] = '\0';
    r->len = new_len;

    return size * nmemb;
}

int getContentForUrl(char **dest, char *url) {
    CURL *curl;
    CURLcode res;
    struct response response;
    init_response(&response);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            if (httpCode == 200) {
                curl_easy_cleanup(curl);
                *dest = strdup(response.str);
                return 1;
            } else {
                curl_easy_cleanup(curl);
                return 0;
            }
        }

        curl_easy_cleanup(curl);
    }

    return 0;
}

void insertCharsAt(char **input, char *insert, int position) {
    size_t newLength = strlen(*input) + strlen(insert);
    char *dest = malloc(newLength + 1);
    char *end = malloc(newLength - position + 1);
    strncpy(dest, *input, position);
    char *in = *input;
    memcpy(end, &in[position], strlen(*input) - position);

    strcat(dest, insert);
    strcat(dest, end);

    *input = malloc(strlen(dest) + 1);
    strcpy(*input, dest);

    free(dest);
    free(end);
}

char *replace(char *source_str, char *search_str, char *replace_str) {
    char *ostr, *nstr = NULL, *pdest = "";
    int length, nlen;
    unsigned int nstr_allocated = 0;
    unsigned int ostr_allocated = 0;
    if (!source_str || !search_str || !replace_str) {
        printf("Not enough arguments\n");
        return NULL;
    }
    ostr_allocated = sizeof(char) * (unsigned int) (strlen(source_str) + 1);
    ostr = malloc(sizeof(char) * (strlen(source_str) + 1));
    if (!ostr) {
        printf("Insufficient memory available\n");
        return NULL;
    }
    strcpy(ostr, source_str);
    while (pdest) {
        pdest = strstr(ostr, search_str);
        length = (int) (pdest - ostr);
        if (pdest != NULL ) {
            ostr[length] = '\0';
            nlen = (unsigned int) strlen(ostr) + (unsigned int) strlen(replace_str) + (unsigned int) strlen(strchr(ostr, 0) + strlen(search_str)) + 1;
            if (!nstr || /* _msize( nstr ) */ nstr_allocated < sizeof(char) * nlen) {
                nstr_allocated = sizeof(char) * nlen;
                nstr = malloc(sizeof(char) * nlen);
            }
            if (!nstr) {
                printf("Insufficient memory available\n");
                return NULL;
            }
            strcpy(nstr, ostr);
            strcat(nstr, replace_str);
            strcat(nstr, strchr(ostr, 0) + strlen(search_str));
            if ( /* _msize(ostr) */ ostr_allocated < sizeof(char) * strlen(nstr) + 1) {
                ostr_allocated = sizeof(char) * (unsigned int) strlen(nstr) + 1;
                ostr = malloc(sizeof(char) * strlen(nstr) + 1);
            }
            if (!ostr) {
                printf("Insufficient memory available\n");
                return NULL;
            }
            strcpy(ostr, nstr);
        }
    }
    if (nstr) {
        free(nstr);
    }
    return ostr;
}

int startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
    lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

int main(int argc, char *argv[]) {
    struct utsname sys;
    uname(&sys);

    if (argc > 1) {
        char *arg = argv[1];
        char *url = NULL;
        getUrlForArg(&url, arg);
        char *urlForPlatform = NULL;
        getUrlForArgAndPlatform(&urlForPlatform, arg, sys.sysname);

        char *response = NULL;
        int res = getContentForUrl(&response, urlForPlatform);
        if (res == 0) {
            getContentForUrl(&response, url);
        }

        response = replace(response, "{{", ANSI_COLOR_WHITE);
        response = replace(response, "}}", ANSI_COLOR_RESET);
        
        printf("%s\n", response);
        
    } else {
        // TODO Print Help!
    }
}


