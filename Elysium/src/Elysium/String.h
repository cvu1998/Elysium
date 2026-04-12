#include <array>

namespace Elysium
{
    class String32 final
    {
    private:
        inline static constexpr size_t STR_SIZE = 32;

        std::array<char, STR_SIZE> m_String;
        char m_NullTerminator = '\0';

    private:
        inline void assign(const char* str)
        {
            bool terminated = false;
            char c[2] = { ' ', '\0' };
            for (unsigned int i = 0; i < STR_SIZE; ++i)
            {
                terminated |= str[i] == '\0';
                c[0] = str[i];
                m_String[i] = c[terminated];
            }
        }

    public:
        String32() = default;
        String32(const String32&) = default;
        String32(String32&&) = default;
        ~String32() = default;

        String32(const char* str);

        inline String32 operator=(const String32& str)
        {
            memcpy(m_String.data(), str.data(), STR_SIZE);
            return *this;
        }

        inline String32 operator=(String32&& str) noexcept
        {
            m_String = std::move(str.m_String);
            return *this;
        }

        inline String32 operator=(const char* str)
        {
            assign(str);
            return *this;
        }

        inline const char* data() const
        {
            return m_String.data();
        }

        inline bool operator==(const String32& str) const
        {
            const uint64_t* a = reinterpret_cast<const uint64_t*>(this->m_String.data());
            const uint64_t* b = reinterpret_cast<const uint64_t*>(str.data());
            return ( a[0] == b[0] ) && ( a[1] == b[1] ) && ( a[2] == b[2] ) && ( a[3] == b[3] );
        }
    };
}

namespace std
{
    template<>
    struct hash<Elysium::String32>
    {
        size_t operator()(const Elysium::String32& str) const noexcept
        {
            const uint64_t* p = reinterpret_cast<const uint64_t*>(str.data());
            size_t seed = 0;
            seed ^= std::hash<uint64_t>()(p[0]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<uint64_t>()(p[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<uint64_t>()(p[2]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<uint64_t>()(p[3]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}