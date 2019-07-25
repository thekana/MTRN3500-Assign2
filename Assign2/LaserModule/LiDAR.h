#pragma once
#include <SMObject.h>
#include <SMStructs.h>
#using <System.dll>

using namespace System; // for console
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

ref class LiDAR
{
private:
	TcpClient^ Client;
	int portNumber;
	System::String^ IPAddress;
	double startAngle;
	double resolution;
	System::String^ message;
public:
	int NumRanges;
	array<double>^ Range;
	array<double>^ RangeX;
	array<double>^ RangeY;
	LiDAR(System::String^ ipaddress, int port);
	bool Connect();
	void GetXYRangeData();
	double getResolution();
	double getStartAngle();
	TcpClient^ getClient();
	void closeClient();
};