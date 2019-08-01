#ifndef __MYVEHICLE_HPP__
#define __MYVEHICLE_HPP__


#include "Vehicle.hpp"

class MyVehicle : public Vehicle
{
private:
	int *numData;
	double *xData;
	double *yData;
	void drawLaser();
public:
	MyVehicle(int *numpoints, double xData[], double yData[]);
	void setLaserData(int *numpoints, double xData[], double yData[]);
	virtual void draw();
};

#endif