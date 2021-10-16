#include "internal.h"
#include <Psapi.h>

uintptr_t GetModuleBaseAddress(const wchar_t* modName) {
	return (uintptr_t)GetModuleHandleW(modName);
}

uintptr_t FindDynamicAddress(uintptr_t ptr, std::vector<unsigned int> offsets) {
	for (unsigned int i = 0; i < offsets.size(); i++) {
		ptr = *(BYTE*)ptr;
		ptr += offsets[i];
	}
	return ptr;
}

void PatchInternal(BYTE* dst, BYTE* src, size_t size) {
	DWORD oldprotect; //variable to hold a backup of our old protection
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect); //make memory writeable and save the old protection in oldprotect
	memcpy(dst, src, size); //write the new opcodes to the target location
	VirtualProtect(dst, size, oldprotect, &oldprotect); //restore our old protection
}

NopInternal::NopInternal(BYTE* dst, size_t size):dst{ dst }, size{ size }, originalCode{ new BYTE[size] }, nopCode{ new BYTE[size] } {
	memset(nopCode, 0x90, size); //initialize our nopCode
	memcpy(originalCode, dst, size); //backup our original Code
}

void NopInternal::enable() { //enable our patch
	//write the code that does nothing to memory:
	PatchInternal(dst, nopCode, size);
}

void NopInternal::disable() { //disable our patch
	//write the original code back to memory:
	PatchInternal(dst, originalCode, size);
}

NopInternal::~NopInternal() { //clean up our patch object
	delete[] this->originalCode;
	delete[] this->nopCode;
}

ManagedPatch::ManagedPatch(BYTE* dst, BYTE* src, size_t size)
	: dst{ dst }, size{ size }, originalCode{ new BYTE[size] }, newCode{ new BYTE[size] } {
	memcpy(newCode, src, size); //initialize our newCode
	memcpy(originalCode, dst, size); //backup our original Code
}

void ManagedPatch::enable() { //enable our patch
	//write the code that does nothing to memory:
	PatchInternal(dst, newCode, size);
}

void ManagedPatch::disable() { //disable our patch
	//write the original code back to memory:
	PatchInternal(dst, originalCode, size);
}

ManagedPatch::~ManagedPatch() { //clean up our patch object
	delete[] this->originalCode;
	delete[] this->newCode;
}

Hook::Hook(void* toHook, void* ourFunct, int len) : tToHook{ toHook }, oldOpcodes{ nullptr }, tLen{ len }, enabled{ false } {
	if (len < 5) {
		return;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	oldOpcodes = std::make_unique<char[]>(len);
	if (oldOpcodes != nullptr) {
		for (int i = 0; i < len; i++) {
			oldOpcodes[i] = ((char*)toHook)[i];
		}
	}

	memset(toHook, 0x90, len);

	DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	VirtualProtect(toHook, len, curProtection, &curProtection);
}

Hook::~Hook() {
	if (oldOpcodes != nullptr) {
		DWORD curProtection;
		VirtualProtect(tToHook, tLen, PAGE_EXECUTE_READWRITE, &curProtection);
		for (int i = 0; i < tLen; ++i) { 
			((char*)tToHook)[i] = Hook::oldOpcodes[i];
		}
		VirtualProtect(tToHook, tLen, curProtection, &curProtection);
	}
}

void Hook::enable() {
	this->enabled = true;
}

void Hook::disable() {
	this->enabled = false;
}

bool Hook::isEnabled() {
	return enabled;
}

TrampHook::TrampHook(void* toHook, void* ourFunct, int len) :
	gateWay{ nullptr }, managedHook{ nullptr } {

	if (len < 5) return;
	gateWay = VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy_s(gateWay, len, toHook, len);
	uintptr_t returnAddress = (BYTE*)toHook - (BYTE*)gateWay - 5;
	*(BYTE*)((uintptr_t)gateWay + len) = 0xE9;
	*(uintptr_t*)((uintptr_t)gateWay + len + 1) = returnAddress;

	managedHook = new Hook(toHook, ourFunct, len);
}

TrampHook::~TrampHook() {
	managedHook->disable();
	delete managedHook;
	VirtualFree(gateWay, 0, MEM_RELEASE);
}

void TrampHook::enable() {
	managedHook->enable();
}

void TrampHook::disable() {
	managedHook->disable();
}

bool TrampHook::isEnabled() {
	return managedHook->isEnabled();
}

void* TrampHook::getGateway() {
	return gateWay;
}

unsigned int FindPattern(std::string moduleName, std::string pattern, bool relativeOffset)
{
	const char* pat = pattern.c_str();
	uintptr_t firstMatch = 0;
	uintptr_t rangeStart = (uintptr_t)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	uintptr_t rangeEnd = rangeStart + miModInfo.SizeOfImage;

	for (uintptr_t pCur = rangeStart; pCur < rangeEnd; pCur++) {
		if (!*pat)
			break;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				break;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;
		}
		else {
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}

	if (relativeOffset)
		firstMatch = firstMatch + 0x5 + *(DWORD*)(firstMatch + 0x1);
	return firstMatch;
}