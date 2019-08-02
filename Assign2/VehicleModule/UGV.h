#pragma once
#using <System.dll>
#include <conio.h>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;


ref class UGV
{
private:
	array<unsigned char>^ SendData;
	TcpClient^ Client;
	NetworkStream^ Stream;
	System::String^ IPAddress;
	int portNumber;

public:
	~UGV();
	UGV(System::String^ ipaddress, int portNumber);
	void setControl(double Steering, double Speed, unsigned int flag);
};