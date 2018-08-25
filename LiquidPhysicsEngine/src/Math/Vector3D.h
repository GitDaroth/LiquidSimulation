#pragma once

namespace LiPhEn {
	class Vector3D {
	public:
		Vector3D();
		Vector3D(const float xyz);
		Vector3D(const float x, const float y, const float z);
		Vector3D(const Vector3D &v);

		bool operator==(const Vector3D &v); 
		bool operator!=(const Vector3D &v); 

		float getX() const;
		float getY() const;
		float getZ() const;

		void setX(const float x1);
		void setY(const float y1);
		void setZ(const float z1);
		void setCoordinates(const float x, const float y, const float z);
		
		float magnitude() const;
		float squareMagnitude() const;

		void normalize();

		void invert();

		void operator*=(const float value);
		Vector3D operator*(const float value) const;

		void operator/=(const float value);
		Vector3D operator/(const float value) const;

		void operator+=(const Vector3D& v);
		Vector3D operator+(const Vector3D& v) const;
		
		void operator-=(const Vector3D& v);
		Vector3D operator-(const Vector3D& v) const;

		Vector3D abs();

		Vector3D addScalar(float scalar);
		void addScaledVector(const Vector3D& v, float scale);

		float scalarProduct(const Vector3D &v);
		float operator*(const Vector3D &v) const;

		Vector3D vectorProduct(const Vector3D &v);
		void operator%=(const Vector3D &v);
		Vector3D operator%(const Vector3D &v);

		void clear();

	protected:
		float m_x;
		float m_y;
		float m_z;
	};
}
bool operator==(const LiPhEn::Vector3D &l, const LiPhEn::Vector3D &r);

