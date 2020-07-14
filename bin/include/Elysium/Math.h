#pragma once

#include <glm/glm.hpp>

namespace Elysium
{
    using Vector2 = glm::vec2;
    using Vector3 = glm::vec3;
    using Vector4 = glm::vec4;

    template<class T>
    static T dot(const T& vector)
    {
        return glm::dot(vector);
    }

    template<class T>
    static T normalize(const T& vector)
    {
        return glm::normalize(vector);
    }

    static float radians(float angle)
    {
        return glm::radians(angle);
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