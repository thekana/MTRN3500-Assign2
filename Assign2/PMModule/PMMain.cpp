#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>	// for kbhit
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

using namespace System; // for console
using namespace System::Threading;
#define NUM_PROCESS 5
#define waitTime 10
TCHAR* Units[10] = //
{
	TEXT("GPSModule.exe"),
	TEXT("LaserModule.exe"),
	TEXT("VehicleModule.exe"),
	TEXT("XBoxModule.exe"),
	TEXT("DisplayModule.exe"),
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

int startProcess(int i) {
	if (!IsProcessRunning(Units[i]))
	{
		ZeroMemory(&s[i], sizeof(s[i]));
		s[i].cb = sizeof(s[i]);
		ZeroMemory(&p[i], sizeof(p[i]));
		// Start the child processes
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
	Sleep(200);
}

struct waitCount {
	unsigned int GPS = 0;
	unsigned int XBOX = 0;
	unsigned int LASER = 0;
	unsigned int MOTOR = 0;
}count;

void shutdownRoutine() {
	/*Scuffed shutdown routine*/
	int retval;
	retval = ::_tsystem(_T("taskkill /F /T /IM GPSModule.exe"));
	retval = ::_tsystem(_T("taskkill /F /T /IM LaserModule.exe"));
	retval = ::_tsystem(_T("taskkill /F /T /IM VehicleModule.exe"));
	retval = ::_tsystem(_T("taskkill /F /T /IM XBoxModule.exe"));
	retval = ::_tsystem(_T("taskkill /F /T /IM DisplayModule.exe"));
}

int main() {
	//------------Shared Memory Objects---------------
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	SMObject LaserObj(_TEXT("Laser"), sizeof(Laser));
	SMObject GPSObj(_TEXT("GPS"), sizeof(GPS_sm));
	SMObject XboxObj(_TEXT("XBOX"), sizeof(Remote));
	SMObject UGVObj(_TEXT("UGV"), sizeof(Vehicle_1));
	//------------SM Objects Create/Access-------------
	PMObj.SMCreate();
	PMObj.SMAccess();
	LaserObj.SMCreate();
	LaserObj.SMAccess();
	GPSObj.SMCreate();
	GPSObj.SMAccess();
	XboxObj.SMCreate();
	XboxObj.SMAccess();
	UGVObj.SMCreate();
	UGVObj.SMAccess();
	//------------Pointer things----------------
	PM* PMSMPtr = nullptr;
	Laser* laserPtr = nullptr;
	PMSMPtr = (PM*)PMObj.pData;
	laserPtr = (Laser*)LaserObj.pData;
	GPS_sm* GPSPtr = (GPS_sm*)GPSObj.pData;
	Remote* XboxPtr = (Remote*)XboxObj.pData;
	Vehicle_1* UGVPtr = (Vehicle_1*)UGVObj.pData;
	//-----------------------------------------
	// Initialize status
	PMSMPtr->Shutdown.Status = 0x00;
	PMSMPtr->Heartbeats.Status = 0x00;
	// Starting the processes
	for (int i = 0; i < NUM_PROCESS; i++)
	{
		if (startProcess(i) == -1) {
			return -1;
		}
	}
	while (!PMSMPtr->Shutdown.Flags.PM) {
		Sleep(200);
		/*Set PM's as alive*/
		PMSMPtr->PMHeartbeats.Status = 0xFF;
		//------Critical processes----------
		if (PMSMPtr->Heartbeats.Flags.Laser == 1) {
			PMSMPtr->Heartbeats.Flags.Laser = 0;
			count.LASER = 0;
		}
		else {
			count.LASER++;
		}
		if (PMSMPtr->Heartbeats.Flags.Xbox == 1) {
			PMSMPtr->Heartbeats.Flags.Xbox = 0;
			count.XBOX = 0;
		}
		else {
			count.XBOX++;
		}
		if (PMSMPtr->Heartbeats.Flags.Vehicle == 1) {
			PMSMPtr->Heartbeats.Flags.Vehicle = 0;
			count.MOTOR = 0;
		}
		else {
			count.MOTOR++;
		}
		//-----Non critical process---------
		if (PMSMPtr->Heartbeats.Flags.GPS == 1) {
			PMSMPtr->Heartbeats.Flags.GPS = 0;
			count.GPS = 0;
		}
		else {
			count.GPS++;
			startProcess(0);
		}

		//---------Printing heartbeats--------------
		Thread::Sleep(10);
		Console::WriteLine("Laser Heartbeat " + PMSMPtr->Heartbeats.Flags.Laser);
		Console::WriteLine("GPS Heartbeat " + PMSMPtr->Heartbeats.Flags.GPS);
		Console::WriteLine("Xbox Heartbeat " + PMSMPtr->Heartbeats.Flags.Xbox);
		Console::WriteLine("Vehicle Heartbeat " + PMSMPtr->Heartbeats.Flags.Vehicle);
		//--------Restart--------
		if (count.GPS > waitTime) {
			int retval = ::_tsystem(_T("taskkill /F /T /IM GPSModule.exe"));
			count.GPS = 0;
		}
		//--------Shutdown all routine--------
		//Press A to shutdown
		if (_kbhit() || XboxPtr->performShutdown || count.XBOX > waitTime || count.LASER > waitTime || count.MOTOR > waitTime) {
			PMSMPtr->Shutdown.Status = 0xFF;
			shutdownRoutine();
		}

	}
	Console::WriteLine("Process manager terminated");
	Console::ReadKey();
	return 0;
}