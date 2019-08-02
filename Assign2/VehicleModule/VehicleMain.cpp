#include <SMStructs.h>
#include <SMObject.h>
#include <conio.h>
#include "UGV.h"
using namespace System; // for console
using namespace System::Threading;

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject UGVObj(_TEXT("UGV"), sizeof(Vehicle_1));
	SMObject XboxObj(_TEXT("XBOX"), sizeof(Remote));
	//-----------Shared memory setup---------
	PMObj.SMAccess();
	UGVObj.SMAccess();
	XboxObj.SMAccess();
	PM* PMSMPtr = (PM*)PMObj.pData;
	Vehicle_1* UGVPtr = (Vehicle_1*)UGVObj.pData;
	Remote* XboxPtr = (Remote*)XboxObj.pData;
	//----------Setup Client-------------
	PMSMPtr->Shutdown.Flags.Vehicle = 0;
	int waitCount = 0;
	UGV^ vehicle = gcnew UGV("192.168.1.200", 25000);
	unsigned int flag = 0;
	//------------Main loop--------------
	while (!PMSMPtr->Shutdown.Flags.Vehicle) {
		PMSMPtr->Heartbeats.Flags.Vehicle = 1;
		if (PMSMPtr->PMHeartbeats.Flags.Vehicle == 1) {
			PMSMPtr->PMHeartbeats.Flags.Vehicle = 0;
			waitCount = 0;
		}
		else {
			if (++waitCount > 50) {
				// we have waited too long
				PMSMPtr->Shutdown.Status = 0xFF;
			}
			Console::WriteLine("Waitcount: " + waitCount);
		}
		vehicle->setControl(XboxPtr->ControlSteering, XboxPtr->ControlSpeed, flag);
		flag = !flag;
		if (_kbhit()) break;
		Thread::Sleep(20);
	}
	Console::WriteLine("Vehicle Process terminated");
	return 0;
}