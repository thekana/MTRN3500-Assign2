#include "LiDAR.h"

LiDAR::LiDAR(System::String ^ ipaddress, int port)
{
	this->IPAddress = ipaddress;
	this->portNumber = port;
	this->Client = gcnew TcpClient(IPAddress, portNumber);
}

bool LiDAR::Connect()
{
	return false;
}

void LiDAR::GetXYRangeData()
{
	throw gcnew System::NotImplementedException();
}

double LiDAR::getResolution()
{
	return 0.0;
}

double LiDAR::getStartAngle()
{
	return 0.0;
}
