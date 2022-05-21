#pragma once

#include "vector_2.hpp"

namespace CascadeGraphics
{
    template <typename Matrix_T>
    class Matrix_2x2
    {
        static_assert(std::is_arithmetic<Matrix_T>::value, "Matrix: Matrix type must be numeric");

    public:
        Matrix_T m_x0;
        Matrix_T m_y0;
        Matrix_T m_x1;
        Matrix_T m_y1;

    public:
        Matrix_2x2(Matrix_T x0, Matrix_T y0, Matrix_T x1, Matrix_T y1) : m_x0(x0), m_y0(y0), m_x1(x1), m_y1(y1){};

        template <typename T>
        Matrix_2x2(Vector_2<T> v0, Vector_2<T> v1) : m_x0(v0.x), m_y0(v0.y), m_x1(v1.x), m_y1(v1.y){};

        template <typename Convert_T>
        operator Matrix_2x2<Convert_T>()
        {
            static_assert(std::is_arithmetic<Convert_T>::value, "Matrix: Cannot convert to a matrix of this type");
            return Matrix_2x2<Convert_T>((Convert_T)m_x0, (Convert_T)m_y0, (Convert_T)m_x1, (Convert_T)m_y1);
        }

        template <typename T>
        Matrix_2x2<Matrix_T> operator+(Matrix_2x2<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            return Matrix_2x2<Matrix_T>(m_x0 + (Matrix_T)matrix.m_x0, m_y0 + (Matrix_T)matrix.m_y0, m_x1 + (Matrix_T)matrix.m_x1, m_y1 + (Matrix_T)matrix.m_y1);
        }
        template <typename T>
        Matrix_2x2<Matrix_T> operator+=(Matrix_2x2<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            *this = Matrix_2x2<Matrix_T>(m_x0 + (Matrix_T)matrix.m_x0, m_y0 + (Matrix_T)matrix.m_y0, m_x1 + (Matrix_T)matrix.m_x1, m_y1 + (Matrix_T)matrix.m_y1);
            return *this;
        }

        template <typename T>
        Matrix_2x2<Matrix_T> operator-(Matrix_2x2<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            return Matrix_2x2<Matrix_T>(m_x0 - (Matrix_T)matrix.m_x0, m_y0 - (Matrix_T)matrix.m_y0, m_x1 - (Matrix_T)matrix.m_x1, m_y1 - (Matrix_T)matrix.m_y1);
        }
        template <typename T>
        Matrix_2x2<Matrix_T> operator-=(Matrix_2x2<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            *this = Matrix_2x2<Matrix_T>(m_x0 - (Matrix_T)matrix.m_x0, m_y0 - (Matrix_T)matrix.m_y0, m_x1 - (Matrix_T)matrix.m_x1, m_y1 - (Matrix_T)matrix.m_y1);
            return *this;
        }

        template <typename T>
        Matrix_2x2<Matrix_T> operator*(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            return Matrix_2x2<Matrix_T>(m_x0 * (Matrix_T)value, m_y0 * (Matrix_T)value, m_x1 * (Matrix_T)value, m_y1 * (Matrix_T)value);
        }
        template <typename T>
        Matrix_2x2<Matrix_T> operator*=(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            *this = Matrix_2x2<Matrix_T>(m_x0 * (Matrix_T)value, m_y0 * (Matrix_T)value, m_x1 * (Matrix_T)value, m_y1 * (Matrix_T)value);
            return *this;
        }
        template <typename T>
        Matrix_2x2<Matrix_T> operator*(Matrix_2x2<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            return Matrix_2x2<Matrix_T>(m_x0 * (Matrix_T)matrix.m_x0, m_y0 * (Matrix_T)matrix.m_y0, m_x1 * (Matrix_T)matrix.m_x1, m_y1 * (Matrix_T)matrix.m_y1);
        }
        template <typename T>
        Matrix_2x2<Matrix_T> operator*=(Matrix_2x2<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            *this = Matrix_2x2<Matrix_T>(m_x0 * (Matrix_T)matrix.m_x0, m_y0 * (Matrix_T)matrix.m_y0, m_x1 * (Matrix_T)matrix.m_x1, m_y1 * (Matrix_T)matrix.m_y1);
            return *this;
        }

        template <typename T>
        Matrix_2x2<Matrix_T> operator/(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            return Matrix_2x2<Matrix_T>(m_x0 / (Matrix_T)value, m_y0 / (Matrix_T)value, m_x1 / (Matrix_T)value, m_y1 / (Matrix_T)value);
        }
        template <typename T>
        Matrix_2x2<Matrix_T> operator/=(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            *this = Matrix_2x2<Matrix_T>(m_x0 / (Matrix_T)value, m_y0 / (Matrix_T)value, m_x1 / (Matrix_T)value, m_y1 / (Matrix_T)value);
            return *this;
        }
    };
} // namespace CascadeGraphics