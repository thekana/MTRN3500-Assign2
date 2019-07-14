#pragma once

struct Remote
{
	double SetSpeed;
	double SetSteering;
};

struct GPS
{
	double Easting;
	double Northing;
	double Height;
};

struct PM
{
	double PMTimeStamp;
	double LasterTimeStamp;
	double GPSTimeStamp;
	double VehicleTimeStamp;
	double RemoteTimeStamp;
	bool Shutdown;
};

struct Vehicle
{
	double ActualSpeed;
	double ActualSteering;
};

struct Laser
{
	double XRange[600];
	double YRange[600];
};