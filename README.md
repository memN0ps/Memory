# Memory

Memory hacking for fun and practice, please note this is just an example soley for offline and educational purposes ;)

#### Usage

```
//Include the MemoryManger.cpp file
#include "MemoryManager.cpp"

//Create a object
MemoryManager Memory;

//Get the process ID of the target process and create a handle to the target process with PROCESS_ALL_ACCESS
uintptr_t targetProcess = Memory.GetProcessID("example.exe");

//Get the module ID of the module from the target process
uintptr_t targetModule = Memory.GetModuleID(targetProcess, "example.dll");


DWORD dwLocalPlayer = 0x1337;
DWORD m_iHealth = 0xBEEF;

//ReadProcessMemory from the target process of a player
DWORD localPlayer = ReadMemory<DWORD>(targetModule + dwLocalPlayer);

//If player is not found then keeping looping through until the player is found
if (localPlayer == NULL)
	while (localPlayer == NULL)
		localPlayer = ReadMemory<DWORD>(targetModule + dwLocalPlayer);

int newHealth = 1337;

//Keep it running
while (true) {
    
    //Write 1337 to the players health when space bar is pressed
    
    if (GetAsyncKeyState(VK_SPACE)) {
        WriteMemory<int>(localPlayer + m_iHealth, newHealth);
        std::cout << "Get Rekt!" << std::endl;
    }
}

```

### References

* https://guidedhacking.com/
* https://docs.microsoft.com/
