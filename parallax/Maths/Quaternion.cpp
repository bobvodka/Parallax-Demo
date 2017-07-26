#include "Quaternion.hpp"
#include <math.h>

// Public part:

Quaternion Quaternion::matrixToQuaternion(const float *matrix)
{
  Quaternion result(matrix);
  return result;
}

Quaternion::Quaternion(float ax, float ay, float az, float degree)
{
  float halfTheta = M_PI * degree / 360.0f;
  float sinHalfTheta = sin(halfTheta);
  w = cos(halfTheta);
  x = ax * sinHalfTheta;
  y = ay * sinHalfTheta;
  z = az * sinHalfTheta;
}

Quaternion::Quaternion(Quaternion const &q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

Quaternion::Quaternion(const float *matrix)
{
  float trace = matrix[0] + matrix[5] + matrix[10] + matrix[15];
  float scale;
  if(trace > M_EPSILON)
    {
      scale = 0.5f / sqrt(trace);
      w = 0.25f / scale;
      x = (matrix[6] - matrix[9]) * scale;
      y = (matrix[8] - matrix[2]) * scale;
      z = (matrix[1] - matrix[4]) * scale;
    }
  else if((matrix[0] > matrix[5]) && (matrix[0] > matrix[10]))
    { 
      scale = sqrt(matrix[15] + matrix[0] - matrix[5] - matrix[10]) * 2.0f;
      w = (matrix[9] - matrix[6]) / scale;
      x = 0.25f * scale;
      y = (matrix[4] + matrix[1]) / scale;
      z = (matrix[8] + matrix[2]) / scale;
      
    }
  else if (matrix[5] > matrix[10])
    {
      scale = sqrt(matrix[15] + matrix[5] - matrix[0] - matrix[10]) * 2.0f;
      w = (matrix[8] - matrix[2]) / scale;
      x = (matrix[4] + matrix[1]) / scale;;
      y = 0.25f * scale;
      z = (matrix[9] + matrix[6]) / scale;
    }
  else
    { 
      scale = sqrt(matrix[15] + matrix[10] - matrix[0] - matrix[5]) * 2.0f;
      w = (matrix[4] - matrix[1]) / scale;
      x = (matrix[8] + matrix[2]) / scale;
      y = (matrix[9] + matrix[6]) / scale;
      z = 0.25f * scale;
    }
}

void Quaternion::getAxis(float &ax, float &ay, float &az)
{
  float scale = sqrt(x*x + y*y + z*z);
  ax = x / scale;
  ay = y / scale;
  az = z / scale;
}

void Quaternion::normalize()
{
  float magn = sqrt(w*w + x*x + y*y + z*z);
  w /= magn;
  x /= magn;
  y /= magn;
  z /= magn;
}

void Quaternion::invert()
{
  float magn = w*w + x*x + y*y + z*z;
  if(magn > 0.0f)
    {
      magn = 1.0f / magn;
      w *= magn;
      x = -x * magn;
      y = -y * magn;
      z = -z * magn;
    }
}

Quaternion Quaternion::slerp(const Quaternion &q, float t) const
{
  float dot = w*q.w + x*q.x + y*q.y + z*q.z;
  int factor = 1;
  Quaternion result;
  if(dot < 0.0f)
    factor = -1;
    
  result.w = (1.0f-t) * w + t * q.w * factor;
  result.x = (1.0f-t) * x + t * q.x * factor;
  result.y = (1.0f-t) * y + t * q.y * factor;
  result.z = (1.0f-t) * z + t * q.z * factor;
  result.normalize();
  return result;
}

matrix44 Quaternion::getMatrix()
{
  

  vector4 col1(	1.0f - 2.0f*y*y - 2.0f*z*z,		// 0
				2.0f*x*y + 2.0f*w*z,			// 1
				2.0f*x*z - 2.0f*w*y,			// 2
				0.0f							// 3
				);
  vector4 col2(	2.0f*x*y - 2.0f*w*z,			// 4
				1.0f - 2.0f*x*x - 2.0f*z*z,		// 5
				2.0f*y*z + 2.0f*w*x,			// 6
				0.0f							// 7
				);
  vector4 col3(	2.0f*x*z + 2.0f*w*y,			// 8
				2.0f*y*z - 2.0f*w*x,			// 9
				1.0f - 2.0f*x*x - 2.0f*y*y,		// 10
				0.0f							// 11
				);
  vector4 col4(0.0f, 0.0f, 0.0f, 1.0f);			// 12, 13, 14, 15
  
  return matrix44(col1, col2, col3, col4);
}

Quaternion Quaternion::operator*(const Quaternion &rhs)
{
  Quaternion result;
  result.w = w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z;
  result.x = w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y;
  result.y = w*rhs.y + y*rhs.w + z*rhs.x - x*rhs.z;
  result.z = w*rhs.z + z*rhs.w + x*rhs.y - y*rhs.x;

  return result;
}

Quaternion Quaternion::Conjugate () const
{
	Quaternion q;
	q.w = w;
	q.x = -x;
	q.y = -y;
	q.z = -z;
	return q;
}

vector3 Quaternion::Rotate(const vector3 &v) const
{
	Quaternion vec;
	vec.w = 0.0f;
	vec.x = v.x;
	vec.y = v.y;
	vec.z = v.z;
	Quaternion q = *this;
	Quaternion qinv = q.Conjugate();

	Quaternion vec2 = q*vec*qinv;
	return vector3 (vec2.x, vec2.y, vec2.z);
}

void Quaternion::ComputeR()
{
	float t = 1.0f-(x*x)-(y*y)-(z*z); 
	w = (t < 0.0f) ? 0.0f : -(float)sqrt(t);
}

// Private part:

const float Quaternion::M_EPSILON = 0.0000001f;

// Support functions
Quaternion RotationArc(vector3 v0,vector3 v1)
{
	Quaternion q;
	// v0.normalize(); 
	// v1.normalize();  // If vector is already unit length then why do it again?
	vector3 c = CrossProduct(v0,v1);
	float   d = DotProduct(v0,v1);
	float   s = (float)sqrt((1+d)*2);
	q.x = c.x / s;
	q.y = c.y / s;
	q.z = c.z / s;
	q.w = s /2.0f;
	return q;
}