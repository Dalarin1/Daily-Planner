#pragma once
#include <cstring>
#include "vector3.hpp"

class mat4 {
public:
    
    float m[16];

    
    mat4() { identity(); }
    mat4(float diagonal) { identity(diagonal); }
    mat4(const float* data) { memcpy(m, data, 16 * sizeof(float)); }

        void identity(float value = 1.0f) {
        memset(m, 0, 16 * sizeof(float));
        m[0] = m[5] = m[10] = m[15] = value;
    }

    float& operator[](int index) { return m[index]; }
    const float& operator[](int index) const { return m[index]; }

    
    mat4 operator*(const mat4& right) const {
        mat4 result(0.0f);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    result[i * 4 + j] += m[i * 4 + k] * right[k * 4 + j];
                }
            }
        }
        return result;
    }

    vector4 operator*(const vector4& v) const {
        return vector4(
            m[0]*v.X + m[4]*v.Y + m[8]*v.Z + m[12]*v.W,
            m[1]*v.X + m[5]*v.Y + m[9]*v.Z + m[13]*v.W,
            m[2]*v.X + m[6]*v.Y + m[10]*v.Z + m[14]*v.W,
            m[3]*v.X + m[7]*v.Y + m[11]*v.Z + m[15]*v.W
        );
    }

    // Транспонирование
    mat4 transpose() const {
        mat4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[j * 4 + i] = m[i * 4 + j];
            }
        }
        return result;
    }

    // Создание матрицы переноса
    static mat4 translate(float x, float y, float z) {
        mat4 result(1.0f);
        result[12] = x;
        result[13] = y;
        result[14] = z;
        return result;
    }

    // Создание матрицы масштабирования
    static mat4 scale(float x, float y, float z) {
        mat4 result(1.0f);
        result[0] = x;
        result[5] = y;
        result[10] = z;
        return result;
    }

    // Создание матрицы вращения (в радианах)
    static mat4 rotate(float angle, float x, float y, float z) {
        float c = cos(angle);
        float s = sin(angle);
        float t = 1.0f - c;
        float len = sqrt(x*x + y*y + z*z);
        if (len != 1.0f) {
            x /= len;
            y /= len;
            z /= len;
        }

        mat4 result(1.0f);
        result[0] = t*x*x + c;
        result[1] = t*x*y + s*z;
        result[2] = t*x*z - s*y;
        
        result[4] = t*x*y - s*z;
        result[5] = t*y*y + c;
        result[6] = t*y*z + s*x;
        
        result[8] = t*x*z + s*y;
        result[9] = t*y*z - s*x;
        result[10] = t*z*z + c;
        
        return result;
    }

    // Ортографическая проекция
    static mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
        mat4 result(1.0f);
        result[0] = 2.0f / (right - left);
        result[5] = 2.0f / (top - bottom);
        result[10] = -2.0f / (far - near);
        
        result[12] = -(right + left) / (right - left);
        result[13] = -(top + bottom) / (top - bottom);
        result[14] = -(far + near) / (far - near);
        return result;
    }

    // Перспективная проекция
    static mat4 perspective(float fov, float aspect, float near, float far) {
        float tanHalfFov = tan(fov / 2.0f);
        
        mat4 result(0.0f);
        result[0] = 1.0f / (aspect * tanHalfFov);
        result[5] = 1.0f / tanHalfFov;
        result[10] = -(far + near) / (far - near);
        result[11] = -1.0f;
        result[14] = -(2.0f * far * near) / (far - near);
        return result;
    }

    // Видовая матрица
    static mat4 lookAt(const vector3& eye, const vector3& center, const vector3& up) {
        vector3 f = (const_cast<vector3&>(center) - const_cast<vector3&>(eye)).NormalizedCopy();
        vector3 s = cross(f, up).NormalizedCopy();
        vector3 u = cross(s, f);
        
        mat4 result(1.0f);
        result[0] = s.X;
        result[4] = s.Y;
        result[8] = s.Z;
        
        result[1] = u.X;
        result[5] = u.Y;
        result[9] = u.Z;
        
        result[2] = -f.X;
        result[6] = -f.Y;
        result[10] = -f.Z;
        
        result[12] = -dot(s, eye);
        result[13] = -dot(s, eye);
        result[14] = dot(f, eye);
        return result;
    }
};
