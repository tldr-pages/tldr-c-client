#include "tldr.h"

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
        int firstComment = 0;

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
                if (firstComment == 0)
                {
                    std::cout << std::endl;
                    firstComment = 1;
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

void replaceAll(std::string& str, std::string const& from, std::string const& to)
{
    if (from.empty()) return;

    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

// =====================================
// Curl Fetching.
// =====================================
size_t writeCallback(void *ptr, size_t size, size_t nmemb, struct response *r)
{
    size_t extra_len = size * nmemb;
    r->str += std::string(reinterpret_cast<std::string::pointer>(ptr), extra_len);
    return extra_len;
}

std::string getContentForUrl(std::string const& url)
{
    CURL *curl;
    CURLcode res;
    struct response response;

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
                return response.str;
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

