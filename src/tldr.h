#include <string>

// Constants.
const std::string kBaseUrl = "http://raw.github.com/tldr-pages/tldr/master/pages";


// Curl.
struct response {
    char *str;
    size_t len;
};

void init_response(struct response *r);
size_t writeCallback(void *ptr, size_t size, size_t nmemb, struct response *r);


// Fetching.
std::string getUrlForArgAndPlatform(const std::string arg, const std::string platform);
std::string getUrlForArg(const std::string arg);
std::string getContentForUrl(const std::string url);

