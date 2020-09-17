#pragma once

#include <cmath>

#include <glm/glm.hpp>

namespace Elysium
{
    using Vector2 = glm::vec2;
    using Vector3 = glm::vec3;
    using Vector4 = glm::vec4;

    template<class T>
    static T abs(const T& vector)
    {
        return glm::abs(vector);
    }

    static float cross(const Vector2& a, const Vector2& b)
    {
        return a.x * b.y - a.y * b.x;
    }

    template<class T>
    static bool isAllNaN(const T& vector)
    {
        return glm::all(glm::isnan(vector));
    }

    template <class T>
    static T max(const T& vector)
    {
        return glm::max(vector);
    }

    static float radians(float angle)
    {
        return glm::radians(angle);
    }

    static bool isinf(const Vector2& vector)
    {
        return (std::isinf(vector.x) || std::isinf(vector.y));
    }

    static bool isNaN(const Vector2& vector)
    {
        return (std::isnan(vector.x) || std::isnan(vector.y));
    }

    struct Complex
    {
    public:
        float real;
        float imaginary;

        Complex(float x, float y) : real(x), imaginary(y) { }

        Complex operator + (const Complex& a)
        {
            return { this->real + a.real, this->imaginary + a.imaginary };
        }

        Complex operator * (float a)
        {
            return { this->real * a, this->real * a };
        }

        static Complex scaleXY(const Complex& complex, float x, float y)
        {
            return { complex.real * x, complex.imaginary * y };
        }

        Complex operator * (const Complex& a)
        {
            return { this->real * a.real - this->imaginary * a.imaginary,
                this->real * a.imaginary + this->imaginary * a.real };
        }

        operator glm::vec2() const
        {
            return { real, imaginary };
        }
    };
}