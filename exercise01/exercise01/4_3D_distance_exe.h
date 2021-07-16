// calculate distance in 2D space exercise
#include<math.h>
class distance3D {
public:
	float computeDistance(float*, float*);
};

float distance3D::computeDistance(float* p0, float* p1) {
	float p[3] = { (p1[0] - p0[0]), (p1[1] - p0[1]), (p1[2] - p0[2]) };
	return sqrt(pow(p[0], 2) + pow(p[1], 2) + pow(p[2], 2));
}