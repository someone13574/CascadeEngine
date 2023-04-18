#pragma once

#include "vector_4.hpp"

namespace Cascade_Graphics
{
    template<typename Matrix_T>
    class Matrix_4x4
    {
        static_assert(std::is_arithmetic<Matrix_T>::value, "Graphics: Matrix type must be numeric");

    public:
        Matrix_T m_x0;
        Matrix_T m_x1;
        Matrix_T m_x2;
        Matrix_T m_x3;
        Matrix_T m_y0;
        Matrix_T m_y1;
        Matrix_T m_y2;
        Matrix_T m_y3;
        Matrix_T m_z0;
        Matrix_T m_z1;
        Matrix_T m_z2;
        Matrix_T m_z3;
        Matrix_T m_w0;
        Matrix_T m_w1;
        Matrix_T m_w2;
        Matrix_T m_w3;

    public:
        Matrix_4x4(Matrix_T x0, Matrix_T y0, Matrix_T z0, Matrix_T w0, Matrix_T x1, Matrix_T y1, Matrix_T z1, Matrix_T w1, Matrix_T x2, Matrix_T y2, Matrix_T z2, Matrix_T w2, Matrix_T x3, Matrix_T y3, Matrix_T z3, Matrix_T w3) :
            m_x0(x0), m_y0(y0), m_z0(z0), m_w0(w0), m_x1(x1), m_y1(y1), m_z1(z1), m_w1(w1), m_x2(x2), m_y2(y2), m_z2(x2), m_w2(w2), m_x3(x3), m_y3(y3), m_z3(z3), m_w3(w3) {};

        template<typename T>
        Matrix_4x4(Vector_4<T> v0, Vector_4<T> v1, Vector_4<T> v2, Vector_4<T> v3) :
            m_x0(v0.m_x), m_y0(v0.m_y), m_z0(v0.m_z), m_w0(v0.m_w), m_x1(v1.m_x), m_y1(v1.m_y), m_z1(v1.m_z), m_w0(v1.m_w), m_x2(v2.m_x), m_y2(v2.m_y), m_z2(v2.m_z), m_w0(v2.m_w), m_x2(v3.m_x), m_y2(v3.m_y), m_z2(v3.m_z), m_w0(v3.m_w) {};

        template<typename Convert_T>
        operator Matrix_4x4<Convert_T>()
        {
            static_assert(std::is_arithmetic<Convert_T>::value, "Graphics: Cannot convert to a matrix of this type.");
            return Matrix_4x4<Convert_T>((Convert_T)m_x0, (Convert_T)m_y0, (Convert_T)m_z0, (Convert_T)m_w0, (Convert_T)m_x1, (Convert_T)m_y1, (Convert_T)m_z1, (Convert_T)m_w1, (Convert_T)m_x2, (Convert_T)m_y2, (Convert_T)m_z2, (Convert_T)m_w2, (Convert_T)m_x3, (Convert_T)m_y3, (Convert_T)m_z3, (Convert_T)m_w3);
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator+(Matrix_4x4<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Matrix type must be numeric");
            return Matrix_4x4<Matrix_T>(m_x0 + (Matrix_T)matrix.m_x0, m_y0 + (Matrix_T)matrix.m_y0, m_z0 + (Matrix_T)matrix.m_z0, m_w0 + (Matrix_T)matrix.m_w0, m_x1 + (Matrix_T)matrix.m_x1, m_y1 + (Matrix_T)matrix.m_y1, m_z1 + (Matrix_T)matrix.m_z1, m_w1 + (Matrix_T)matrix.m_w1, m_x2 + (Matrix_T)matrix.m_x2, m_y2 + (Matrix_T)matrix.m_y2, m_z2 + (Matrix_T)matrix.m_z2, m_w2 + (Matrix_T)matrix.m_w2, m_x3 + (Matrix_T)matrix.m_x3, m_y3 + (Matrix_T)matrix.m_y3, m_z3 + (Matrix_T)matrix.m_z3, m_w3 + (Matrix_T)matrix.m_w3);
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator+=(Matrix_4x4<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Matrix type must be numeric");
            *this = Matrix_4x4<Matrix_T>(m_x0 + (Matrix_T)matrix.m_x0, m_y0 + (Matrix_T)matrix.m_y0, m_z0 + (Matrix_T)matrix.m_z0, m_w0 + (Matrix_T)matrix.m_w0, m_x1 + (Matrix_T)matrix.m_x1, m_y1 + (Matrix_T)matrix.m_y1, m_z1 + (Matrix_T)matrix.m_z1, m_w1 + (Matrix_T)matrix.m_w1, m_x2 + (Matrix_T)matrix.m_x2, m_y2 + (Matrix_T)matrix.m_y2, m_z2 + (Matrix_T)matrix.m_z2, m_w2 + (Matrix_T)matrix.m_w2, m_x3 + (Matrix_T)matrix.m_x3, m_y3 + (Matrix_T)matrix.m_y3, m_z3 + (Matrix_T)matrix.m_z3, m_w3 + (Matrix_T)matrix.m_w3);

            return *this;
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator-(Matrix_4x4<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Matrix type must be numeric");
            return Matrix_4x4<Matrix_T>(m_x0 - (Matrix_T)matrix.m_x0, m_y0 - (Matrix_T)matrix.m_y0, m_z0 - (Matrix_T)matrix.m_z0, m_w0 - (Matrix_T)matrix.m_w0, m_x1 - (Matrix_T)matrix.m_x1, m_y1 - (Matrix_T)matrix.m_y1, m_z1 - (Matrix_T)matrix.m_z1, m_w1 - (Matrix_T)matrix.m_w1, m_x2 - (Matrix_T)matrix.m_x2, m_y2 - (Matrix_T)matrix.m_y2, m_z2 - (Matrix_T)matrix.m_z2, m_w2 - (Matrix_T)matrix.m_w2, m_x3 - (Matrix_T)matrix.m_x3, m_y3 - (Matrix_T)matrix.m_y3, m_z3 - (Matrix_T)matrix.m_z3, m_w3 - (Matrix_T)matrix.m_w3);
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator-=(Matrix_4x4<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Matrix type must be numeric");
            *this = Matrix_4x4<Matrix_T>(m_x0 - (Matrix_T)matrix.m_x0, m_y0 - (Matrix_T)matrix.m_y0, m_z0 - (Matrix_T)matrix.m_z0, m_w0 - (Matrix_T)matrix.m_w0, m_x1 - (Matrix_T)matrix.m_x1, m_y1 - (Matrix_T)matrix.m_y1, m_z1 - (Matrix_T)matrix.m_z1, m_w1 - (Matrix_T)matrix.m_w1, m_x2 - (Matrix_T)matrix.m_x2, m_y2 - (Matrix_T)matrix.m_y2, m_z2 - (Matrix_T)matrix.m_z2, m_w2 - (Matrix_T)matrix.m_w2, m_x3 - (Matrix_T)matrix.m_x3, m_y3 - (Matrix_T)matrix.m_y3, m_z3 - (Matrix_T)matrix.m_z3, m_w3 - (Matrix_T)matrix.m_w3);

            return *this;
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator*(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Scalar type must be numeric");
            return Matrix_4x4<Matrix_T>(m_x0 * (Matrix_T)value, m_y0 * (Matrix_T)value, m_z0 * (Matrix_T)value, m_w0 * (Matrix_T)value, m_x1 * (Matrix_T)value, m_y1 * (Matrix_T)value, m_z1 * (Matrix_T)value, m_w1 * (Matrix_T)value, m_x2 * (Matrix_T)value, m_y2 * (Matrix_T)value, m_z2 * (Matrix_T)value, m_w2 * (Matrix_T)value, m_x3 * (Matrix_T)value, m_y3 * (Matrix_T)value, m_z3 * (Matrix_T)value, m_w3 * (Matrix_T)value);
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator*=(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Scalar type must be numeric");
            *this = Matrix_4x4<Matrix_T>(m_x0 * (Matrix_T)value, m_y0 * (Matrix_T)value, m_z0 * (Matrix_T)value, m_w0 * (Matrix_T)value, m_x1 * (Matrix_T)value, m_y1 * (Matrix_T)value, m_z1 * (Matrix_T)value, m_w1 * (Matrix_T)value, m_x2 * (Matrix_T)value, m_y2 * (Matrix_T)value, m_z2 * (Matrix_T)value, m_w2 * (Matrix_T)value, m_x3 * (Matrix_T)value, m_y3 * (Matrix_T)value, m_z3 * (Matrix_T)value, m_w3 * (Matrix_T)value);

            return *this;
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator/(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Scalar type must be numeric");
            return Matrix_4x4<Matrix_T>(m_x0 / (Matrix_T)value, m_y0 / (Matrix_T)value, m_z0 / (Matrix_T)value, m_w0 / (Matrix_T)value, m_x1 / (Matrix_T)value, m_y1 / (Matrix_T)value, m_z1 / (Matrix_T)value, m_w1 / (Matrix_T)value, m_x2 / (Matrix_T)value, m_y2 / (Matrix_T)value, m_z2 / (Matrix_T)value, m_w2 / (Matrix_T)value, m_x3 / (Matrix_T)value, m_y3 / (Matrix_T)value, m_z3 / (Matrix_T)value, m_w3 / (Matrix_T)value);
        }

        template<typename T>
        Matrix_4x4<Matrix_T> operator/=(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Graphics: Scalar type must be numeric");
            *this = Matrix_4x4<Matrix_T>(m_x0 / (Matrix_T)value, m_y0 / (Matrix_T)value, m_z0 / (Matrix_T)value, m_w0 / (Matrix_T)value, m_x1 / (Matrix_T)value, m_y1 / (Matrix_T)value, m_z1 / (Matrix_T)value, m_w1 / (Matrix_T)value, m_x2 / (Matrix_T)value, m_y2 / (Matrix_T)value, m_z2 / (Matrix_T)value, m_w2 / (Matrix_T)value, m_x3 / (Matrix_T)value, m_y3 / (Matrix_T)value, m_z3 / (Matrix_T)value, m_w3 / (Matrix_T)value);

            return *this;
        }
    };
}    // namespace Cascade_Graphics