#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include <cmath>

namespace SpoonEngine {
    
    struct Matrix4 {
        float m[4][4];
        
        Matrix4() {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    m[i][j] = (i == j) ? 1.0f : 0.0f;
                }
            }
        }
        
        Matrix4(float diagonal) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    m[i][j] = (i == j) ? diagonal : 0.0f;
                }
            }
        }
        
        Matrix4 operator*(const Matrix4& other) const {
            Matrix4 result(0.0f);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    for (int k = 0; k < 4; k++) {
                        result.m[i][j] += m[i][k] * other.m[k][j];
                    }
                }
            }
            return result;
        }
        
        Vector4 operator*(const Vector4& vec) const {
            return Vector4(
                m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.w,
                m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.w,
                m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.w,
                m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.w
            );
        }
        
        static Matrix4 Identity() {
            return Matrix4(1.0f);
        }
        
        static Matrix4 Translate(const Vector3& translation) {
            Matrix4 result;
            result.m[0][3] = translation.x;
            result.m[1][3] = translation.y;
            result.m[2][3] = translation.z;
            return result;
        }
        
        static Matrix4 Scale(const Vector3& scale) {
            Matrix4 result;
            result.m[0][0] = scale.x;
            result.m[1][1] = scale.y;
            result.m[2][2] = scale.z;
            return result;
        }
        
        static Matrix4 RotateX(float angle) {
            Matrix4 result;
            float c = std::cos(angle);
            float s = std::sin(angle);
            result.m[1][1] = c;
            result.m[1][2] = -s;
            result.m[2][1] = s;
            result.m[2][2] = c;
            return result;
        }
        
        static Matrix4 RotateY(float angle) {
            Matrix4 result;
            float c = std::cos(angle);
            float s = std::sin(angle);
            result.m[0][0] = c;
            result.m[0][2] = s;
            result.m[2][0] = -s;
            result.m[2][2] = c;
            return result;
        }
        
        static Matrix4 RotateZ(float angle) {
            Matrix4 result;
            float c = std::cos(angle);
            float s = std::sin(angle);
            result.m[0][0] = c;
            result.m[0][1] = -s;
            result.m[1][0] = s;
            result.m[1][1] = c;
            return result;
        }
        
        static Matrix4 Perspective(float fov, float aspect, float near, float far) {
            Matrix4 result(0.0f);
            float tanHalfFov = std::tan(fov / 2.0f);
            
            result.m[0][0] = 1.0f / (aspect * tanHalfFov);
            result.m[1][1] = 1.0f / tanHalfFov;
            result.m[2][2] = -(far + near) / (far - near);
            result.m[2][3] = -(2.0f * far * near) / (far - near);
            result.m[3][2] = -1.0f;
            
            return result;
        }
        
        static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far) {
            Matrix4 result;
            
            result.m[0][0] = 2.0f / (right - left);
            result.m[1][1] = 2.0f / (top - bottom);
            result.m[2][2] = -2.0f / (far - near);
            result.m[0][3] = -(right + left) / (right - left);
            result.m[1][3] = -(top + bottom) / (top - bottom);
            result.m[2][3] = -(far + near) / (far - near);
            
            return result;
        }
        
        static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
            Vector3 f = (center - eye).Normalize();
            Vector3 s = f.Cross(up).Normalize();
            Vector3 u = s.Cross(f);
            
            Matrix4 result;
            result.m[0][0] = s.x;
            result.m[1][0] = s.y;
            result.m[2][0] = s.z;
            result.m[0][1] = u.x;
            result.m[1][1] = u.y;
            result.m[2][1] = u.z;
            result.m[0][2] = -f.x;
            result.m[1][2] = -f.y;
            result.m[2][2] = -f.z;
            result.m[0][3] = -s.Dot(eye);
            result.m[1][3] = -u.Dot(eye);
            result.m[2][3] = f.Dot(eye);
            
            return result;
        }
    };
    
}