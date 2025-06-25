#pragma once
#include <cmath>

struct vector3
{
public:
    float X;
    float Y;
    float Z;
    vector3(float _x, float _y, float _z) : X(_x), Y(_y), Z(_z)
    {
    }
    vector3() : X(0.0f), Y(0.0f), Z(0.0f)
    {
    }
    vector3 operator+(vector3 other)
    {
        return vector3(X + other.X, Y + other.Y, Z + other.Z);
    }
    vector3 operator-(vector3 other)
    {
        return vector3(X - other.X, Y - other.Y, Z - other.Z);
    }
    vector3 operator/(float num)
    {
        return vector3(X / num, Y / num, Z / num);
    }
    vector3 operator*(float num)
    {
        return vector3(X * num, Y * num, Z * num);
    }
    inline bool operator==(vector3 other)
    {
        return X == other.X && Y == other.Y && Z == other.Z;
    }
    inline bool operator!=(vector3 other)
    {
        return !((*this) == other);
    }
    float GetSquaredLength()
    {
        return X * X + Y * Y + Z * Z;
    }
    float GetLengt()
    {
        return sqrt(X * X + Y * Y + Z * Z);
    }
    void Normalize()
    {
        float len = this->GetLengt();
        X /= len;
        Y /= len;
        Z /= len;
    }
    vector3 NormalizedCopy()
    {
        float len = this->GetLengt();
        return ((*this) / len);
    }
};
struct vector4
{
public:
    float X;
    float Y;
    float Z;
    float W;
    vector4(float _x, float _y, float _z, float _w) : X(_x), Y(_y), Z(_z), W(_w)
    {
    }
    vector4() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
    {
    }
    vector4 operator+(vector4 other)
    {
        return vector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
    }
    vector4 operator-(vector4 other)
    {
        return vector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
    }
    vector4 operator/(float num)
    {
        return vector4(X / num, Y / num, Z / num, W / num);
    }
    vector4 operator*(float num)
    {
        return vector4(X * num, Y * num, Z * num, W * num);
    }
    bool operator==(vector4 other)
    {
        return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
    }
    bool operator!=(vector4 other)
    {
        return !((*this) == other);
    }
    float GetSquaredLength()
    {
        return X * X + Y * Y + Z * Z + W * W;
    }
    float GetLengt()
    {
        return sqrt(X * X + Y * Y + Z * Z + W * W);
    }
    void Normalize()
    {
        float len = this->GetLengt();
        X /= len;
        Y /= len;
        Z /= len;
        W /= len;
    }
    vector4 NormalizedCopy()
    {
        float len = this->GetLengt();
        return ((*this) / len);
    }
};

float dot(vector3 a, vector3 b)
{
    return sqrt(a.X * b.X + a.Y * b.Y + a.Z * b.Z);
}
float dot(vector4 a, vector4 b)
{
    return sqrt(a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W);
}

template <typename T>
T clamp(T value, T min, T max)
{
    return value >= min ? (value <= max ? value : max) : min;
}

template <typename T>
bool inRange(T value, T min, T max) {
    return value >= min && value <= max;
}