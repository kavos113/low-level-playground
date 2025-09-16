#ifndef KERNEL_REGISTER_H
#define KERNEL_REGISTER_H

#include <cstdint>
#include <cstddef>

template <typename T>
struct ArrayLength
{
};

template <typename T, size_t N>
struct ArrayLength<T[N]>
{
    static const size_t value = N;
};

template <typename T>
class MemMapRegister
{
public:
    T Read() const
    {
        T tmp;

        for (size_t i = 0; i < m_length; ++i)
        {
            tmp.data[i] = m_value.data[i];
        }

        return tmp;
    }

    void Write(const T& value)
    {
        for (size_t i = 0; i < m_length; ++i)
        {
            m_value.data[i] = value.data[i];
        }
    }

private:
    volatile T m_value;
    static const size_t m_length = ArrayLength<decltype(T::data)>::value;
};

template <typename T>
struct DefaultBitmap
{
    T data[1];

    DefaultBitmap& operator=(const T& other)
    {
        data[0] = other;
        return *this;
    }

    operator T() const
    {
        return data[0];
    }
};

template <typename T>
class ArrayWrapper
{
public:
    using ValueType = T;
    using Iterator = ValueType*;
    using ConstIterator = const ValueType*;

    ArrayWrapper(uintptr_t array_base_addr, size_t size)
        : m_array{reinterpret_cast<ValueType*>(array_base_addr)},
          m_size{size}
    {
    }

    size_t Size() const
    {
        return m_size;
    }

    Iterator begin()
    {
        return m_array;
    }

    Iterator end()
    {
        return m_array + m_size;
    }

    ConstIterator begin() const
    {
        return m_array;
    }

    ConstIterator end() const
    {
        return m_array + m_size;
    }

    ValueType& operator[](size_t index)
    {
        return m_array[index];
    }

private:
    ValueType* m_array;
    size_t m_size;
};

#endif //KERNEL_REGISTER_H
