#pragma once
#include "vector3.h"


struct Color
{
public:
    float Red, Green, Blue;

    Color(float red = 255.0f, float green = 255.0f, float blue = 255.0f)
        : Red(clamp(red, 0.0f, 255.0f)),
          Green(clamp(green, 0.0f, 255.0f)),
          Blue(clamp(blue, 0.0f, 255.0f)) {}

    Color(vector3 rgb)
        : Red(clamp(rgb.X, 0.0f, 255.0f)),
          Green(clamp(rgb.Y, 0.0f, 255.0f)),
          Blue(clamp(rgb.Z, 0.0f, 255.0f)) {}

    vector3 to_vector3() const {
        return vector3(Red, Green, Blue);
    }

    /// @brief Осветляет цвет на amount процентов
    /// @param amount в промежутке от 0 до 100
    Color Lighten(float amount = 10.0f) const {
        // Нормализуем проценты в диапазон [0, 1]
        float factor = clamp(amount, 0.0f, 100.0f) / 100.0f;
        
        return Color(
            Red + (255.0f - Red) * factor,
            Green + (255.0f - Green) * factor,
            Blue + (255.0f - Blue) * factor
        );
    }

    /// @brief Затемняет цвет на amount процентов
    /// @param amount в промежутке от 0 до 100
    Color Darken(float amount = 10.0f) const {
        float factor = clamp(amount, 0.0f, 100.0f) / 100.0f;
        
        return Color(
            Red - Red * factor,
            Green - Green * factor,
            Blue - Blue * factor
        );
    }

    /// @brief Линейная интерполяция между цветами
    Color Lerp(const Color& target, float t) const {
        t = clamp(t, 0.0f, 1.0f);
        return Color(
            Red + (target.Red - Red) * t,
            Green + (target.Green - Green) * t,
            Blue + (target.Blue - Blue) * t
        );
    }
};

class Triangle
{
public:
    vector3 Points[3];
    Triangle(vector3 a, vector3 b, vector3 c)
    {
        Points[0] = a;
        Points[1] = b;
        Points[2] = c;
    }
};

class Line
{
public:
    vector3 Points[2];
    Line(vector3 start, vector3 end)
    {
        Points[0] = start;
        Points[1] = end;
    }
};

class Rectangle
{
public:
    vector3 Location; // Левый верхний угол
    vector3 Points[4];
    vector3 Size; // Ширина и высота (положительные значения)

    Rectangle(vector3 location = vector3(0, 0, 0), vector3 size = vector3(0, 0, 0))
        : Location(location), Size(size)
    {
        Update();
    }

    void Update()
    {

        Points[0] = Location;                                             // Левый верхний
        Points[1] = vector3(Location.X + Size.X, Location.Y, 0);          // Правый верхний
        Points[2] = vector3(Location.X + Size.X, Location.Y - Size.Y, 0); // Правый нижний
        Points[3] = vector3(Location.X, Location.Y - Size.Y, 0);          // Левый нижний
    }

    bool Contains(const vector3 &point) const
    {
        return inRange(point.X, Location.X, Location.X + Size.X) &&
               inRange(point.Y, Location.Y - Size.Y, Location.Y);
    }
};
class Circle
{
    vector3 Position;
    float Radius;
    Circle(vector3 _pos = vector3(0, 0, 0), float _radius = 1.0f) : Position(_pos), Radius(_radius)
    {
    }
};
