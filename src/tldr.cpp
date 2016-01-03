#include <iostream>
#include <string>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/utsname.h>

static const char* const ANSI_COLOR_RESET_FG            = "\x1b[39m";

static const char* const ANSI_COLOR_TITLE_FG            = ANSI_COLOR_RESET_FG;
static const char* const ANSI_COLOR_EXPLANATION_FG      = ANSI_COLOR_RESET_FG;
static const char* const ANSI_COLOR_COMMENT_FG          = "\x1b[32m";

static const char* const ANSI_COLOR_CODE_FG             = "\x1b[31m";
static const char* const ANSI_COLOR_CODE_PLACEHOLDER_FG = "\x1b[34m";

static const char* const ANSI_BOLD_ON                   = "\x1b[1m";
static const char* const ANSI_BOLD_OFF                  = "\x1b[22m";


// Constants.
std::string const kBaseUrl =
    "http://raw.github.com/tldr-pages/tldr/master/pages";

// cURL
struct curl_holder
{
    CURL *curl_;
    curl_holder(CURL *curl) : curl_(curl) {}
    ~curl_holder() { curl_easy_cleanup(curl_); }
};
size_t writeCallback(char* raw, size_t size, size_t nmemb, std::string* data);

// Fetching.
std::string getUrlForArgAndPlatform(std::string const& arg,
                                    std::string const& platform);
std::string getUrlForArg(std::string const& arg);
std::string getContentForUrl(std::string const& url);

// Utilities.
void replaceAll(std::string& str, std::string const& from,
                std::string const& to);


int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        std::cout << "usage: tldr <command>" << std::endl;
        std::cout << "Which page do you want to see?" << std::endl;
        std::cout << "try: tldr tldr" << std::endl;
        return EXIT_FAILURE;
    }

    struct utsname sys;
    uname(&sys);

    int i = 2;
    std::string arg(argv[1]);
    while (i < argc) {
        arg += "-" + std::string(argv[i++]);
    }

    std::string url = getUrlForArg(arg);
    std::string urlForPlatform = getUrlForArgAndPlatform(arg, sys.sysname);

    std::string response = getContentForUrl(urlForPlatform);
    if (response.empty())
    {
        response = getContentForUrl(url);
    }

    if (response.empty())
    {
        std::cout << "This page doesn't exist yet!" << std::endl;
        std::cout << "Submit new pages here: https://github.com/tldr-pages/tldr" << std::endl;
        return EXIT_FAILURE;
    }

    replaceAll(response, "{{", ANSI_COLOR_CODE_PLACEHOLDER_FG);
    replaceAll(response, "}}", ANSI_COLOR_RESET_FG);

    std::string const stripPrefix("#");
    std::string const explainPrefix(">");
    std::string const commentPrefix("-");
    std::string const codePrefix("`");
    std::stringstream ss(response);
    std::string line;
    bool firstComment = true;

    while (std::getline(ss, line, '\n'))
    {
        // Title
        if (line.compare(0, stripPrefix.size(), stripPrefix) == 0)
        {
            replaceAll(line, "#", ANSI_COLOR_TITLE_FG);
            std::cout << std::endl
                      << ANSI_BOLD_ON
                      << line
                      << ANSI_BOLD_OFF
                      << ANSI_COLOR_RESET_FG
                      << std::endl;
        }
        // Command explanation
        else if (line.compare(0, explainPrefix.size(), explainPrefix) == 0)
        {
            replaceAll(line, explainPrefix, ANSI_COLOR_EXPLANATION_FG);
            std::cout << line << ANSI_COLOR_RESET_FG << std::endl;
        }
        // Example comment
        else if (line.compare(0, commentPrefix.size(), commentPrefix) == 0)
        {
            if (firstComment)
            {
                std::cout << std::endl;
                firstComment = false;
            }

            replaceAll(line, commentPrefix, ANSI_COLOR_COMMENT_FG);
            std::cout << line << ANSI_COLOR_RESET_FG << std::endl;
        }
        // Code example
        else if (line.compare(0, codePrefix.size(), codePrefix) == 0)
        {
            // Remove trailing backtick (`).
            line = line.substr(0, line.size() - 1);

            // Replace first backtick (`) with three spaces for aligned indentation.
            replaceAll(line, "`", "   ");
            std::cout << ANSI_COLOR_CODE_FG
                      << line
                      << ANSI_COLOR_RESET_FG
                      << std::endl
                      << std::endl;
        }
    }
    return EXIT_SUCCESS;
}


// =====================================
// URL determination.
// =====================================
std::string getUrlForArgAndPlatform(std::string const& arg,
                                    std::string const& platform)
{
    int isLinux = !platform.compare("Linux");
    int isOSX = !platform.compare("Darwin");

    std::string platformUrlDelimiter;
    if (isLinux) { platformUrlDelimiter = "linux"; }
    else if (isOSX) { platformUrlDelimiter = "osx"; }
    else { platformUrlDelimiter = "common"; }

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
size_t writeCallback(char* raw, size_t size, size_t nmemb, std::string* data)
{
    if (data == NULL) { return 0; }

    data->append(raw, size * nmemb);
    return size * nmemb;
}

std::string getContentForUrl(std::string const& url)
{
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        curl_holder holder(curl);
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
                return response;
            }
            else
            {
                return "";
            }
        }
    }

    return "";
}


// =====================================
// Utilities.
// =====================================
void replaceAll(std::string& str, std::string const& from,
                std::string const& to)
{
    if (from.empty()) { return; }

    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

