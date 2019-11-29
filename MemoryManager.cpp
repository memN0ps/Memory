#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

#Author memN0ps

class MemoryManager {
private:
	HANDLE hProcess;

public:
	MemoryManager() {
		this->hProcess = NULL;
	}

	~MemoryManager() {
		CloseHandle(this->hProcess);
	}

	uintptr_t getProcessID(const char* targetProcess, uintptr_t desiredAccess) {

		//Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes. 
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		uintptr_t processID = NULL;
		//Check if snapshot created is valid
		if (hSnapShot == INVALID_HANDLE_VALUE) {
			std::cout << "Failed to take a snapshot" << std::endl;
			return false;
		}


		PROCESSENTRY32 pEntry;
		//The size of the structure, in bytes. Before calling the Process32First function, set this
		//member to sizeof(PROCESSENTRY32). If you do not initialize dwSize, Process32First fails.
		pEntry.dwSize = sizeof(PROCESSENTRY32);

		//Loop through the processes
		do {
			//Compare the targetProcess with the process in pEntry.szExeFile (current process)
			//if the name of the process we are at right now matches the target process then we found it
			if (!strcmp(pEntry.szExeFile, targetProcess)) {
				//Process Found
				std::cout << "Found Process " << pEntry.szExeFile << " with process ID " << pEntry.th32ProcessID << std::endl;
				
				//Open the process with desired access and the process ID of the target process
				this->hProcess = OpenProcess(desiredAccess, FALSE, pEntry.th32ProcessID);
				processID = pEntry.th32ProcessID;
				CloseHandle(hSnapShot);

				//Check if handle value valid
				if (this->hProcess == INVALID_HANDLE_VALUE) {
					std::cout << "Failed getting a handle to the process!" << std::endl;
					return false;
				}		
			}

			//Retrieves information about the first process encountered in a system snapshot.
			//Returns TRUE if the first entry of the process list has been copied to the buffer or FALSE otherwise.
		} while (Process32Next(hSnapShot, &pEntry));

		return processID;
	}

	uintptr_t getModuleID(uintptr_t processID, const char* targetModule) {
		//Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes. 
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
		
		//Check if snapshot created is valid
		if (hSnapShot == INVALID_HANDLE_VALUE) {
			std::cout << "Failed to take a snapshot" << std::endl;
			return false;
		}

		MODULEENTRY32 mEntry;
		//The size of the structure, in bytes. Before calling the Process32First function, set this
		//member to sizeof(MODULEENTRY32). If you do not initialize dwSize, Process32First fails.
		mEntry.dwSize = sizeof(MODULEENTRY32);

		//Loop through the modules
		do {
			//Compare the targetModule with the module in mEntry.szModule (current module)
			//if the name of the module we are at right now matches the target module then we found it
			if (!strcmp(mEntry.szModule, targetModule)) {
				//Module Found
				std::cout << "Found Module " << mEntry.szModule << " with module ID " << mEntry.th32ModuleID << std::endl;
				
				CloseHandle(hSnapShot);
				return (uintptr_t)mEntry.hModule;
			}

			//The size of the structure, in bytes. Before calling the Module32First function, set this member to sizeof(MODULEENTRY32). If you do not 
			//initialize dwSize, Module32First fails.
		} while (Module32Next(hSnapShot, &mEntry));

		return false;
	}

	//Read Process Memory Template
	template <class dataType>
	dataType readMemory(uintptr_t lpBaseAddress) {
		dataType lpBuffer;
		ReadProcessMemory(this->hProcess, (LPVOID*)lpBaseAddress, &lpBuffer, sizeof(lpBuffer), NULL);
		return lpBuffer;
	}

	//Write Process Memory Template
	template <class dataType>
	dataType writeMemory(uintptr_t lpBaseAddress, dataType lpBuffer) {
		WriteProcessMemory(this->hProcess, (LPVOID*)lpBaseAddress, &lpBuffer, sizeof(lpBuffer), NULL);
		return lpBuffer;
	}

};
