#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>	// for kbhit

using namespace System; // for console
using namespace System::Threading;

int main() {
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	PM* PMSMPtr = nullptr;

	PMObj.SMAccess();
	if (PMObj.SMAccessError) {
		Console::WriteLine("Shared memory access failed");
		return -2;
	}

	PMSMPtr = (PM*)PMObj.pData;

	while (!PMSMPtr->Shutdown) {
		Console::WriteLine("{0,10:F3}",PMSMPtr->PMTimeStamp);
		PMSMPtr->PMTimeStamp = 0;
		if (_kbhit()) break;
		Thread::Sleep(20);
	}

	Console::WriteLine("GPS Process terminated");
	Console::ReadKey();
	return 0;
}