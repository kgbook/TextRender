//
// Created by kang on 2018/8/18.
//

#include <sstream>
#include <string>
#include "exception.h"

Exception::RuntimeError::RuntimeError(std::string& reason)
        :std::runtime_error(reason),
         mCode(Exception::ErrorCode::internalError) { }

Exception::RuntimeError::RuntimeError(const char* reason)
        :std::runtime_error(std::string(reason)),
         mCode(Exception::ErrorCode::internalError) { }

Exception::RuntimeError::RuntimeError(std::string& reason, Exception::ErrorCode& code)
        :std::runtime_error(reason),
         mCode(code) { }

Exception::RuntimeError::RuntimeError(const char* reason, Exception::ErrorCode& code)
        :std::runtime_error(std::string(reason)),
         mCode(code) { }

Exception::RuntimeError::~RuntimeError() throw() { }

inline Exception::ErrorCode Exception::RuntimeError::whatCode() const throw()
{
    return mCode;
}

void Exception::runtimeAssert(std::string reason, Exception::ErrorCode code, bool isOkay)
{
    if (!isOkay) {
        runtimeAssert(reason.c_str(), code, isOkay);
    }
}

void Exception::runtimeAssert(const char* reason, Exception::ErrorCode code, bool isOkay)
{
    if (!isOkay && code!=Exception::ErrorCode::ok) {
        throw RuntimeError(reason, code);
    }
}

void Exception::runtimeAssert(std::string reason, Exception::ErrorCode code)
{
    Exception::runtimeAssert(reason.c_str(), code);
}

void Exception::runtimeAssert(const char* reason, Exception::ErrorCode code)
{
    if (ErrorCode::ok!=code) {
        throw Exception::RuntimeError(reason, code);
    }
}

void Exception::runtimeAssert(std::string reason, bool isOkay)
{
    Exception::runtimeAssert(reason.c_str(), isOkay);
}

void Exception::runtimeAssert(const char* reason, bool isOkay)
{
    if (!isOkay) {
        throw Exception::RuntimeError(reason);
    }
}

void Exception::runtimeAssert(std::string reason, int32_t rc) {
    if (0 != rc) {
        auto msg = reason + std::string(", rc = 0x") + decimal_to_hexstring(rc);
        runtimeAssert(msg, false);
    }
}

void Exception::runtimeAssert(const char *reason, int32_t rc) {
    if (nullptr == reason) {
        return;
    }

    auto msg = std::string(reason);
    runtimeAssert(msg, rc);
}

template <typename T>
std::string Exception::decimal_to_hexstring(T t) {
    std::stringstream ss;
    ss << std::hex <<t;

    return ss.str();
}
