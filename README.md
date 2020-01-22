# Memory

Memory hacking for fun and practice

#### Usage

```
//Include the MemoryManger.cpp file
#include "MemoryManager.cpp"

//Create a object
MemoryManager Memory;

//Get the process ID of the target process and create a handle to the target process
uintptr_t targetProcess = Memory.GetProcessID("example.exe");

//Get the module ID of the module from the target process
uintptr_t targetModule = Memory.GetModuleID(targetProcess, "example.dll");
```

### References

* https://guidedhacking.com/
* https://docs.microsoft.com/
