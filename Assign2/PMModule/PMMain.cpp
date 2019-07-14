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
	PMSMPtr->Shutdown = false;
	while (1) {
		PMSMPtr->PMTimeStamp = 500.00;
		if (_kbhit()) break;
		Thread::Sleep(1000);
	}
	PMSMPtr->Shutdown = true;
	Console::WriteLine("Process manager terminated");
	Console::ReadKey();
	return 0;
}