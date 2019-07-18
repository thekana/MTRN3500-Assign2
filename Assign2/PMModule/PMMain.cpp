#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>	// for kbhit
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

using namespace System; // for console
using namespace System::Threading;
TCHAR* Units[10] = //
{
	TEXT("LaserModule.exe"),
	TEXT("GPS.exe"),
	TEXT("XBox.exe"),
	TEXT("VehicleControl.exe"),
	TEXT("OpenGL.exe"),

};
// Module execution based variable declarations
STARTUPINFO s[10];
PROCESS_INFORMATION p[10];

//Is process running function
bool IsProcessRunning(const char *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

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
	// Starting the processes
	for (int i = 0; i < 1; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes.

			if (!CreateProcess(NULL,   // No module name (use command line)
				Units[i],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&s[i],            // Pointer to STARTUPINFO structure
				&p[i])           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
				return -1;
			}
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(1000);
	}
	while (!PMSMPtr->Shutdown.Flags.PM) {
		//Thread::Sleep(20);
		//PMSMPtr->variable = 1;
		PMSMPtr->PMHeartbeats.Flags.Laser = 1;
		if (PMSMPtr->Heartbeats.Flags.Laser == 1) {
			PMSMPtr->Heartbeats.Flags.Laser = 0;
			PMSMPtr->PMHeartbeats.Flags.Laser = 1;
		}
		//PMSMPtr->PMHeartbeats.Flags.GPS = 1;
		//if (PMSMPtr->Heartbeats.Flags.GPS == 1) {
		//	PMSMPtr->Heartbeats.Flags.GPS = 0;
		//	PMSMPtr->PMHeartbeats.Flags.GPS = 1;
		//}
		//else {
		//	// if GPS is critical we shutdown all
		//	PMSMPtr->Shutdown.Status = 0xFF;
		//}
		Thread::Sleep(20);
		Console::WriteLine("Laser Heartbeat " + PMSMPtr->Heartbeats.Flags.Laser);
		if (_kbhit()) break;

	}
	Console::WriteLine("Process manager terminated");
	Console::ReadKey();
	return 0;
}