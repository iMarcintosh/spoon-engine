#pragma once

#include <cmath>

namespace SpoonEngine {
    
    struct Vector2 {
        float x, y;
        
        Vector2() : x(0.0f), y(0.0f) {}
        Vector2(float x, float y) : x(x), y(y) {}
        Vector2(float scalar) : x(scalar), y(scalar) {}
        
        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }
        
        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }
        
        Vector2 operator*(float scalar) const {
            return Vector2(x * scalar, y * scalar);
        }
        
        Vector2 operator/(float scalar) const {
            return Vector2(x / scalar, y / scalar);
        }
        
        Vector2& operator+=(const Vector2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
        
        Vector2& operator-=(const Vector2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        
        Vector2& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }
        
        Vector2& operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }
        
        float Length() const {
            return std::sqrt(x * x + y * y);
        }
        
        float LengthSquared() const {
            return x * x + y * y;
        }
        
        Vector2 Normalize() const {
            float length = Length();
            if (length > 0.0f) {
                return *this / length;
            }
            return Vector2();
        }
        
        float Dot(const Vector2& other) const {
            return x * other.x + y * other.y;
        }
        
        static Vector2 Zero() { return Vector2(0.0f, 0.0f); }
        static Vector2 One() { return Vector2(1.0f, 1.0f); }
        static Vector2 Up() { return Vector2(0.0f, 1.0f); }
        static Vector2 Down() { return Vector2(0.0f, -1.0f); }
        static Vector2 Left() { return Vector2(-1.0f, 0.0f); }
        static Vector2 Right() { return Vector2(1.0f, 0.0f); }
    };
    
}