#pragma once

#include <cmath>

namespace SpoonEngine {
    
    struct Vector4 {
        float x, y, z, w;
        
        Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        Vector4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        
        Vector4 operator+(const Vector4& other) const {
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
        }
        
        Vector4 operator-(const Vector4& other) const {
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
        }
        
        Vector4 operator*(float scalar) const {
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
        }
        
        Vector4 operator/(float scalar) const {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }
        
        Vector4& operator+=(const Vector4& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }
        
        Vector4& operator-=(const Vector4& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }
        
        Vector4& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }
        
        Vector4& operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }
        
        float Length() const {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }
        
        float LengthSquared() const {
            return x * x + y * y + z * z + w * w;
        }
        
        Vector4 Normalize() const {
            float length = Length();
            if (length > 0.0f) {
                return *this / length;
            }
            return Vector4();
        }
        
        float Dot(const Vector4& other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }
        
        static Vector4 Zero() { return Vector4(0.0f, 0.0f, 0.0f, 0.0f); }
        static Vector4 One() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
    };
    
}