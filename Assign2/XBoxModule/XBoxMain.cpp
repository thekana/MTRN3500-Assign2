#include <SMStructs.h>
#include <SMObject.h>
#include <conio.h>
#include "XBoxController.h"

using namespace System; // for console
using namespace System::Threading;

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject XboxObj(_TEXT("XBOX"), sizeof(Remote));
	//-----------Shared memory setup---------
	PMObj.SMAccess();
	XboxObj.SMAccess();
	PM* PMSMPtr = (PM*)PMObj.pData;
	Remote* XboxPtr = (Remote*)XboxObj.pData;
	//----------Controller setup-------------
	XInputWrapper xinput;
	GamePad::XBoxController controller(&xinput, 0);
	PMSMPtr->Shutdown.Flags.Xbox = 0;
	int waitCount = 0;
	while (!PMSMPtr->Shutdown.Flags.Xbox) {
		PMSMPtr->Heartbeats.Flags.Xbox = 1;
		if (PMSMPtr->PMHeartbeats.Flags.Xbox == 1) {
			PMSMPtr->PMHeartbeats.Flags.Xbox = 0;
			waitCount = 0;
		}
		else {
			if (++waitCount > 50) {
				// we have waited too long
				PMSMPtr->Shutdown.Status = 0xFF;
			}
			Console::WriteLine("Waitcount: " + waitCount);
		}
		XboxPtr->ControlSpeed = controller.RightTriggerLocation();
		XboxPtr->ControlSteering = controller.LeftTriggerLocation();
		if (_kbhit()) break;
		Thread::Sleep(20);
	}
	Console::WriteLine("Xbox Process terminated");
	return 0;
}