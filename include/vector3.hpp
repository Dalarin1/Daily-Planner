#pragma once
#include <cmath>

struct vector2
{
public:
    float X, Y;
    vector2(float _x, float _y) : X(_x), Y(_y) {}
    vector2() : X(0), Y(0) {}
    vector2 operator+(const vector2& other) { return vector2(X + other.X, Y + other.Y); }
    vector2 operator-(const vector2& other) { return vector2(X - other.X, Y - other.Y); }
    vector2 operator*(float num) { return vector2(X * num, Y * num); }
    vector2 operator/(float num) { return vector2(X / num, Y / num); }
    bool operator==(const vector2& other) { return (X == other.X && Y == other.Y); }
    bool operator!=(const vector2& other) { return (X != other.X || Y != other.Y); }
    float GetSquaredLength() { return X * X + Y * Y; }
    float GetLength() { return sqrt(X * X + Y * Y); }
    void Normalize()
    {
        float len = this->GetLength();
        X /= len;
        Y /= len;
    }
    vector2 NormalizedCopy()
    {
        float len = this->GetLength();
        return (*this) / len;
    }
};

struct vector3
{
public:
    float X;
    float Y;
    float Z;
    vector3(float _x, float _y, float _z) : X(_x), Y(_y), Z(_z) {}
    vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    vector3 operator+(const vector3& other) { return vector3(X + other.X, Y + other.Y, Z + other.Z); }
    vector3 operator-(const vector3& other) { return vector3(X - other.X, Y - other.Y, Z - other.Z); }
    vector3 operator/(float num) { return vector3(X / num, Y / num, Z / num); }
    vector3 operator*(float num) { return vector3(X * num, Y * num, Z * num); }
    inline bool operator==(const vector3& other) { return X == other.X && Y == other.Y && Z == other.Z; }
    inline bool operator!=(const vector3& other) { return !((*this) == other); }
    float GetSquaredLength() { return X * X + Y * Y + Z * Z; }
    float GetLength() { return sqrt(X * X + Y * Y + Z * Z); }
    void Normalize()
    {
        float len = this->GetLength();
        X /= len;
        Y /= len;
        Z /= len;
    }
    vector3 NormalizedCopy()
    {
        float len = this->GetLength();
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
    vector4 operator+(const vector4& other) { return vector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W); }
    vector4 operator-(const vector4& other) { return vector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W); }
    vector4 operator/(float num) { return vector4(X / num, Y / num, Z / num, W / num); }
    vector4 operator*(float num) { return vector4(X * num, Y * num, Z * num, W * num); }
    bool operator==(const vector4& other ) const { return X == other.X && Y == other.Y && Z == other.Z && W == other.W; }
    bool operator!=(const vector4& other) const { return !((*this) == other); }
    float GetSquaredLength() const { return X * X + Y * Y + Z * Z + W * W; }
    float GetLength() const { return sqrt(X * X + Y * Y + Z * Z + W * W); }
    void Normalize()
    {
        float len = this->GetLength();
        X /= len;
        Y /= len;
        Z /= len;
        W /= len;
    }
    vector4 NormalizedCopy()
    {
        float len = this->GetLength();
        return ((*this) / len);
    }
};

float dot(const vector2& a, const vector2& b)
{
    return (a.X * b.X + a.Y * b.Y);
}

float dot(const vector3& a, const vector3& b)
{
    return (a.X * b.X + a.Y * b.Y + a.Z * b.Z);
}
float dot(const vector4& a, const vector4& b)
{
    return (a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W);
}

template <typename T>
T clamp(T value, T min, T max)
{
    return value >= min ? (value <= max ? value : max) : min;
}

template <typename T>
bool inRange(T value, T min, T max)
{
    return value >= min && value <= max;
}