#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>	// for kbhit

using namespace System; // for console
using namespace System::Threading;

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	PM* PMSMPtr = nullptr;

	PMObj.SMCreate();
	if (PMObj.SMCreateError) {
		Console::WriteLine("Shared memory creation failed");
		return -1;
	}

	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed");
		return -2;
	}

	PMSMPtr = (PM*)PMObj.pData;
	PMSMPtr->Shutdown.Flags.PM = 0;
	PMSMPtr->Heartbeats.Status = 0xFF;

	while (!PMSMPtr->Shutdown.Flags.PM) {
		Thread::Sleep(2000);
		// GPS must have set this to HIGH
		PMSMPtr->PMHeartbeats.Flags.GPS = 1;
		if (PMSMPtr->Heartbeats.Flags.GPS == 1) {
			Console::WriteLine("In PM Set GPS HIGH");
			PMSMPtr->Heartbeats.Flags.GPS = 0;
			PMSMPtr->PMHeartbeats.Flags.GPS = 1;
		}
		else {
			// if GPS is critical we shutdown all
			PMSMPtr->Shutdown.Status = 0xFF;
		}
		if (_kbhit()) break;

	}
	Console::WriteLine("Process manager terminated");
	Console::ReadKey();
	return 0;
}