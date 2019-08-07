#include "UGV.h"
#include <math.h>

UGV::~UGV()
{
	Stream->Close();
	Client->Close();
}

/*Create Client and authenticate with zID*/
UGV::UGV(System::String^ ipaddress, int portNumber)
{
	this->IPAddress = ipaddress;
	this->portNumber = portNumber;
	this->Client = gcnew TcpClient(this->IPAddress, this->portNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	SendData = gcnew array<unsigned char>(50);
	Stream = Client->GetStream();
	//-----Authentication------
	System::String^ Authen = gcnew System::String("5176970\n");
	SendData = System::Text::Encoding::ASCII->GetBytes(Authen);
	Stream->Write(SendData, 0, SendData->Length);
	SendData = gcnew array<unsigned char>(50); // reset buffer
}

void UGV::setControl(double Steering, double Speed, unsigned int flag)
{
	String^ SendControl = gcnew String("# " + Steering.ToString("f2") + " " 
		+ Speed.ToString("f2") + " " + flag + " #");
	Console::WriteLine(SendControl);
	SendData = System::Text::Encoding::ASCII->GetBytes(SendControl);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	System::Threading::Thread::Sleep(100);
}
