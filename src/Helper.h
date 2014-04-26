#include <string>

class Helper {

public:
    // Con- and Destructor.
    Helper();
    ~Helper();

    // String Helper.
    static void replaceAll(std::string &str, const std::string &from, const std::string &to);

};
