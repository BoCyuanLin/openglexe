// calculate distance in 2D space exercise
#include<math.h>
class distance2D{
	public:
		float computeDistance(float *, float *);
};

float distance2D::computeDistance(float *p0, float* p1) {
	float p[2] = {(p1[0] - p0[0]), (p1[1] - p0[1])};
	return sqrt(pow(p[0], 2)+ pow(p[1], 2));
}