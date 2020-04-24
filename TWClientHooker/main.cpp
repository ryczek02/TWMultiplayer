#include <Windows.h>
#include <iostream>

DWORD pid;

int main() {
	HWND hWnd = FindWindowA(0, ("Island"));
	GetWindowThreadProcessId(hWnd, &pid);
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hWnd != NULL)
	{
		std::cout << "===========================================\n";
		std::cout << "Tajemnicza Wyspa.exe [PROCESS ID = " << hWnd << "]\n";
		std::cout << "===========================================\n";
	}
	else {
		std::cout << "NIE MOZNA ZAINICJALOZIOWAC PROCESU Z GRA.";
		getchar();
		return 1;
	}
	return 0;
}