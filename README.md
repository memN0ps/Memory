# Memory
Memory hacking etc....

Made some of these a long time ago just for fun.

#### How to use?
Example below:

```
#include "MemoryManager.h"

//Create a handle to the target process

MemoryManager memory;

uintptr_t targetProcess = memory.getProcessID("example.exe");
uintptr_t targetModule = memory.getModuleID(targetProcess, "example.dll");
```
