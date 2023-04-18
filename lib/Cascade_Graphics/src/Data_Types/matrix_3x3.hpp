#pragma once

#include "vector_3.hpp"

namespace Cascade_Graphics
{
    template<typename Matrix_T>
    class Matrix_3x3
    {
        static_assert(std::is_arithmetic<Matrix_T>::value, "Matrix: Matrix type must be numeric");

    public:
        Matrix_T m_x0;
        Matrix_T m_y0;
        Matrix_T m_z0;
        Matrix_T m_x1;
        Matrix_T m_y1;
        Matrix_T m_z1;
        Matrix_T m_x2;
        Matrix_T m_y2;
        Matrix_T m_z2;

    public:
        Matrix_3x3(Matrix_T x0, Matrix_T y0, Matrix_T z0, Matrix_T x1, Matrix_T y1, Matrix_T z1, Matrix_T x2, Matrix_T y2, Matrix_T z2) :
            m_x0(x0), m_y0(y0), m_z0(z0), m_x1(x1), m_y1(y1), m_z1(z1), m_x2(x2), m_y2(y2), m_z2(z2) {};

        template<typename T>
        Matrix_3x3(Vector_3<T> v0, Vector_3<T> v1, Vector_3<T> v2) :
            m_x0(v0.m_x), m_y0(v0.m_y), m_z0(v0.m_z), m_x1(v1.m_x), m_y1(v1.m_y), m_z1(v1.m_z), m_x2(v2.m_x), m_y2(v2.m_y), m_z2(v2.m_z) {}

        template<typename Convert_T>
        operator Matrix_3x3<Convert_T>()
        {
            static_assert(std::is_arithmetic<Convert_T>::value, "Matrix: Cannot convert to a matrix of this type.");
            return Matrix_3x3<Convert_T>((Convert_T)m_x0, (Convert_T)m_y0, (Convert_T)m_z0, (Convert_T)m_x1, (Convert_T)m_y1, (Convert_T)m_z1, (Convert_T)m_x2, (Convert_T)m_y2, (Convert_T)m_z2);
        }

        template<typename T>
        Matrix_3x3<Matrix_T> operator+(Matrix_3x3<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            return Matrix_3x3<Matrix_T>(m_x0 + (Matrix_T)matrix.m_x0, m_y0 + (Matrix_T)matrix.m_y0, m_z0 + (Matrix_T)matrix.m_z0, m_x1 + (Matrix_T)matrix.m_x1, m_y1 + (Matrix_T)matrix.m_y1, m_z1 + (Matrix_T)matrix.m_z1, m_x2 + (Matrix_T)matrix.m_x2, m_y2 + (Matrix_T)matrix.m_y2, m_z2 + (Matrix_T)matrix.m_z2);
        }
        template<typename T>
        Matrix_3x3<Matrix_T> operator+=(Matrix_3x3<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            *this = Matrix_3x3<Matrix_T>(m_x0 + (Matrix_T)matrix.m_x0, m_y0 + (Matrix_T)matrix.m_y0, m_z0 + (Matrix_T)matrix.m_z0, m_x1 + (Matrix_T)matrix.m_x1, m_y1 + (Matrix_T)matrix.m_y1, m_z1 + (Matrix_T)matrix.m_z1, m_x2 + (Matrix_T)matrix.m_x2, m_y2 + (Matrix_T)matrix.m_y2, m_z2 + (Matrix_T)matrix.m_z2);
            return *this;
        }

        template<typename T>
        Matrix_3x3<Matrix_T> operator-(Matrix_3x3<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            return Matrix_3x3<Matrix_T>(m_x0 - (Matrix_T)matrix.m_x0, m_y0 - (Matrix_T)matrix.m_y0, m_z0 - (Matrix_T)matrix.m_z0, m_x1 - (Matrix_T)matrix.m_x1, m_y1 - (Matrix_T)matrix.m_y1, m_z1 - (Matrix_T)matrix.m_z1, m_x2 - (Matrix_T)matrix.m_x2, m_y2 - (Matrix_T)matrix.m_y2, m_z2 - (Matrix_T)matrix.m_z2);
        }
        template<typename T>
        Matrix_3x3<Matrix_T> operator-=(Matrix_3x3<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            *this = Matrix_3x3<Matrix_T>(m_x0 - (Matrix_T)matrix.m_x0, m_y0 - (Matrix_T)matrix.m_y0, m_z0 - (Matrix_T)matrix.m_z0, m_x1 - (Matrix_T)matrix.m_x1, m_y1 - (Matrix_T)matrix.m_y1, m_z1 - (Matrix_T)matrix.m_z1, m_x2 - (Matrix_T)matrix.m_x2, m_y2 - (Matrix_T)matrix.m_y2, m_z2 - (Matrix_T)matrix.m_z2);
            return *this;
        }

        template<typename T>
        Matrix_3x3<Matrix_T> operator*(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            return Matrix_3x3<Matrix_T>(m_x0 * (Matrix_T)value, m_y0 * (Matrix_T)value, m_z0 * (Matrix_T)value, m_x1 * (Matrix_T)value, m_y1 * (Matrix_T)value, m_z1 * (Matrix_T)value, m_x2 * (Matrix_T)value, m_y2 * (Matrix_T)value, m_z2 * (Matrix_T)value);
        }
        template<typename T>
        Matrix_3x3<Matrix_T> operator*=(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            *this = Matrix_3x3<Matrix_T>(m_x0 * (Matrix_T)value, m_y0 * (Matrix_T)value, m_z0 * (Matrix_T)value, m_x1 * (Matrix_T)value, m_y1 * (Matrix_T)value, m_z1 * (Matrix_T)value, m_x2 * (Matrix_T)value, m_y2 * (Matrix_T)value, m_z2 * (Matrix_T)value);
            return *this;
        }
        template<typename T>
        Matrix_3x3<Matrix_T> operator*(Matrix_3x3<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            return Matrix_3x3<Matrix_T>(m_x0 * (Matrix_T)matrix.m_x0, m_y0 * (Matrix_T)matrix.m_y0, m_z0 * (Matrix_T)matrix.m_z0, m_x1 * (Matrix_T)matrix.m_x1, m_y1 * (Matrix_T)matrix.m_y1, m_z1 * (Matrix_T)matrix.m_z1, m_x2 * (Matrix_T)matrix.m_x2, m_y2 * (Matrix_T)matrix.m_y2, m_z2 * (Matrix_T)matrix.m_z2);
        }
        template<typename T>
        Matrix_3x3<Matrix_T> operator*=(Matrix_3x3<T> matrix)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Matrix type must be numeric");
            *this = Matrix_3x3<Matrix_T>(m_x0 * (Matrix_T)matrix.m_x0, m_y0 * (Matrix_T)matrix.m_y0, m_z0 * (Matrix_T)matrix.m_z0, m_x1 * (Matrix_T)matrix.m_x1, m_y1 * (Matrix_T)matrix.m_y1, m_z1 * (Matrix_T)matrix.m_z1, m_x2 * (Matrix_T)matrix.m_x2, m_y2 * (Matrix_T)matrix.m_y2, m_z2 * (Matrix_T)matrix.m_z2);
            return *this;
        }

        template<typename T>
        Matrix_3x3<Matrix_T> operator/(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            return Matrix_3x3<Matrix_T>(m_x0 / (Matrix_T)value, m_y0 / (Matrix_T)value, m_z0 / (Matrix_T)value, m_x1 / (Matrix_T)value, m_y1 / (Matrix_T)value, m_z1 / (Matrix_T)value, m_x2 / (Matrix_T)value, m_y2 / (Matrix_T)value, m_z2 / (Matrix_T)value);
        }
        template<typename T>
        Matrix_3x3<Matrix_T> operator/=(T value)
        {
            static_assert(std::is_arithmetic<T>::value, "Matrix: Scalar type must be numeric");
            *this = Matrix_3x3<Matrix_T>(m_x0 / (Matrix_T)value, m_y0 / (Matrix_T)value, m_z0 / (Matrix_T)value, m_x1 / (Matrix_T)value, m_y1 / (Matrix_T)value, m_z1 / (Matrix_T)value, m_x2 / (Matrix_T)value, m_y2 / (Matrix_T)value, m_z2 / (Matrix_T)value);
            return *this;
        }

        double Determinant()
        {
            return (m_x0 * m_y1 * m_z2) + (m_y0 * m_z1 * m_x2) + (m_z0 * m_x1 * m_y2) - (m_z0 * m_y1 * m_x2) - (m_x0 * m_z1 * m_y2) - (m_y0 * m_x1 * m_z2);
        }

        template<typename T>
        static Matrix_3x3<T> Invert(Matrix_3x3<T> matrix)
        {
            double determinant = matrix.Determinant();
            if (determinant == 0)
            {
                return Matrix_3x3<T>(1, 0, 0, 0, 1, 0, 0, 0, 1);
            }
            determinant = 1.0 / determinant;

            Matrix_3x3<T> matrix_of_minors = Matrix_3x3<T>((matrix.m_y1 * matrix.m_z2 - matrix.m_z1 * matrix.m_y2), -(matrix.x1 * matrix.m_z2 - matrix.m_z1 * matrix.m_x2), (matrix.x1 * matrix.y2 - matrix.y1 * matrix.x2), -(matrix.y0 * matrix.m_z2 - matrix.m_z0 * matrix.m_y2), (matrix.m_x0 * matrix.m_z2 - matrix.m_z0 * matrix.m_x2), -(matrix.x0 * matrix.y2 - matrix.m_y0 * matrix.m_x2), (matrix.m_y0 * matrix.m_z1 - matrix.m_z0 * matrix.m_y1), -(matrix.m_x0 * matrix.m_z1 - matrix.m_z0 * matrix.m_x1), (matrix.m_x0 * matrix.m_y1 - matrix.m_y0 * matrix.m_x1));
            Matrix_3x3<T> matrix_of_minors_transposed = Matrix_3x3<T>(matrix_of_minors.m_x0 * determinant, matrix_of_minors.m_x1 * determinant, matrix_of_minors.m_x2 * determinant, matrix_of_minors.m_y0 * determinant, matrix_of_minors.m_y1 * determinant, matrix_of_minors.m_y2 * determinant, matrix_of_minors.m_z0 * determinant, matrix_of_minors.m_z1 * determinant, matrix_of_minors.m_z2 * determinant);

            return matrix_of_minors_transposed;
        }
    };
}    // namespace Cascade_Graphics