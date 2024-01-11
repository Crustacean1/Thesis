#ifndef RUNTIME_EXCEPTION
#define RUNTIME_EXCEPTION

#include <exception>
#include <sstream>
#include <string>

class RuntimeException : public std::exception {
    std::string info;

    RuntimeException(std::string info) : info(info){};

  public:
    template <typename... Args> static RuntimeException error(Args... args) {
        std::stringstream ss;
        ss << "Exception thrown:\n";
        ((ss << args << " "), ...);
        ss << "\nEnd of exception\n";
        return RuntimeException{ss.str()};
    }

    virtual const char *what() const noexcept { return info.c_str(); }
};

#endif /*RUNTIME_EXCEPTION*/
