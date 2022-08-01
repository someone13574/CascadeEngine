#pragma once

#include <cmath>
#include <type_traits>

namespace Cascade_Graphics
{
    template <typename Vector_T>
    class Vector_4
    {
        static_assert(std::is_arithmetic<Vector_T>::value, "Vector: Vector type must be numeric");

    public:
        Vector_T m_x = 0;
        Vector_T m_y = 0;
        Vector_T m_z = 0;
        Vector_T m_w = 0;

    public:
        Vector_4(Vector_T x, Vector_T y, Vector_T z, Vector_T w) : m_x(x), m_y(y), m_z(z), m_w(w){};

        template <typename Convert_T>
        operator Vector_4<Convert_T>()
        {
            static_assert(std::is_arithmetic<Convert_T>::value, "Vector: Cannot convert to a vector of this type");
            return Vector_4<Convert_T>((Convert_T)m_x, (Convert_T)m_y, (Convert_T)m_z, (Convert_T)m_w);
        }

        template <typename Scalar_T>
        Vector_4<Vector_T> operator+(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_4<Vector_T>(m_x + (Vector_T)value, m_y + (Vector_T)value, m_z + (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator+=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_4<Vector_T>(m_x + (Vector_T)value, m_y + (Vector_T)value, m_z + (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator+(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_4<Vector_T>(m_x + (Vector_T)vector.m_x, m_y + (Vector_T)vector.m_y, m_z + (Vector_T)vector.m_z, m_w + (Vector_T)vector.m_w);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator+=(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_4<Vector_T>(m_x + (Vector_T)vector.m_x, m_y + (Vector_T)vector.m_y, m_z + (Vector_T)vector.m_z, m_w + (Vector_T)vector.m_w);
            return *this;
        }
        Vector_4<Vector_T> operator++()
        {
            *this = Vector_4<Vector_T>(m_x++, m_y++);
            return *this;
        }

        Vector_4<Vector_T> operator-()
        {
            return Vector_4<Vector_T>(-m_x, -m_y, -m_z, -m_w);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator-(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_4<Vector_T>(m_x - (Vector_T)value, m_y - (Vector_T)value, m_z - (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator-=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_4<Vector_T>(m_x - (Vector_T)value, m_y - (Vector_T)value, m_z - (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator-(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_4<Vector_T>(m_x - (Vector_T)vector.m_x, m_y - (Vector_T)vector.m_y, m_z - (Vector_T)vector.m_z, m_w - (Vector_T)vector.m_w);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator-=(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_4<Vector_T>(m_x - (Vector_T)vector.m_x, m_y - (Vector_T)vector.m_y, m_z - (Vector_T)vector.m_z, m_w - (Vector_T)vector.m_w);
            return *this;
        }
        Vector_4<Vector_T> operator--()
        {
            *this = Vector_4<Vector_T>(m_x--, m_y--);
            return *this;
        }

        template <typename Scalar_T>
        Vector_4<Vector_T> operator*(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_4<Vector_T>(m_x * (Vector_T)value, m_y * (Vector_T)value, m_z * (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator*=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_4<Vector_T>(m_x * (Vector_T)value, m_y * (Vector_T)value, m_z * (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator*(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_4<Vector_T>(m_x * (Vector_T)vector.m_x, m_y * (Vector_T)vector.m_y, m_z * (Vector_T)vector.m_z, m_w * (Vector_T)vector.m_w);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator*=(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_4<Vector_T>(m_x * (Vector_T)vector.m_x, m_y * (Vector_T)vector.m_y, m_z * (Vector_T)vector.m_z, m_w * (Vector_T)vector.m_w);
            return *this;
        }

        template <typename Scalar_T>
        Vector_4<Vector_T> operator/(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_4<Vector_T>(m_x / (Vector_T)value, m_y / (Vector_T)value, m_z / (Vector_T)value);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator/=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_4<Vector_T>(m_x / (Vector_T)value, m_y / (Vector_T)value, m_z / (Vector_T)value);
            return *this;
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator/(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_4<Vector_T>(m_x / (Vector_T)vector.m_x, m_y / (Vector_T)vector.m_y, m_z / (Vector_T)vector.m_z, m_w / (Vector_T)vector.m_w);
        }
        template <typename Scalar_T>
        Vector_4<Vector_T> operator/=(Vector_4<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_4<Vector_T>(m_x / (Vector_T)vector.m_x, m_y / (Vector_T)vector.m_y, m_z / (Vector_T)vector.m_z, m_w / (Vector_T)vector.m_w);
            return *this;
        }

        Vector_4<Vector_T> Normalized()
        {
            return *this / sqrt((double)(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w));
        }
        void Normalize()
        {
            *this /= sqrt((double)(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w));
        }

        Vector_T Length()
        {
            return sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
        }

        template <typename T>
        static double Dot(Vector_4<T> vector_a, Vector_4<T> vector_b)
        {
            return (double)vector_a.x * (double)vector_b.x + (double)vector_a.y * (double)vector_b.y + (double)vector_a.z * (double)vector_b.z + (double)vector_a.w * (double)vector_b.w;
        }
    };
} // namespace Cascade_Graphics