#ifndef __GEN_EXCEPT_HPP__
#include <exception>
#include <string>
class generic_exception : public std::exception {
    std::string message;

    public:
    generic_exception(std::string mess)
    : message(mess) {};

    virtual const char *what() const throw() {
        return message.c_str();
    }
};
#define __GEN_EXCEPT_HPP__
#endif
