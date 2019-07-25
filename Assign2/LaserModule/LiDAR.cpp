#include "LiDAR.h"

LiDAR::LiDAR(System::String ^ ipaddress, int port)
{
	this->IPAddress = ipaddress;
	this->portNumber = port;
	this->Client = gcnew TcpClient(IPAddress, portNumber);
	this->Client->NoDelay = true;
	this->Client->ReceiveTimeout = 500;//ms
	this->Client->SendTimeout = 500;//ms
	this->Client->ReceiveBufferSize = 2048;
	this->Client->SendBufferSize = 1024;
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

TcpClient ^ LiDAR::getClient()
{
	return this->Client;
}

void LiDAR::closeClient()
{
	this->Client->Close();
}
