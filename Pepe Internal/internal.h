#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <memory>
#include <string>

#define INRANGE(x,a,b)    (x >= a && x <= b) 

#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))


uintptr_t GetModuleBaseAddress(const wchar_t* modName);

uintptr_t FindDynamicAddress(uintptr_t ptr, std::vector<unsigned int> offsets);

void PatchInternal(BYTE* dst, BYTE* src, size_t size);

class ManagedPatch {
	BYTE* dst; //address of the code to patch
	size_t size; //size of the patch
	BYTE* originalCode; //backup of the original code
	BYTE* newCode; //new functionality
public:
	ManagedPatch(BYTE* dst, BYTE* src, size_t size); //constructor
	~ManagedPatch(); //destructor
	void enable(); //enable the patch
	void disable(); //disable the patch
};
class NopInternal { //class for replacing code with code that does nothing
private:
	BYTE* dst; //address of the code to patch
	size_t size; //size of the patch
	BYTE* originalCode; //backup of the original code
	BYTE* nopCode; //code that does nothing
public:
	NopInternal(BYTE* dst, size_t size); //constructor
	~NopInternal(); //destructor
	void enable(); //enable the patch
	void disable(); //disable the patch
};


class Hook {
	void* tToHook;
	std::unique_ptr<char[]> oldOpcodes;
	int tLen;
	bool enabled;
public:
	Hook(void* toHook, void* ourFunct, int len);
	~Hook();
	void enable();
	void disable();
	bool isEnabled();
};

class TrampHook { //easy to use class for hooking functions
	void* gateWay;
	Hook* managedHook;
public:
	TrampHook(void* toHook, void* ourFunct, int len);
	~TrampHook(); //restores original code
	void enable();
	void disable();
	bool isEnabled();
	void* getGateway();
};

unsigned int FindPattern(std::string moduleName, std::string pattern, bool relativeOffset = false);
