#pragma once
#include <DirectXMath.h>
#include "QAngle.h"
#include "Vector.h"

struct ViewMatrix
{
    float matrix[16];
};

class matrix3x4_t
{
public:
    matrix3x4_t() {}
    matrix3x4_t(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23)
    {
        m_flMatVal[0][0] = m00; m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
        m_flMatVal[1][0] = m10; m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
        m_flMatVal[2][0] = m20; m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
    }
    //-----------------------------------------------------------------------------
    // Creates a matrix where the X axis = forward
    // the Y axis = left, and the Z axis = up
    //-----------------------------------------------------------------------------
    void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin)
    {
        m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
        m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
        m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
    }

    //-----------------------------------------------------------------------------
    // Creates a matrix where the X axis = forward
    // the Y axis = left, and the Z axis = up
    //-----------------------------------------------------------------------------
    matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin)
    {
        Init(xAxis, yAxis, zAxis, vecOrigin);
    }

    inline void SetOrigin(Vector const& p)
    {
        m_flMatVal[0][3] = p.x;
        m_flMatVal[1][3] = p.y;
        m_flMatVal[2][3] = p.z;
    }

    template<typename T>
    T ToRadians(T degrees) {
        return (degrees * 3.14159265359 / 180.f);
    }

    void AngleMatrix(const QAngle& angles)
    {
        float sr, sp, sy, cr, cp, cy;
        DirectX::XMScalarSinCos(&sy, &cy, ToRadians(angles.yaw));
        DirectX::XMScalarSinCos(&sp, &cp, ToRadians(angles.pitch));
        DirectX::XMScalarSinCos(&sr, &cr, ToRadians(angles.roll));

        m_flMatVal[0][0] = cp * cy;
        m_flMatVal[1][0] = cp * sy;
        m_flMatVal[2][0] = -sp;

        float crcy = cr * cy;
        float crsy = cr * sy;
        float srcy = sr * cy;
        float srsy = sr * sy;
        m_flMatVal[0][1] = sp * srcy - crsy;
        m_flMatVal[1][1] = sp * srsy + crcy;
        m_flMatVal[2][1] = sr * cp;

        m_flMatVal[0][2] = (sp * crcy + srsy);
        m_flMatVal[1][2] = (sp * crsy - srcy);
        m_flMatVal[2][2] = cr * cp;

        m_flMatVal[0][3] = 0.0f;
        m_flMatVal[1][3] = 0.0f;
        m_flMatVal[2][3] = 0.0f;
    }

    void MatrixSetColumn(const Vector& in, int column)
    {
        m_flMatVal[0][column] = in.x;
        m_flMatVal[1][column] = in.y;
        m_flMatVal[2][column] = in.z;
    }

    void AngleMatrix(const QAngle& angles, const Vector& position)
    {
        AngleMatrix(angles);
        MatrixSetColumn(position, 3);
    }


    Vector GetXAxis()  const { return at(0); }
    Vector GetYAxis()  const { return at(1); }
    Vector GetZAxis()  const { return at(2); }
    Vector GetOrigin() const { return at(3); }

    Vector at(int i) const { return Vector{ m_flMatVal[0][i], m_flMatVal[1][i], m_flMatVal[2][i] }; }

    float* operator[](int i) { return m_flMatVal[i]; }
    const float* operator[](int i) const { return m_flMatVal[i]; }
    float* Base() { return &m_flMatVal[0][0]; }
    const float* Base() const { return &m_flMatVal[0][0]; }

    float m_flMatVal[3][4];
};

struct matrix3x3_t {
    float m_flMatVal[3][3];

    matrix3x3_t(float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22)
    {
        m_flMatVal[0][0] = m00; m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02;
        m_flMatVal[1][0] = m10; m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12;
        m_flMatVal[2][0] = m20; m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22;
    }

    Vector operator*(const Vector& vec) const {
        return Vector(
            m_flMatVal[0][0] * vec.x + m_flMatVal[0][1] * vec.y + m_flMatVal[0][2] * vec.z,
            m_flMatVal[1][0] * vec.x + m_flMatVal[1][1] * vec.y + m_flMatVal[1][2] * vec.z,
            m_flMatVal[2][0] * vec.x + m_flMatVal[2][1] * vec.y + m_flMatVal[2][2] * vec.z
        );
    }
};