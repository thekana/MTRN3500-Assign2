#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>	// for kbhit
#include "GPS.h"

using namespace System; // for console
using namespace System::Threading;

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject GPSObj(_TEXT("GPS"), sizeof(GPS_sm));
	//-----------Shared memory setup---------
	PMObj.SMAccess();
	GPSObj.SMAccess();
	PM* PMSMPtr = (PM*)PMObj.pData;
	GPS_sm* GPSPtr = (GPS_sm*)GPSObj.pData;
	//-----------GPS Object setup------------
	GPS^ myGPS = gcnew GPS("192.168.1.200", 24000);
	PMSMPtr->Shutdown.Flags.GPS = 0;
	int waitCount = 0;
	while (!PMSMPtr->Shutdown.Flags.GPS) {
		PMSMPtr->Heartbeats.Flags.GPS = 1;
		if (PMSMPtr->PMHeartbeats.Flags.GPS == 1) {
			PMSMPtr->PMHeartbeats.Flags.GPS = 0;
			waitCount = 0;
		}
		else {
			if (++waitCount > 50) {
				// we have waited too long
				PMSMPtr->Shutdown.Status = 0xFF;
			}
			Console::WriteLine("Waitcount: " + waitCount);
		}
		/*GPS routine CRC is handled in GPS::processGPSData()*/
		myGPS->processGPSData();
		GPSPtr->Easting = myGPS->getEasting();
		GPSPtr->Northing = myGPS->getNorthing();
		GPSPtr->Height = myGPS->getHeight();
		Console::Write("\t[{0,10:F3}, {1,10:F3}, {2,10:F3}]", GPSPtr->Northing, GPSPtr->Easting, GPSPtr->Height);
		Console::WriteLine("");
		if (_kbhit()) break;
		Thread::Sleep(20);
	}

	Console::WriteLine("GPS Process terminated");
	//Console::ReadKey();
	return 0;
}