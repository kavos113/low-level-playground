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
        HOST_CONTROLLER_NOT_HALTED,
        INVALID_SLOT_ID,
        PORT_NOT_CONNECTED,
        INVALID_ENDPOINT_NUMBER,
        TRANSFER_RING_NOT_SET,
        ALREADY_ALLOCATED,
        NOT_IMPLEMENTED,
        INVALID_DESCRIPTOR,
        BUFFER_TOO_SMALL,
        UNKNOWN_DEVICE,
        NO_CORRESPONDING_SETUP_STAGE,
        TRANSFER_FAILED,
        INVALID_PHASE,
        UNKNOWN_XHCI_SPEED_ID,
        NO_WAITER,
        LAST_OF_CODE
    };

    Error(Code code) : m_code(code)
    {
    }

    operator bool() const
    {
        return this->m_code != Code::SUCCESS;
    }

    const char* name() const
    {
        return m_codeNames[static_cast<int>(this->m_code)];
    }

private:
    static constexpr std::array<const char*, 19> m_codeNames = {
        "SUCCESS",
        "FULL",
        "EMPTY",
        "INDEX_OUT_OF_RANGE",
        "HOST_CONTROLLER_NOT_HALTED",
        "INVALID_SLOT_ID",
        "PORT_NOT_CONNECTED",
        "INVALID_ENDPOINT_NUMBER",
        "TRANSFER_RING_NOT_SET",
        "ALREADY_ALLOCATED",
        "NOT_IMPLEMENTED",
        "INVALID_DESCRIPTOR",
        "BUFFER_TOO_SMALL",
        "UNKNOWN_DEVICE",
        "NO_CORRESPONDING_SETUP_STAGE",
        "TRANSFER_FAILED",
        "INVALID_PHASE",
        "UNKNOWN_XHCI_SPEED_ID",
        "NO_WAITER"
    };
    Code m_code;
};

#endif //KERNEL_ERROR_H
