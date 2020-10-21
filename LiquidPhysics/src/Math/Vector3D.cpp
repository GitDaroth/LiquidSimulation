#include "Math/Vector3D.h"

namespace LiPhEn {
	Vector3D::Vector3D() : m_x(0), m_y(0), m_z(0) {}

	Vector3D::Vector3D(const float xyz) : m_x(xyz), m_y(xyz), m_z(xyz) {}

	Vector3D::Vector3D(const float x, const float y, const float z) : m_x(x), m_y(y), m_z(z) {}

	Vector3D::Vector3D(const Vector3D & v)
	{
		m_x = v.m_x;
		m_y = v.m_y;
		m_z = v.m_z;
	}

	float Vector3D::getX() const { 
		return m_x; 
	}
	float Vector3D::getY() const { 
		return m_y; 
	}
	float Vector3D::getZ() const { 
		return m_z; 
	}

	void Vector3D::setX(const float x1) { 
		m_x = x1; 
	}

	void Vector3D::setY(const float y1) { 
		m_y = y1; 
	}
	void Vector3D::setZ(const float z1) { 
		m_z = z1; 
	}

	void Vector3D::setCoordinates(const float x_, const float y_, const float z_)
	{
		m_x = x_;
		m_y = y_;
		m_z = z_;
	}

	bool Vector3D::operator==(const Vector3D& v)
	{
		return m_x == v.m_x && m_y == v.m_y && m_z == v.m_z;
	}

	bool Vector3D::operator!=(const Vector3D& v)
	{
		return !(*this == v);
	}

	float Vector3D::magnitude() const
	{
		return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
	}

	float LiPhEn::Vector3D::squareMagnitude() const
	{
		return m_x*m_x + m_y*m_y + m_z*m_z;
	}

	
	void Vector3D::normalize() {
		float mag = magnitude();
		if (mag > 0) {
			(*this) *= ((float)1 / mag);
		}
	}

	void Vector3D::invert() {
		m_x = -m_x;
		m_y = -m_y;
		m_z = -m_z;
	}
	void Vector3D::operator*=(const float value)
	{
		m_x *= value;
		m_y *= value;
		m_z *= value;
	}
	Vector3D Vector3D::operator*(const float value) const
	{
		return Vector3D(m_x*value, m_y*value, m_z*value);
	}
	void Vector3D::operator/=(const float value)
	{
		m_x /= value;
		m_y /= value;
		m_z /= value;
	}
	Vector3D Vector3D::operator/(const float value) const
	{
		return Vector3D(m_x/value, m_y/value, m_z/value);
	}
	void Vector3D::operator+=(const Vector3D& v)
	{
		m_x += v.m_x;
		m_y += v.m_y;
		m_z += v.m_z;
	}

	Vector3D Vector3D::operator+(const Vector3D& v) const
	{
		return Vector3D(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
	}

	void Vector3D::operator-=(const Vector3D& v)
	{
		m_x -= v.m_x;
		m_y -= v.m_y;
		m_z -= v.m_z;
	}

	Vector3D Vector3D::operator-(const Vector3D& v) const
	{
		return Vector3D(m_x - v.m_x, m_y - v.m_y, m_z - v.m_z);
	}
	void Vector3D::addScaledVector(const Vector3D& v, float scale)
	{
		m_x += v.m_x * scale;
		m_y += v.m_y * scale;
		m_z += v.m_z * scale;
	}

	float Vector3D::scalarProduct(const Vector3D& v)
	{
		return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
	}

	float Vector3D::operator*(const Vector3D& v) const
	{
		return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
	}

	Vector3D Vector3D::vectorProduct(const Vector3D& v)
	{
		return Vector3D(
			m_y * v.m_z - m_z * v.m_y,
			m_z * v.m_x - m_x * v.m_z,
			m_x * v.m_y - m_y * v.m_x);
	}

	void Vector3D::operator%=(const Vector3D& v)
	{
		*this = vectorProduct(v);
	}

	Vector3D Vector3D::operator%(const Vector3D& v)
	{
		return Vector3D(
			m_y * v.m_z - m_z * v.m_y,
			m_z * v.m_x - m_x * v.m_z,
			m_x * v.m_y - m_y * v.m_x);
	}

	void Vector3D::clear()
	{
		m_x = m_y = m_z = 0;
	}

	Vector3D Vector3D::abs()
	{
		return Vector3D(fabs(m_x), fabs(m_y), fabs(m_z));
	}

	Vector3D Vector3D::addScalar(float scalar)
	{
		return Vector3D(m_x + scalar, m_y + scalar, m_z + scalar);
	}
}
bool operator==(const LiPhEn::Vector3D &l, const LiPhEn::Vector3D &r) {
	return l.getX() == r.getX() && l.getY() == r.getY() && l.getZ() == r.getZ();
}


