#ifndef KERNEL_ERROR_H
#define KERNEL_ERROR_H

#include <array>

class Error
{
public:
    enum class Code
    {
        SUCCESS,
        FULL,
        EMPTY,
        LAST_OF_CODE
    };

    Error(Code code) : m_code(code) {}

    operator bool() const
    {
        return this->m_code != Code::SUCCESS;
    }

    const char *name() const
    {
        return m_codeNames[static_cast<int>(this->m_code)];
    }

private:
    static constexpr std::array<const char *, 3> m_codeNames = {
        "SUCCESS",
        "FULL",
        "EMPTY"
    };
    Code m_code;
};

#endif //KERNEL_ERROR_H