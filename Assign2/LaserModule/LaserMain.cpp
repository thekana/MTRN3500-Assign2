#include "LiDAR.h"

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	PM* PMSMPtr = nullptr;
	int waitCount = 0;
	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed");
		return -2;
	}
	PMSMPtr = (PM*)PMObj.pData;
	PMSMPtr->Shutdown.Flags.Laser = 0;
	// LMS151 port number must be 2111
	int PortNumber = 23000;
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	System::String^ AskScan = gcnew System::String("sRN LMDscandata");
	System::String^ Authen = gcnew System::String("5176970\n");
	// String to store received data for display
	System::String^ ResponseData;
	/*For data reading*/
	double StartAngle;
	double Resolution;
	double NumRanges;
	array<double> ^Range;
	array<double> ^RangeX;
	array<double> ^RangeY;

	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 2048;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(Authen);
	array<System::String^>^ Fragments = nullptr; // to store splited arrays
	// Get the network streab object associated with clien so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();
	Stream->Write(SendData, 0, SendData->Length);	// authen
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	while (!PMSMPtr->Shutdown.Flags.Laser) {
		Thread::Sleep(20);
		PMSMPtr->Heartbeats.Flags.Laser = 1;
		if (PMSMPtr->PMHeartbeats.Flags.Laser == 1) {
			PMSMPtr->PMHeartbeats.Flags.Laser = 0;
			waitCount = 0;
			// Write command asking for data
			Stream->WriteByte(0x02);
			Stream->Write(SendData, 0, SendData->Length);
			Stream->WriteByte(0x03);
			// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
			System::Threading::Thread::Sleep(10);
			// Read the incoming data
			Stream->Read(ReadData, 0, ReadData->Length);
			// Convert incoming data from an array of unsigned char bytes to an ASCII string
			ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
			// Print the received string on the screen
			Fragments = ResponseData->Split(' ');
			StartAngle = System::Convert::ToInt32(Fragments[23], 16);
			Resolution = System::Convert::ToInt32(Fragments[24], 16) / 10000.0;
			NumRanges = System::Convert::ToInt32(Fragments[25], 16);
			Console::WriteLine("{0,10:F3} {1,10:F3} {2:D}", StartAngle, Resolution, NumRanges);
			Range = gcnew array<double>(NumRanges);
			RangeX = gcnew array<double>(NumRanges);
			RangeY = gcnew array<double>(NumRanges);

			for (int i = 0; i < NumRanges; i++) {
				Range[i] = System::Convert::ToInt32(Fragments[26 + i], 16);
				Console::Write("{0,10:F3}", Range[i]);
				RangeX[i] = Range[i] * Math::Sin(i*Resolution*Math::PI / 180.0);
				RangeY[i] = -Range[i] * Math::Cos(i*Resolution*Math::PI / 180.0);
			}
		}
		else {
			//if (++waitCount > 200) {
			//	// we have waited too long
			//	PMSMPtr->Shutdown.Status = 0xFF;
			//}
		}
		if (_kbhit()) break;
		//Console::WriteLine("Variable" + PMSMPtr->variable);
	}
	Stream->Close();
	Client->Close();
	Console::WriteLine("Laser Process terminated");
	Console::ReadKey();
	return 0;
}