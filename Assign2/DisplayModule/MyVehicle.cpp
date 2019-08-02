#include "MyVehicle.hpp"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <unistd.h>
#elif defined(WIN32)
#include <Windows.h>
#include <tchar.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#pragma warning(disable : 4996)
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <unistd.h>
#endif
using namespace System; // for console
using namespace System::Threading;


void drawUGV(double steerAngle)
{
	const float red = .8, green = .1, blue = .2;
	const float width = .4;
	const float wheel_height = .4;
	const float wheel_width = .1;
	const float vertices[10][3] = {
		// left-hand side
		{ .5,   wheel_height - 0.1,    width},
		{-.5,   wheel_height - 0.1,    width},
		{-.5,   wheel_height + 0.3,    width},
		{ .4,   wheel_height + 0.3,    width},
		{ .5,   wheel_height + 0.15,   width},
		// right-hand side
		{ .5,   wheel_height - 0.1,   -width},
		{-.5,   wheel_height - 0.1,   -width},
		{-.5,   wheel_height + 0.3,   -width},
		{ .4,   wheel_height + 0.3,   -width},
		{ .5,   wheel_height + 0.15,  -width}
	};

	// quad faces
	glBegin(GL_QUADS);
	// bottom		
	glColor3f(red*.3, green, blue);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[1]);

	// back
	glColor3f(red* .5, green, blue);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glVertex3fv(vertices[2]);

	// top
	glColor3f(red * .7, green, blue);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[7]);
	glVertex3fv(vertices[8]);
	glVertex3fv(vertices[3]);

	// angled 
	glColor3f(red * .6, green, blue);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[8]);
	glVertex3fv(vertices[9]);
	glVertex3fv(vertices[4]);

	// front
	glColor3f(red * .5, green, blue);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[9]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[0]);
	glEnd();

	// left
	glBegin(GL_POLYGON);
	glColor3f(red, green, blue);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[4]);
	glEnd();

	// right
	glBegin(GL_POLYGON);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glVertex3fv(vertices[8]);
	glVertex3fv(vertices[9]);
	glEnd();


	// draw wheels
	glPushMatrix();
	glColor3f(1, 1, 1);
	static GLUquadric *quad = gluNewQuadric();
	glTranslatef(0, wheel_height*.5, -wheel_width * .5);

	// front
	glPushMatrix();
	glTranslatef(vertices[0][0], 0, -(width + wheel_width));
	glPushMatrix();
	glRotatef(-steerAngle, 0, 1, 0);
	gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);
	glPopMatrix();

	glTranslatef(0, 0, (width + wheel_width) * 2);
	glPushMatrix();
	glRotatef(-steerAngle, 0, 1, 0);
	gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);
	glPopMatrix();
	glPopMatrix();

	// back
	glPushMatrix();
	glTranslatef(vertices[1][0], 0, -(width + wheel_width));
	gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);

	glTranslatef(0, 0, (width + wheel_width) * 2);
	gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);
	glPopMatrix();
	glPopMatrix();
}

void MyVehicle::drawLaser()
{
	glPushMatrix();
	// glTranslatef(x, y, z);
	// glRotatef(-rotation, 0, 0, 0);
	double X1 = 0, X2 = 0, Y1 = 0, Y2 = 0;
	glColor3f(0, 1.0, 0);
	glLineWidth(2);
	for (int i = 0; i < *numData; i++) {


		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(*(xData + i) / 100, 0, -*(yData + i) / 100);
		//glVertex3f(0, *(yData + i), *(xData + i));
		glEnd();
		//if (*(xData + i) != 0 || *(yData + i) != 0) {
		//	X1 = X2;
		//	Y1 = Y2;
		//	X2 = *(xData + i);
		//	Y2 = *(yData + i);
		//	//glVertex3f(1, 0, 0);
		//	glBegin(GL_LINES);
		//	glTranslatef(x, y, z);
		//	glVertex3f(X1 / 100, 0 , -Y1 / 100);
		//	glVertex3f(X2 / 100, 0 , -Y2 / 100);
		//	glEnd();
		//	//glVertex3f(Y2 / 100 + 1, 0, X2 / 100);
		//}
		// glEnd();
		//std::cout << "print laser_" << *numData << " X: " << *(xData + i) << " Y: " << *(yData + i) << std::endl;
	}
	//std::cout << "print laser2" << std::endl;
	//glEnd();
	glPopMatrix();
}

MyVehicle::MyVehicle(int * numpoints, double xData[], double yData[])
{
	this->numData = numpoints;
	this->xData = xData;
	this->yData = yData;
}

void MyVehicle::setLaserData(int * numpoints, double xData[], double yData[])
{
	this->numData = numpoints;
	this->xData = xData;
	this->yData = yData;
}

void MyVehicle::draw()
{
	glPushMatrix();
	positionInGL();

	drawUGV(steering);
	drawLaser();
	Thread::Sleep(50);
	glPopMatrix();
}