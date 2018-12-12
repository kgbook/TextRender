#ifndef PROJECT_EXCEPTION_H
#define PROJECT_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace Exception {
enum class ErrorCode {
    BEGIN = __LINE__,
    ok,
    internalError,
    invalidParameter,
    exist,
    send,
    recv,
    select,
    alloc,
    notSupport,
    parse,
    other,

    /* add error code before size */
    size = __LINE__ - BEGIN
};

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const char* reason);

    RuntimeError(std::string& reason);

    RuntimeError(const char* reason, ErrorCode& code);

    RuntimeError(std::string& reason, ErrorCode& code);

    virtual ~RuntimeError() throw();

private:
    ErrorCode mCode;

    ErrorCode whatCode() const throw();
};

void runtimeAssert(std::string reason, ErrorCode code, bool isOkay);
void runtimeAssert(const char* reason, ErrorCode code, bool isOkay);

void runtimeAssert(std::string reason, ErrorCode code);
void runtimeAssert(const char* reason, ErrorCode code);

void runtimeAssert(std::string reason, bool isOkay);
void runtimeAssert(const char* reason, bool isOkay);

void runtimeAssert(std::string reason, int32_t rc);
void runtimeAssert(const char* reason, int32_t rc);

template <typename T>
std::string decimal_to_hexstring(T t);
}

#endif //PROJECT_EXCEPTION_H
