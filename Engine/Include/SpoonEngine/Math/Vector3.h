#pragma once

#include <cmath>

namespace SpoonEngine {
    
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
        
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }
        
        Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        
        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
        
        Vector3 operator/(float scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }
        
        Vector3& operator+=(const Vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        
        Vector3& operator-=(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        
        Vector3& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        
        Vector3& operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }
        
        float Length() const {
            return std::sqrt(x * x + y * y + z * z);
        }
        
        float LengthSquared() const {
            return x * x + y * y + z * z;
        }
        
        Vector3 Normalize() const {
            float length = Length();
            if (length > 0.0f) {
                return *this / length;
            }
            return Vector3();
        }
        
        float Dot(const Vector3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }
        
        Vector3 Cross(const Vector3& other) const {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }
        
        bool operator==(const Vector3& other) const {
            const float epsilon = 1e-6f;
            return std::abs(x - other.x) < epsilon && 
                   std::abs(y - other.y) < epsilon && 
                   std::abs(z - other.z) < epsilon;
        }
        
        bool operator!=(const Vector3& other) const {
            return !(*this == other);
        }
        
        static Vector3 Zero() { return Vector3(0.0f, 0.0f, 0.0f); }
        static Vector3 One() { return Vector3(1.0f, 1.0f, 1.0f); }
        static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }
        static Vector3 Down() { return Vector3(0.0f, -1.0f, 0.0f); }
        static Vector3 Left() { return Vector3(-1.0f, 0.0f, 0.0f); }
        static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }
        static Vector3 Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
        static Vector3 Backward() { return Vector3(0.0f, 0.0f, -1.0f); }
    };
    
}