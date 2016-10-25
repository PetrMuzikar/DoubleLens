#ifndef INTEGRATION_EXCEPTION_HPP
#define INTEGRATION_EXCEPTION_HPP

#include <string>
#include <exception>

class IntegrationException : public std::exception
{
public:
    IntegrationException(const std::string& msg = "")
    {
        msg_ = msg;
    }

//    virtual const char* what() const throw()
//    {
//        return msg_.c_str();
//    }

    virtual ~IntegrationException() throw()
    {

    }

private:
    std::string msg_;
};

#endif
