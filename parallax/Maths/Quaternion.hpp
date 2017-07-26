#ifndef RESURRECTIONENGINE_QUATERNION
#define RESURRECTIONENGINE_QUATERNION
#define _USE_MATH_DEFINES
#include <math.h>
#include "mtxlib.h"

class Quaternion
{
public:
  float w, x, y, z;
  static Quaternion matrixToQuaternion(const float *matrix);
  
  Quaternion(float ax = 1.0, float ay = 0.0, float az = 0.0, float degree = 0.0);
  Quaternion(const float *matrix);
  Quaternion(Quaternion const &q);
  
  matrix44 getMatrix();
  void getAxis(float &ax, float &ay, float &az);
  void normalize();
  void invert();
  Quaternion slerp(const Quaternion &q, float t) const;
  vector3 Rotate(const vector3 &vec) const;
  Quaternion Conjugate () const;
  Quaternion operator*(const Quaternion &rhs);
  void ComputeR();
private:
  static const float M_EPSILON;
  
};

// Free support functions
Quaternion RotationArc(vector3 v0,vector3 v1);

#endif //QUATERNION
