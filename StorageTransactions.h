#pragma once
#include <string>
#include "Types.h"
#include "Memory.h"
#include <exception>

class StorageTransactions
{
public:
	static void load_BIOS(Memory* memory, const void* source, u32 len);
	static void load_BIOS(Memory* memory, const std::string& filename);
	static void load_GBA(Memory* memory, const void* source, u32 len);
	static void load_GBA(Memory* memory, const std::string& filename);
};

class ROMLoadingException : public std::exception
{
public:
	ROMLoadingException(const char* msg = "Failed to load GBA ROM");
};