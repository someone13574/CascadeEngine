#pragma once

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace Cascade_Graphics
{
    template<typename Vector_T>
    class Vector_3
    {
        static_assert(std::is_arithmetic<Vector_T>::value, "Vector: Vector type must be numeric");

    public:
        Vector_T m_x = 0;
        Vector_T m_y = 0;
        Vector_T m_z = 0;

    public:
        Vector_3() {};
        Vector_3(Vector_T x, Vector_T y, Vector_T z) :
            m_x(x), m_y(y), m_z(z) {};

        template<typename Convert_T>
        operator Vector_3<Convert_T>()
        {
            static_assert(std::is_arithmetic<Convert_T>::value, "Vector: Cannot convert to a vector of this type");
            return Vector_3<Convert_T>((Convert_T)m_x, (Convert_T)m_y, (Convert_T)m_z);
        }

        Vector_T* operator[](uint32_t index)
        {
            if (index == 0)
            {
                return &m_x;
            }
            else if (index == 1)
            {
                return &m_y;
            }
            else if (index == 2)
            {
                return &m_z;
            }
            else
            {
                exit(EXIT_FAILURE);
            }
        }

        template<typename Scalar_T>
        Vector_3<Vector_T> operator+(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_3<Vector_T>(m_x + (Vector_T)value, m_y + (Vector_T)value, m_z + (Vector_T)value);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator+=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_3<Vector_T>(m_x + (Vector_T)value, m_y + (Vector_T)value, m_z + (Vector_T)value);
            return *this;
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator+(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_3<Vector_T>(m_x + (Vector_T)vector.m_x, m_y + (Vector_T)vector.m_y, m_z + (Vector_T)vector.m_z);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator+=(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_3<Vector_T>(m_x + (Vector_T)vector.m_x, m_y + (Vector_T)vector.m_y, m_z + (Vector_T)vector.m_z);
            return *this;
        }
        Vector_3<Vector_T> operator++()
        {
            *this = Vector_3<Vector_T>(m_x++, m_y++);
            return *this;
        }

        Vector_3<Vector_T> operator-()
        {
            return Vector_3<Vector_T>(-m_x, -m_y, -m_z);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator-(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_3<Vector_T>(m_x - (Vector_T)value, m_y - (Vector_T)value, m_z - (Vector_T)value);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator-=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_3<Vector_T>(m_x - (Vector_T)value, m_y - (Vector_T)value, m_z - (Vector_T)value);
            return *this;
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator-(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_3<Vector_T>(m_x - (Vector_T)vector.m_x, m_y - (Vector_T)vector.m_y, m_z - (Vector_T)vector.m_z);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator-=(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_3<Vector_T>(m_x - (Vector_T)vector.m_x, m_y - (Vector_T)vector.m_y, m_z - (Vector_T)vector.m_z);
            return *this;
        }
        Vector_3<Vector_T> operator--()
        {
            *this = Vector_3<Vector_T>(m_x--, m_y--);
            return *this;
        }

        template<typename Scalar_T>
        Vector_3<Vector_T> operator*(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_3<Vector_T>(m_x * (Vector_T)value, m_y * (Vector_T)value, m_z * (Vector_T)value);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator*=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_3<Vector_T>(m_x * (Vector_T)value, m_y * (Vector_T)value, m_z * (Vector_T)value);
            return *this;
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator*(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_3<Vector_T>(m_x * (Vector_T)vector.m_x, m_y * (Vector_T)vector.m_y, m_z * (Vector_T)vector.m_z);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator*=(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_3<Vector_T>(m_x * (Vector_T)vector.m_x, m_y * (Vector_T)vector.m_y, m_z * (Vector_T)vector.m_z);
            return *this;
        }

        template<typename Scalar_T>
        Vector_3<Vector_T> operator/(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            return Vector_3<Vector_T>(m_x / (Vector_T)value, m_y / (Vector_T)value, m_z / (Vector_T)value);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator/=(Scalar_T value)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Scalar type must be numeric");
            *this = Vector_3<Vector_T>(m_x / (Vector_T)value, m_y / (Vector_T)value, m_z / (Vector_T)value);
            return *this;
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator/(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            return Vector_3<Vector_T>(m_x / (Vector_T)vector.m_x, m_y / (Vector_T)vector.m_y, m_z / (Vector_T)vector.m_z);
        }
        template<typename Scalar_T>
        Vector_3<Vector_T> operator/=(Vector_3<Scalar_T> vector)
        {
            static_assert(std::is_arithmetic<Scalar_T>::value, "Vector: Vector type must be numeric");
            *this = Vector_3<Vector_T>(m_x / (Vector_T)vector.m_x, m_y / (Vector_T)vector.m_y, m_z / (Vector_T)vector.m_z);
            return *this;
        }

        Vector_3<Vector_T> Normalized()
        {
            return *this / sqrt((double)(m_x * m_x + m_y * m_y + m_z * m_z));
        }
        void Normalize()
        {
            *this /= sqrt((double)(m_x * m_x + m_y * m_y + m_z * m_z));
        }

        Vector_T Length()
        {
            return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
        }

        template<typename T>
        static double Dot(Vector_3<T> vector_a, Vector_3<T> vector_b)
        {
            return (double)vector_a.m_x * (double)vector_b.m_x + (double)vector_a.m_y * (double)vector_b.m_y + (double)vector_a.m_z * (double)vector_b.m_z;
        }
        template<typename T>
        static Vector_3<T> Cross(Vector_3<T> vector_a, Vector_3<T> vector_b)
        {
            return Vector_3<T>(vector_a.m_y * vector_b.m_z - vector_a.m_z * vector_b.m_y, vector_a.m_z * vector_b.m_x - vector_a.m_x * vector_b.m_z, vector_a.m_x * vector_b.m_y - vector_a.m_y * vector_b.m_x);
        }
    };
}    // namespace Cascade_Graphics