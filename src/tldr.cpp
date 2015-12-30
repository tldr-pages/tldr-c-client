#include "tldr.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/utsname.h>

#define ANSI_COLOR_BLACK_BG     "\x1b[40m"
#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_WHITE        "\x1b[37m"
#define ANSI_COLOR_RESET_ALL    "\x1b[0m"
#define ANSI_COLOR_RESET_BG     "\x1b[49m"
#define ANSI_COLOR_RESET_FG     "\x1b[39m"


int main(int argc, char *argv[])
{
    struct utsname sys;
    uname(&sys);

    if (argc > 1)
    {
        std::string arg(argv[1]);
        std::string url = getUrlForArg(arg);
        std::string urlForPlatform = getUrlForArgAndPlatform(arg, sys.sysname);

        std::string response = getContentForUrl(urlForPlatform);
        if (response.empty()) response = getContentForUrl(url);

        replaceAll(response, "{{", ANSI_COLOR_WHITE);
        replaceAll(response, "}}", ANSI_COLOR_RESET_FG);

        std::string const stripPrefix("#");
        std::string const explainPrefix(">");
        std::string const commentPrefix("-");
        std::string const codePrefix("`");
        std::stringstream ss(response);
        std::string to;
        bool firstComment = true;

        while(std::getline(ss, to, '\n'))
        {
            if (to.compare(0, stripPrefix.size(), stripPrefix) == 0)
            {
                // Do nothing!
            }
            else if (to.compare(0, explainPrefix.size(), explainPrefix) == 0)
            {
                replaceAll(to, ">", ANSI_COLOR_WHITE);
                std::cout << to << ANSI_COLOR_RESET_FG << std::endl;
            }
            else if (to.compare(0, commentPrefix.size(), commentPrefix) == 0)
            {
                if (firstComment)
                {
                    std::cout << std::endl;
                    firstComment = false;
                }

                replaceAll(to, "-", ANSI_COLOR_GREEN);
                std::cout << to << ANSI_COLOR_RESET_FG << std::endl;
            }
            else if (to.compare(0, codePrefix.size(), codePrefix) == 0)
            {
                to = to.substr(1, to.size());
                to = to.substr(0, to.size() - 1);
                std::cout << ANSI_COLOR_BLACK_BG << to << ANSI_COLOR_RESET_BG << std::endl << std::endl;
            }
        }
    }
}


// =====================================
// URL determination.
// =====================================
std::string getUrlForArgAndPlatform(std::string const& arg, std::string const& platform)
{
    int isLinux = !platform.compare("Linux");
    int isOSX = !platform.compare("Darwin");

    std::string platformUrlDelimiter;
    if (isLinux) platformUrlDelimiter = "linux";
    else if (isOSX) platformUrlDelimiter = "osx";
    else platformUrlDelimiter = "common";

    std::string url(kBaseUrl);
    url += "/" + platformUrlDelimiter + "/" + arg + ".md";

    return url;
}

std::string getUrlForArg(std::string const& arg)
{
    return getUrlForArgAndPlatform(arg, "common");
}


// =====================================
// Curl Fetching.
// =====================================
void init_response(struct response *r)
{
    r->len = 0;
    r->str = (char *) malloc(r->len + 1);
    if (r->str == NULL)
    {
        exit(EXIT_FAILURE);
    }

    r->str[0] = '\0';
}

size_t writeCallback(void *ptr, size_t size, size_t nmemb, struct response *r)
{
    size_t new_len = r->len + (size * nmemb);
    r->str = (char *) realloc(r->str, new_len + 1);
    if (r->str == NULL)
    {
        exit(EXIT_FAILURE);
    }

    memcpy(r->str + r->len, ptr, size * nmemb);
    r->str[new_len] = '\0';
    r->len = new_len;

    return size * nmemb;
}

std::string getContentForUrl(std::string const& url)
{
    CURL *curl;
    CURLcode res;
    struct response response;
    init_response(&response);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            if (httpCode == 200)
            {
                curl_easy_cleanup(curl);
                return std::string(response.str);
            }
            else
            {
                curl_easy_cleanup(curl);
                return "";
            }
        }

        curl_easy_cleanup(curl);
    }

    return "";
}

