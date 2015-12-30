#include <string>

// Constants.
std::string const kBaseUrl = "http://raw.github.com/tldr-pages/tldr/master/pages";


// cURL.
struct response {
	std::string str;
};

void init_response(struct response *r);
size_t writeCallback(void *ptr, size_t size, size_t nmemb, struct response *r);

// Fetching.
std::string getUrlForArgAndPlatform(std::string const& arg, std::string const& platform);
std::string getUrlForArg(std::string const& arg);
std::string getContentForUrl(std::string const& url);

void replaceAll(std::string& str, std::string const& from, std::string const& to);
