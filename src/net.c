#include "tldr.h"
#include <curl/curl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                  curl_off_t ultotal, curl_off_t ulnow)
{
    int i, total;
    double progress;
    ((void)ultotal), ((void)ulnow);
    if (dltotal <= 0)
        return 0;

    progress = (double)(dlnow / dltotal);
    total = (int)rround(progress * 40.0);

    printf("%s [", (char *)clientp);
    for (i = 0; i < total; i++)
        printf("=");
    for (; i < 40; i++)
        printf(" ");
    printf("] %.0f%%\r", progress * 100.0f);
    fflush(stdout);

    return 0;
}

int
old_progress_callback(
    void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
    return progress_callback(p, (curl_off_t)dltotal, (curl_off_t)dlnow,
                             (curl_off_t)ultotal, (curl_off_t)ulnow);
}

int
download_file(char const *url, char const *outfile, int verbose)
{
    CURL *curl;

    curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        int ret = 1;
        FILE *file;

        file = fopen(outfile, "wb");
        if (!file)
            return 1;

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        if (verbose) {
            char *base;
            char filename[FILENAME_MAX];
            size_t len = strlen(outfile);
            memcpy(filename, outfile, len);
            filename[len] = '\0';
            base = basename(filename);

#if LIBCURL_VERSION_NUM >= 0x072000
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(
                curl, CURLOPT_XFERINFOFUNCTION, &progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, base);
#else
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(
                curl, CURLOPT_PROGRESSFUNCTION, &old_progress_callback);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, base);
#endif
        }

        res = curl_easy_perform(curl);
        if (verbose)    printf("\n");
        if (res == CURLE_OK) {
            long http_code = 0;

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200)    ret = 0;
            else    ret = 1;
        }

        curl_easy_cleanup(curl);
        fclose(file);
        return ret;
    }

    return 1;
}

struct curl_string {
    char *str;
    size_t len;
};

void
curl_string_init(struct curl_string *str)
{
    str->len = 0;
    str->str = (char *)malloc(1024);
    if (str->str == NULL)
        exit(EXIT_FAILURE);

    str->str[1024] = '\0';
}

size_t
write_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
    struct curl_string *str = (struct curl_string *)stream;
    size_t newlen = str->len + size * nmemb;

    str->str = (char *)realloc(str->str, newlen + 1);
    if (str->str == NULL)
        exit(EXIT_FAILURE);

    memcpy(str->str + str->len, ptr, size * nmemb);
    str->str[newlen] = '\0';
    str->len = newlen;

    return size * nmemb;
}

int
download_content(char const *url, char **out, int verbose)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        struct curl_string str;
        int ret = 1;

        curl_string_init(&str);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        if (verbose) {
        }

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long http_code = 0;

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200) {
                *out = str.str;
                ret = 0;
            } else {
                free(str.str);
                *out = NULL;
                ret = 1;
            }
        }

        curl_easy_cleanup(curl);
        return ret;
    }

    *out = NULL;
    return 1;
}

