#include "StorageTransactions.h"

#include <fstream>

void StorageTransactions::load_BIOS(Memory* memory, const void* source, u32 len)
{
	if (len > Memory::BIOS_SIZE)
	{
		throw ROMLoadingException("BIOS size must not be greater than 16KB");
	}
	memory->fill(Memory::BIOS_OFFSET, Memory::BIOS_OFFSET + Memory::BIOS_SIZE, 0xFFFFFFFF);
	memory->write(Memory::BIOS_OFFSET, source, len);
}

void StorageTransactions::load_BIOS(Memory* memory, const std::string& filename)
{
	std::ifstream file(filename, std::ios::ios_base::binary);

	file.seekg(0, std::ios::end);
	u64 len = file.tellg();
	file.seekg(0);

	if (len > Memory::BIOS_SIZE)
	{
		file.close();
		throw ROMLoadingException("BIOs size must not be greater than 16KB");
	}

	memory->fill(Memory::BIOS_OFFSET, Memory::BIOS_OFFSET + Memory::BIOS_SIZE, 0xFFFFFFFF);
	u8* data = new u8[(u32)len];
	file.read((char*)data, (u32)len);
	load_BIOS(memory, data, (u32)len);
	delete[] data;

	file.close();
}

void StorageTransactions::load_GBA(Memory* memory, const void* source, u32 len)
{
	if (len > Memory::ROM_SIZE)
	{
		throw ROMLoadingException("ROM size must not be greater than 32MB");
	}
	memory->fill(Memory::ROM0_OFFSET, Memory::ROM0_OFFSET + Memory::ROM_SIZE, 0xFFFFFFFF);
	memory->write(Memory::ROM0_OFFSET, source, len);
}

void StorageTransactions::load_GBA(Memory* memory, const std::string& filename)
{
	std::ifstream file(filename, std::ios::ios_base::binary);

	file.seekg(0, std::ios::end);
	u64 len = file.tellg();
	file.seekg(0);

	if (len > Memory::ROM_SIZE)
	{
		file.close();
		throw ROMLoadingException("ROM size must not be greater than 32MB");
	}

	memory->fill(Memory::ROM0_OFFSET, Memory::ROM0_OFFSET + Memory::ROM_SIZE, 0xFFFFFFFF);
	u8* data = new u8[(u32)len];
	file.read((char*)data, (u32)len);

	load_GBA(memory, data, (u32)len);	
	delete[] data;

	file.close();
}

ROMLoadingException::ROMLoadingException(const char* msg) : std::exception(msg) { }