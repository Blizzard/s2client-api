#include "sc2api/sc2_common.h"

#include <random>
#include <thread>

// Avoiding use of "thread_local" as that isn't supported in older versions of Xcode.
#if defined(__clang__) || defined(__GNUC__)
#define TLS_OBJECT  __thread
#else
#define TLS_OBJECT  __declspec(thread)
#endif

namespace sc2 {

Point3D& Point3D::operator+=(const Point3D& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Point3D& Point3D::operator-=(const Point3D& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Point3D& Point3D::operator*=(float rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

Point3D& Point3D::operator/=(float rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

bool Point3D::operator==(const Point3D& rhs) {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Point3D::operator!=(const Point3D& rhs) {
    return !(*this == rhs);
}

Point3D operator+(const Point3D& lhs, const Point3D& rhs) {
    return Point3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Point3D operator-(const Point3D& lhs, const Point3D& rhs) {
    return Point3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Point3D operator*(const Point3D& lhs, float rhs) {
    return Point3D(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

Point3D operator*(float lhs, const Point3D& rhs) {
    return rhs * lhs;
}

Point3D operator/(const Point3D& lhs, float rhs) {
    return Point3D(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

Point3D operator/(float lhs, const Point3D& rhs) {
    return rhs / lhs;
}

Point2D& Point2D::operator+=(const Point2D& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Point2D& Point2D::operator-=(const Point2D& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Point2D& Point2D::operator*=(float rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
}

Point2D& Point2D::operator/=(float rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
}

bool Point2D::operator==(const Point2D& rhs) {
    return x == rhs.x && y == rhs.y;
}

bool Point2D::operator!=(const Point2D& rhs) {
    return !(*this == rhs);
}

Point2D operator+(const Point2D& lhs, const Point2D& rhs) {
    return Point2D(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point2D operator-(const Point2D& lhs, const Point2D& rhs) {
    return Point2D(lhs.x - rhs.x, lhs.y - rhs.y);
}

Point2D operator*(const Point2D& lhs, float rhs) {
    return Point2D(lhs.x * rhs, lhs.y * rhs);
}

Point2D operator*(float lhs, const Point2D& rhs) {
    return rhs * lhs;
}

Point2D operator/(const Point2D& lhs, float rhs) {
    return Point2D(lhs.x / rhs, lhs.y / rhs);
}

Point2D operator/(float lhs, const Point2D& rhs) {
    return rhs / lhs;
}

bool Point2DI::operator==(const Point2DI& rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Point2DI::operator!=(const Point2DI& rhs) const {
    return !(*this == rhs);
}

struct RandomGenerator {
    RandomGenerator() : rd(), mt(rd()) {}
    std::random_device rd;
    std::mt19937 mt;
};

static std::mt19937& GetGenerator () {
    static TLS_OBJECT RandomGenerator* generator;
    if (!generator)
        generator = new RandomGenerator();
    return generator->mt;
}
    
float GetRandomScalar() {
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(GetGenerator());
}

float GetRandomFraction() {
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(GetGenerator());
}

int GetRandomInteger(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(GetGenerator());
}

float Distance2D(const Point2D& a, const Point2D& b) {
    Point2D diff = a - b;
    return std::sqrt(Dot2D(diff, diff));
}

float DistanceSquared2D(const Point2D& a, const Point2D& b) {
    Point2D diff = a - b;
    return Dot2D(diff, diff);
}

void Normalize2D(Point2D& a) {
    a /= std::sqrt(Dot2D(a, a));
}

float Dot2D(const Point2D& a, const Point2D& b) {
    return a.x * b.x + a.y * b.y;
}

float Distance3D(const Point3D& a, const Point3D& b) {
    Point3D diff = a - b;
    return std::sqrt(Dot3D(diff, diff));
}

float DistanceSquared3D(const Point3D& a, const Point3D& b) {
    Point3D diff = a - b;
    return Dot3D(diff, diff);
}

void Normalize3D(Point3D& a) {
    a /= std::sqrt(Dot3D(a, a));
}

float Dot3D(const Point3D& a, const Point3D& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

}
