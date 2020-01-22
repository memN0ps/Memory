#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

//Author memN0ps

/**
This class gets the process ID and the module base address of the target process provided, it also has a template for ReadProcessMemory and WriteProcessMemory
**/

class MemoryManager {
private:
	//Private member to open a handle to the process
	HANDLE hProcess;

public:
	//Constructor
	MemoryManager() {
		hProcess = NULL;
	}

	//Destructor
	~MemoryManager() {
		CloseHandle(hProcess);
	}

	//Gets the process ID of the given target process

	/**
	The wchar_t type is an implementation-defined wide character type. In the Microsoft compiler, it represents a 16-bit wide character used to store Unicode encoded as UTF-16LE, 
	the native character type on Windows operating systems. The wide character versions of the Universal C Runtime (UCRT) library functions use wchar_t and its pointer and array 
	types as parameters and return values, as do the wide character versions of the native Windows API.
	*/
	uintptr_t GetProcessID(const wchar_t* targetProcess) {

		uintptr_t processID = NULL;

		//CreateToolhelp32Snapshot: Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes.
		//TH32CS_SNAPPROCESS: Includes all processes in the system in the snapshot. To enumerate the processes, see Process32First.
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		//Check to see if the snapshot is created
		if (hSnapshot == INVALID_HANDLE_VALUE) {
			std::cout << "Failed to take a snapshot for GetProcessID!" << std::endl;
			return false;
		}


		//PROCESSENTRY32: Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
		//dwSize: The size of the structure, in bytes. Before calling the Process32First function, set this member to sizeof(PROCESSENTRY32). If you do not initialize dwSize, Process32First fails.
		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(processEntry);

		//Process32First: Retrieves information about the first process encountered in a system snapshot.
		//hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
		//processEntry: A pointer to a PROCESSENTRY32 structure. It contains process information such as the name of the executable file, the process identifier, and the process identifier of the parent process.
		if (Process32First(hSnapshot, &processEntry)) {
		
			//Loop through all the processes recorded in a system snapshot until the matched process is found.
			do {
				/**The strcmp function performs an ordinal comparison of string1 and string2 and returns a value that indicates their relationship. 
				*wcscmp and _mbscmp are, respectively, wide-character and multibyte-character versions of strcmp. 
				*_mbscmp recognizes multibyte-character sequences according to the current multibyte code page and returns _NLSCMPERROR on an error.
				**
				*The wchar_t, a.k.a. wide characters, provides more room for encodings.
				*Use char data type when the range of encodings is 256 or less, such as ASCII. Use wchar_t when you need the capacity for more than 256.
				**/
				//Return value is 0 if string1 is identical to string2
				////szExeFile: The name of the executable file for the process.
				if (!wcscmp(processEntry.szExeFile, targetProcess)) {
					//Found Process ID
					//th32ProcessID: The process identifier.
					processID = processEntry.th32ProcessID;
					std::wcout << "Found Process " << processEntry.szExeFile << " with process ID " << processID << std::endl;

					//OpenProcess: Opens an existing local process object.
					//bInheritHandle: false: If this value is TRUE, processes created by this process will inherit the handle. Otherwise, the processes do not inherit this handle.
					//processID: The identifier of the local process to be opened.
					this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);

					if (hProcess == INVALID_HANDLE_VALUE) {
						std::wcout << "Failed to get a handle to the process " << processEntry.szExeFile << " with process ID " << processID << std::endl;
						CloseHandle(hSnapshot);
						return false;
					}

					//Closes an open object handle.
					CloseHandle(hSnapshot);

					return processID;
				}

				//Process32Next: Retrieves information about the next process recorded in a system snapshot.
				//hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
				//A pointer to a PROCESSENTRY32 structure.
			} while (Process32Next(hSnapshot, &processEntry));
		}

		CloseHandle(hSnapshot);
		return false;
	}

	//Get module base address of the given target module
	uintptr_t GetModuleID(uintptr_t processID, const wchar_t* targetModule) {

		uintptr_t moduleID = NULL;

		//CreateToolhelp32Snapshot: Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes.
		/**
		TH32CS_SNAPMODULE: Includes all modules of the process specified in th32ProcessID in the snapshot. To enumerate the modules, see Module32First. 
		If the function fails with ERROR_BAD_LENGTH, retry the function until it succeeds. 
		
		64-bit Windows: Using this flag in a 32-bit process includes the 32-bit modules of the process
		specified in th32ProcessID, while using it in a 64-bit process includes the 64-bit modules. 
		To include the 32-bit modules of the process specified in th32ProcessID from a 64-bit process, use the TH32CS_SNAPMODULE32 flag.

		TH32CS_SNAPMODULE32: Includes all 32-bit modules of the process specified in th32ProcessID in the snapshot when called from a 64-bit process.
		This flag can be combined with TH32CS_SNAPMODULE or TH32CS_SNAPALL. If the function fails with ERROR_BAD_LENGTH, retry the function until it succeeds.

		values combined using an OR operation ('|').

		The process identifier of the process to be included in the snapshot. This parameter can be zero to indicate the current process. 
		This parameter is used when the TH32CS_SNAPHEAPLIST, TH32CS_SNAPMODULE, TH32CS_SNAPMODULE32, or TH32CS_SNAPALL value is specified. 
		Otherwise, it is ignored and all processes are included in the snapshot.
		**/
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);

		if (hSnapshot == INVALID_HANDLE_VALUE) {
			std::cout << "Failed to take a snapshot for GetModuleID!" << std::endl;
			CloseHandle(hSnapshot);
			return false;
		}

		//MODULEENTRY32: Describes an entry from a list of the modules belonging to the specified process.
		//dwSize: The size of the structure, in bytes. Before calling the Module32First function, set this member to sizeof(MODULEENTRY32). If you do not initialize dwSize, Module32First fails.
		MODULEENTRY32 moduleEntry;
		moduleEntry.dwSize = sizeof(moduleEntry);

		
		//Module32First: Retrieves information about the first module associated with a process.
		//hSnapshot: A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
		//moduleEntry: A pointer to a MODULEENTRY32 structure.
		if (Module32First(hSnapshot, &moduleEntry)) {
			//Loop through all the processes recorded in a system snapshot until the matched process is found.
			do {
				/**The strcmp function performs an ordinal comparison of string1 and string2 and returns a value that indicates their relationship.
				*wcscmp and _mbscmp are, respectively, wide-character and multibyte-character versions of strcmp.
				*_mbscmp recognizes multibyte-character sequences according to the current multibyte code page and returns _NLSCMPERROR on an error.
				**
				*The wchar_t, a.k.a. wide characters, provides more room for encodings.
				*Use char data type when the range of encodings is 256 or less, such as ASCII. Use wchar_t when you need the capacity for more than 256.
				**/
				//Return value is 0 if string1 is identical to string2
				//szExeFile: The name of the executable file for the process.
				if (!wcscmp(moduleEntry.szModule, targetModule)) {
					//Found Module ID
					//th32ProcessID: The process identifier.
					//modBaseAddr: The base address of the module in the context of the owning process.
					moduleID = (uintptr_t)moduleEntry.modBaseAddr;
					std::wcout << "Found Process " << moduleEntry.szModule << " with module ID " << moduleID << std::endl;

					//Closes an open object handle.
					CloseHandle(hSnapshot);

					return moduleID;
				}

				//Module32Next: Retrieves information about the first module associated with a process.
				//A handle to the snapshot returned from a previous call to the CreateToolhelp32Snapshot function.
				//moduleEntry: A pointer to a MODULEENTRY32 structure.
			} while (Module32Next(hSnapshot, &moduleEntry));
		}

		CloseHandle(hSnapshot);
		return false;
	}

	//ReadProcess Memory Template
	//ReadProcessMemory copies the data in the specified address range from the address space of the specified process into the specified buffer of the current process. 
	//Any process that has a handle with PROCESS_VM_READ access can call the function.
	template <class dataType>
	dataType ReadMemory(dataType lpBaseAddress) {
		dataType lpBuffer;
		ReadProcessMemory(this->hProcess, (LPVOID*)lpBaseAddress, &lpBuffer, sizeof(lpBuffer), NULL);
		return lpBuffer;
	}

	//WriteProcessMemory Template
	//WriteProcessMemory copies the data from the specified buffer in the current process to the address range of the specified process. 
	//Any process that has a handle with PROCESS_VM_WRITE and PROCESS_VM_OPERATION access to the process to be written to can call the function. 
	//Typically but not always, the process with address space that is being written to is being debugged.

	//The entire area to be written to must be accessible, and if it is not accessible, the function fails.
	template <class dataType>
	dataType WriteMemory(dataType lpBaseAddress, dataType lpBuffer) {
		WriteProcessMemory(this->hProcess, (LPVOID*)lpBaseAddress, &lpBuffer, sizeof(lpBuffer), NULL);
		return lpBuffer;
	}

	/**Finds multi level pointers
	A pointer itself points to only one address. But when you have a list of offsets you can walk that pointer chain to find the end address.

	When you find a pointer in Cheat Engine, what you're finding is a path from one address to another using pointers and relative offsets. 
	This is how computers locate and act on data stored in memory.
	
	The logic behind this is based on 2 important features of modern object oriented programming:
		Applications are made memory efficient by dynamically allocating memory objects only when needed and assigning pointers to point at them only when needed.
		Classes can contain member variables that are pointers, specifically pointers to other objects in memory.
	
	The baseAddress or first pointer in the pointer chain is generally one that exists in static memory, 
	meaning it's always located at the same address or can be accessed using a relative offset from the base address of a module.
	/**
	Address = Value = ?

	base ptr -> address + offset4 = address

	base ptr -> address + offset3 = address

	base ptr -> address + offset2 = address

	static base -> address + offset1 = address
	**/
	uintptr_t FindDMAAddy(uintptr_t pointer, std::vector<unsigned int> offsets)
	{
		uintptr_t address = pointer;
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			ReadProcessMemory(this->hProcess, (BYTE*)address, &address, sizeof(address), 0);
			address += offsets[i];
		}
		return address;
	}

	/**
	References:
	https://guidedhacking.com/
	https://docs.microsoft.com/
	**/

};


