#include "Memory.h"
#include <string.h>
#include <fstream>

u8* Memory::validate_offset(u32 offset) const
{
	if (offset & 0xF0000000)
	{
		throw InvalidMemoryAccess("Upper 4bits of address bus unused");
	}

	u32 zone_index = (offset & 0x0F000000) >> 24;	
	u32 relative_offset = offset - mem_map[zone_index].zone;
	if (relative_offset >= mem_map[zone_index].size)
	{
		throw InvalidMemoryAccess("Offset out of zone");
	}
	return mem_map[zone_index].buffer + relative_offset;
}

u8* Memory::validate_range(u32 offset1, u32 offset2) const
{
	if (offset1 >= offset2)
	{
		throw InvalidMemoryAccess("Reversed interval");
	}
	if ((offset1 | offset2) & 0xF0000000)
	{
		throw InvalidMemoryAccess("Upper 4bits of address bus unused");
	}
	u32 zone_index = (offset1 & 0x0F000000) >> 24;	
	u32 zone_index2 = (offset2 & 0x0F000000) >> 24;	
	if (zone_index != zone_index2 && (zone_index >= 8 && zone_index2 < 14 && zone_index % 2 == 0 && zone_index2 - zone_index != 1))
	{
		throw InvalidMemoryAccess("Range in different memmory zones");
	}
	u32 relative_offset1 = offset1 - mem_map[zone_index].zone;
	if (relative_offset1 >= mem_map[zone_index].size)
	{
		throw InvalidMemoryAccess("Offset out of zone");
	}
	u32 relative_offset2 = offset2 - mem_map[zone_index].zone;
	if (relative_offset2 >= mem_map[zone_index].size)
	{
		throw InvalidMemoryAccess("Offset out of zone");
	}
	return mem_map[zone_index].buffer + relative_offset1;
}

u16 Memory::get16(u32 offset) const
{
	return *((u16*)validate_offset(offset));
}

u32 Memory::get32(u32 offset) const
{
	return *((u32*)validate_offset(offset));
}

u8 Memory::operator[](u32 offset) const
{
	return *validate_offset(offset);
}

void Memory::set_at(u32 offset, u8 byte)
{
	*validate_offset(offset) = byte;
}

void Memory::write(u32 offset, const void* data, u32 size)
{
	u8* dest = validate_range(offset, offset + size - 1);
	memcpy(dest, data, size);
}

void Memory::fill(u32 offset1, u32 offset2, u32 value)
{
	u32* dest = (u32*)validate_range(offset1, offset2-1);
	int n_dwords = (offset2 - offset1)/4;
	if (n_dwords > 0)
	{
		for (; n_dwords--; *(dest++) = value);
	}
	int n_bytes = (offset2 - offset1) & 3;
	if (n_bytes == 0) return;
	for (u8* dest8=(u8*)dest; n_bytes--;)
	{
		*(dest8++) = value & 0xFF;
		value >>= 8;
	}
}

Memory::~Memory()
{
	delete[] buff_BIOS;
	delete[] buff_IWRAM;
	delete[] buff_EWRAM;
	delete[] buff_IO;
	delete[] buff_PAL;
	delete[] buff_VRAM;
	delete[] buff_OAM;
	delete[] buff_ROM;
	delete[] buff_SRAM;
}


MemoryDump::MemoryDump(const Memory* memory, MemoryDump::DumpType dump_type) : memory{memory}
{	
	int zone_id = (int)dump_type;
	size = memory->mem_map[zone_id].size;
	dumped_buffer = new u8[size];
	memcpy(dumped_buffer, memory->mem_map[zone_id].buffer, size);
}

void MemoryDump::write_to_file(const std::string& filename)
{
	std::ofstream file(filename, std::ios::ios_base::binary);
	file.write((char*)dumped_buffer, size);
	file.close();
}

MemoryDump::~MemoryDump()
{
	delete[] dumped_buffer;
}

InvalidMemoryAccess::InvalidMemoryAccess(const char* msg) : std::exception(msg) { }
InvalidMemoryAccess::InvalidMemoryAccess(const char* msg, u32 offset) : std::exception(msg), offset(offset) { }