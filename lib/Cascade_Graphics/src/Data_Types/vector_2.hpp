#pragma once

#include <cmath>
#include <type_traits>

namespace Cascade_Graphics
{
    template <typename Vector_T>
    class Vector_2
    {
        static_assert(std::is_arithmetic<Vector_T>::value, "Vector: Vector type must be numeric");

    public:
        Vector_2(){};
        Vector_2(Vector_T x, Vector_T y) : m_x(x), m_y(y){};

        template <typename Convert_T>
        operator Vector_2<Convert_T>()
        {
            static_assert(std::is_arithmetic<Convert_T>::value, "Vector: Cannot convert to a vector of this type");
            return Vector_2<Convert_T>((Convert_T)m_x, (Convert_T)m_y);
        }

        template <typename Scalar_T>
        Vector_2<Vector_T> operator+(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_2<Vector_T>(m_x + (Vector_T)value, m_y + (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator+=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_2<Vector_T>(m_x + (Vector_T)value, m_y + (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator+(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_2<Vector_T>(m_x + (Vector_T)vector.x, m_y + (Vector_T)vector.y);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator+=(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_2<Vector_T>(m_x + (Vector_T)vector.x, m_y + (Vector_T)vector.y);
            return *this;
        }
        Vector_2<Vector_T> operator++()
        {
            *this = Vector_2<Vector_T>(m_x++, m_y++);
            return *this;
        }

        Vector_2<Vector_T> operator-()
        {
            return Vector_2<Vector_T>(-m_x, -m_y);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator-(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric.");
            return Vector_2<Vector_T>(m_x - (Vector_T)value, m_y - (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator-=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_2<Vector_T>(m_x - (Vector_T)value, m_y - (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator-(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_2<Vector_T>(m_x - (Vector_T)vector.x, m_y - (Vector_T)vector.y);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator-=(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_2<Vector_T>(m_x - (Vector_T)vector.x, m_y - (Vector_T)vector.y);
            return *this;
        }
        Vector_2<Vector_T> operator--()
        {
            *this = Vector_2<Vector_T>(m_x--, m_y--);
            return *this;
        }

        template <typename Scalar_T>
        Vector_2<Vector_T> operator*(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_2<Vector_T>(m_x * (Vector_T)value, m_y * (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator*=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_2<Vector_T>(m_x * (Vector_T)value, m_y * (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator*(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_2<Vector_T>(m_x * (Vector_T)vector.x, m_y * (Vector_T)vector.y);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator*=(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_2<Vector_T>(m_x * (Vector_T)vector.x, m_y * (Vector_T)vector.y);
            return *this;
        }

        template <typename Scalar_T>
        Vector_2<Vector_T> operator/(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_2<Vector_T>(m_x / (Vector_T)value, m_y / (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator/=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_2<Vector_T>(m_x / (Vector_T)value, m_y / (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator/(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_2<Vector_T>(m_x / (Vector_T)vector.x, m_y / (Vector_T)vector.y);
        }
        template <typename Scalar_T>
        Vector_2<Vector_T> operator/=(Vector_2<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_2<Vector_T>(m_x / (Vector_T)vector.x, m_y / (Vector_T)vector.y);
            return *this;
        }

        Vector_2<Vector_T> Normalized()
        {
            return *this / sqrt((double)(m_x * m_x + m_y * m_y));
        }
        void Normalize()
        {
            *this /= sqrt((double)(m_x * m_x + m_y * m_y));
        }

        Vector_T Length()
        {
            return sqrt(m_x * m_x + m_y * m_y);
        }

        template <typename T>
        static double Dot(Vector_2<T> vector_a, Vector_2<T> vector_b)
        {
            return (double)vector_a.x * (double)vector_b.x + (double)vector_a.y * (double)vector_b.y;
        }

    public:
        Vector_T m_x = 0;
        Vector_T m_y = 0;
    };
} // namespace Cascade_Graphics