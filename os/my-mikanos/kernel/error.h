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
        INDEX_OUT_OF_RANGE,
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
    static constexpr std::array<const char *, 4> m_codeNames = {
        "SUCCESS",
        "FULL",
        "EMPTY",
        "INDEX_OUT_OF_RANGE"
    };
    Code m_code;
};

#endif //KERNEL_ERROR_H