#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>	// for kbhit

using namespace System; // for console
using namespace System::Threading;

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

	while (!PMSMPtr->Shutdown.Flags.Laser) {
		Thread::Sleep(20);
		PMSMPtr->Heartbeats.Flags.Laser = 1;
		if (PMSMPtr->PMHeartbeats.Flags.Laser == 1) {
			PMSMPtr->PMHeartbeats.Flags.Laser = 0;
			waitCount = 0;
		}
		else {
			//if (++waitCount > 20) {
			//	// we have waited too long
			//	PMSMPtr->Shutdown.Status = 0xFF;
			//}
		}
		if (_kbhit()) break;
		//Console::WriteLine("Variable" + PMSMPtr->variable);
	}

	Console::WriteLine("Laser Process terminated");
	Console::ReadKey();
	return 0;
}