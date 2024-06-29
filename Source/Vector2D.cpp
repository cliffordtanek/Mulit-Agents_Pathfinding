#include "MathLib.h"

// ===================================== //
// Vector2D's constructor definition
// ===================================== //

// Constructors definitions
Vector2D::Vector2D() : x{ 0.f }, y{ 0.f } {}			         // default ctor

Vector2D::Vector2D(float _x, float _y) : x{ _x }, y{ _y } {}	// arguments ctor


// ===================================== //
// Vector2D's member functions definition
// ===================================== //

// Assignment operators
Vector2D &Vector2D::operator+= (Vector2D const &rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2D &Vector2D::operator-= (Vector2D const &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2D &Vector2D::operator*= (float rhs)
{
    x *= rhs;
    y *= rhs;
    return *this;
}

Vector2D &Vector2D::operator/= (float rhs)
{
    x /= rhs;
    y /= rhs;
    return *this;
}


// unary operator
Vector2D Vector2D::operator-() const
{
    Vector2D vector{ -x, -y };
    return vector;
}


Vector2D Vector2D::Normalize() const
{
    // if this is a zero vector
    if (Zero())
    {
        return vec2{ 0.f, 0.f };
    }


    float magnitude = std::sqrtf(x * x + y * y);

    Vec2 pResult{ x / magnitude, y / magnitude };

    return pResult;
}

Vector2D Vector2D::OutwardNormal() const
{
    return { y, -x };
}

Vector2D Vector2D::InwardNormal() const
{
    return { -y, x };
}

float Vector2D::Length() const
{
    return std::sqrtf(x * x + y * y);
}


float Vector2D::SquareLength() const
{
    return x * x + y * y;
}


float Vector2D::Distance(Vector2D const &rhs) const
{
    return std::sqrtf(std::powf(x - rhs.x, 2.f) + std::powf(y - rhs.y, 2.f));
}


float Vector2D::SquareDistance(Vector2D const &rhs) const
{
    return std::powf(x - rhs.x, 2.f) + std::powf(y - rhs.y, 2.f);
}


float Vector2D::Dot(Vector2D const &rhs) const
{
    return x * rhs.x + y * rhs.y;
}


float Vector2D::Cross(Vector2D const &rhs) const
{
    return x * rhs.y - y * rhs.x;
}


bool Vector2D::Zero() const
{
    // if this is a zero vector
    if ((x > -EPSILON && x < EPSILON) && (y > -EPSILON && y < EPSILON))
    {
        return true;
    }
    return false;
}


// ===================================== //
// Vector2D's binary operator definition
// ===================================== //


// binary operator
Vector2D operator+ (Vector2D const &lhs, Vector2D const &rhs)
{
    Vector2D vector{ lhs.x + rhs.x, lhs.y + rhs.y };
    return vector;
}


Vector2D operator- (Vector2D const &lhs, Vector2D const &rhs)
{
    Vector2D vector{ lhs.x - rhs.x, lhs.y - rhs.y };
    return vector;
}


Vector2D operator* (Vector2D const &lhs, float rhs)
{
    Vector2D vector{ lhs.x * rhs, lhs.y * rhs };
    return vector;
}


Vector2D operator* (float lhs, Vector2D const &rhs)
{
    Vector2D vector{ rhs.x * lhs, rhs.y * lhs };
    return vector;
}


Vector2D operator/ (Vector2D const &lhs, float rhs)
{
    Vector2D vector{ lhs.x / rhs, lhs.y / rhs };
    return vector;
}

std::ostream &operator<<(std::ostream &os, Vector2D const &rhs)
{
    os << "{" << rhs.x << ", " << rhs.y << "}";
    return os;
}