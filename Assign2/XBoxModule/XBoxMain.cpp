#include <SMStructs.h>
#include <SMObject.h>
#include <conio.h>
#include "XBoxController.h"
#define MIN_JOY_STICK -32768.0
#define MAX_JOY_STICK 32767.0
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
		controller.SetDeadzone(10000);
		//--------------Scaling to -1 +1 for Speed and -40 +40 for Steering
		double speed = (controller.RightThumbLocation().GetY() - MIN_JOY_STICK) / (MAX_JOY_STICK - MIN_JOY_STICK) * (1 - (-1)) + (-1);
		double steering = (controller.LeftThumbLocation().GetX() - MIN_JOY_STICK) / (MAX_JOY_STICK - MIN_JOY_STICK) * (40 - (-40)) + (-40);
		XboxPtr->ControlSpeed = speed;
		XboxPtr->ControlSteering = steering;
		if (_kbhit()) break;
		Thread::Sleep(20);
	}
	Console::WriteLine("Xbox Process terminated");
	return 0;
}