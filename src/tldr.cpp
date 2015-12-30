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

using namespace std;

int main(int argc, char *argv[])
{
    struct utsname sys;
    uname(&sys);

    if (argc > 1)
    {
        string arg(argv[1]);
        int k = 2;
        while (k < argc) {
            arg += "-" + string(argv[k++]);
        }
        string url = getUrlForArg(arg);
        string urlForPlatform = getUrlForArgAndPlatform(arg, sys.sysname);

        string response = getContentForUrl(urlForPlatform);
        if (response.empty()) response = getContentForUrl(url);

        replaceAll(response, "{{", ANSI_COLOR_WHITE);
        replaceAll(response, "}}", ANSI_COLOR_RESET_FG);

        string const stripPrefix("#");
        string const explainPrefix(">");
        string const commentPrefix("-");
        string const codePrefix("`");
        stringstream ss(response);
        string line;
        int firstComment = 0;

        while(getline(ss, line, '\n'))
        {
            if (line.compare(0, stripPrefix.size(), stripPrefix) == 0)
            {
                // Do nothing!
            }
            else if (line.compare(0, explainPrefix.size(), explainPrefix) == 0)
            {
                replaceAll(line, explainPrefix, ANSI_COLOR_WHITE);
                cout << line << ANSI_COLOR_RESET_FG << endl;
            }
            else if (line.compare(0, commentPrefix.size(), commentPrefix) == 0)
            {
                if (firstComment == 0)
                {
                    cout << endl;
                    firstComment = 1;
                }

                replaceAll(line, commentPrefix, ANSI_COLOR_GREEN);
                cout << line << ANSI_COLOR_RESET_FG << endl;
            }
            else if (line.compare(0, codePrefix.size(), codePrefix) == 0)
            {
                line = line.substr(1, line.size());
                line = line.substr(0, line.size() - 1);
                cout << ANSI_COLOR_BLACK_BG << line << ANSI_COLOR_RESET_BG << endl << endl;
            }
        }
    }
}


// =====================================
// URL determination.
// =====================================
string getUrlForArgAndPlatform(string const& arg, string const& platform)
{
    int isLinux = !platform.compare("Linux");
    int isOSX = !platform.compare("Darwin");

    string platformUrlDelimiter;
    if (isLinux) platformUrlDelimiter = "linux";
    else if (isOSX) platformUrlDelimiter = "osx";
    else platformUrlDelimiter = "common";

    string url(kBaseUrl);
    url += "/" + platformUrlDelimiter + "/" + arg + ".md";

    return url;
}

string getUrlForArg(string const& arg)
{
    return getUrlForArgAndPlatform(arg, "common");
}

void replaceAll(string& str, string const& from, string const& to)
{
    if (from.empty()) return;

    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
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
    r->str += string(reinterpret_cast<string::pointer>(ptr), extra_len);
    return extra_len;
}

string getContentForUrl(string const& url)
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

