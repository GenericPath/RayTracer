#include "Cylinder.h"

#include "utility.h"

Cylinder::Cylinder() : Object() {

}

Cylinder::Cylinder(const Cylinder& cylinder) : Object(cylinder) {

}

Cylinder::~Cylinder() {

}

Cylinder& Cylinder::operator=(const Cylinder& cylinder) {
	if (this != &cylinder) {
		Object::operator=(cylinder);
	}
	return *this;
}

std::vector<RayIntersection> Cylinder::intersect(const Ray& ray) const {

	std::vector<RayIntersection> result;

	Ray inverseRay = transform.applyInverse(ray);

	// Intersection is of the form ad^2 + bd + c, where 
	// ignoring z component as it is a circle that spans the z-axis.
	const Point& p = Point(inverseRay.point(0), inverseRay.point(1), 0);
	// d = distance along the ray
	const Direction& d = Direction(inverseRay.direction(0), inverseRay.direction(1), 0);

	double a = d.dot(d);
	double b = 2 * d.dot(p);
	double c = p.dot(p) - 1;

	RayIntersection hit;
	hit.material = material;

	double b2_4ac = b*b - 4*a*c;
	double t;

	// z components to find the ends
	double z0 = inverseRay.point(2);
	double dz = inverseRay.direction(2);

	// Solving the quadratic formula
	double t0 = (-b + sqrt(b2_4ac))/(2*a);
	double t1 = (-b - sqrt(b2_4ac))/(2*a);

	// saves having to check both for < -1 and > 1
	if(t0 > t1){
		std::swap(t0,t1);
	}

	// Calculate intersection point for z axis
	// with each solution to quadratic formula.
	double z0t0 = z0 + t0 * dz;
	double z0t1 = z0 + t1 * dz;

	if (z0t0 < -1 || z0t0 > 1) { // End circles/caps
		t = t0 + (t1 - t0) * (z0t0+1)/(z0t0-z0t1);
		if (t > 0) {
			hit.point = inverseRay.point + t * inverseRay.direction;
			double x = hit.point(0);
			double y = hit.point(1);
			//Check to see if the hit point's radius is in the range [-1,1]
			// (so within a unit circle)
			if (x*x + y*y <= 1){
				hit.material = material;
				hit.point = transform.apply(Point(hit.point));
				if(z0t0 < -1) hit.normal = transform.apply(Normal(0,0,1)); //top end circle normal
				else hit.normal = transform.apply(Normal(0,0,-1)); //bottom end circle normal
				if (hit.normal.dot(ray.direction) > 0){
					hit.normal = -hit.normal;
				}
				hit.distance = (hit.point - ray.point).norm();
				result.push_back(hit);
			}
		}
	} else if (z0t0 >= -1 && z0t0 <= 1) { // Curved surface
		switch (sign(b2_4ac)) {
		case -1:
			// No intersections
			break;
		case 0:
			// One intersection
			t = -b/(2*a);
			if (t > 0) {
				// Intersection is in front of the ray's start point
				hit.point = transform.apply(Point(p + t*d));
				hit.normal = transform.apply(Normal(p + t*d));
				if (hit.normal.dot(ray.direction) > 0) {
					hit.normal = -hit.normal;
				}
				hit.distance = (hit.point - ray.point).norm();
				result.push_back(hit);
			}
			break;
		case 1:
			// Two intersections
			t = (-b + sqrt(b*b - 4*a*c))/(2*a);
			if (t > 0) {
				// Intersection is in front of the ray's start point
				hit.point = transform.apply(Point(p + t * d));
				hit.normal = transform.apply(Normal(p + t*d));
				if (hit.normal.dot(ray.direction) > 0) {
					hit.normal = -hit.normal;
				}
				hit.distance = (hit.point - ray.point).norm();
				result.push_back(hit);
			}

			t = (-b - sqrt(b*b - 4*a*c))/(2*a);
			if (t > 0) {
				// Intersection is in front of the ray's start point
				hit.point = transform.apply(Point(p + t*d));
				hit.normal = transform.apply(Normal(p + t*d));
				if (hit.normal.dot(ray.direction) > 0) {
					hit.normal = -hit.normal;
				}
				hit.distance = (hit.point - ray.point).norm();
				result.push_back(hit);
			}
			break;
		default:
			// Shouldn't be possible, but just in case
			std::cerr << "Something's wrong - sign(x) should be -1, +1 or 0" << std::endl;
			exit(-1);
			break;
		}	

	}

	return result;
}
