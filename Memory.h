#pragma once

#include <exception>
#include "Types.h"
#include <string>

/*  http://problemkaputt.de/gbatek-gba-memory-map.htm
	General Internal Memory
	00000000 - 00003FFF   BIOS - System ROM(16 KBytes)
	00004000 - 01FFFFFF   Not used
	02000000 - 0203FFFF   WRAM - On - board Work RAM(256 KBytes) 2 Wait
	02040000 - 02FFFFFF   Not used
	03000000 - 03007FFF   WRAM - On - chip Work RAM(32 KBytes)
	03008000 - 03FFFFFF   Not used
	04000000 - 040003FE   I / O Registers
	04000400 - 04FFFFFF   Not used
	Internal Display Memory
	05000000 - 050003FF   BG / OBJ Palette RAM(1 Kbyte)
	05000400 - 05FFFFFF   Not used
	06000000 - 06017FFF   VRAM - Video RAM(96 KBytes)
	06018000 - 06FFFFFF   Not used
	07000000 - 070003FF   OAM - OBJ Attributes(1 Kbyte)
	07000400 - 07FFFFFF   Not used
	External Memory(Game Pak)
	08000000 - 09FFFFFF   Game Pak ROM / FlashROM(max 32MB) - Wait State 0 (*!)
	0A000000 - 0BFFFFFF   Game Pak ROM / FlashROM(max 32MB) - Wait State 1 (*!)
	0C000000 - 0DFFFFFF   Game Pak ROM / FlashROM(max 32MB) - Wait State 2 (*!)
	0E000000 - 0E00FFFF   Game Pak SRAM(max 64 KBytes) - 8bit Bus width
	0E010000 - 0FFFFFFF   Not used
	Unused Memory Area
	10000000 - FFFFFFFF   Not used(upper 4bits of address bus unused)
*/

/* 
	(*!) The game pak ROM is mirrored to three address regions at 08000000h, 0A000000h, and 0C000000h, 
	these areas are called Wait State 0-2.
*/

class Memory
{
	friend class MemoryDump;
private:
	u8* buff_BIOS = new u8[BIOS_SIZE]; 
	u8* buff_EWRAM = new u8[EWRAM_SIZE];
	u8* buff_IWRAM = new u8[IWRAM_SIZE];
	u8* buff_IO = new u8[IO_SIZE];
	u8* buff_PAL = new u8[PAL_SIZE];
	u8* buff_VRAM = new u8[VRAM_SIZE];
	u8* buff_OAM = new u8[OAM_SIZE];
	u8* buff_ROM = new u8[ROM_SIZE];
	u8* buff_SRAM = new u8[SRAM_SIZE];

	struct
	{
		u32 zone; u32 size; u8* buffer;
	} mem_map[16] =
	{
		{(u32)0x00000000, BIOS_SIZE    , buff_BIOS},
		{(u32)0x01000000, 0            , nullptr},
		{(u32)0x02000000, EWRAM_SIZE   , buff_EWRAM},
		{(u32)0x03000000, IWRAM_SIZE   , buff_IWRAM},
		{(u32)0x04000000, IO_SIZE      , buff_IO},
		{(u32)0x05000000, PAL_SIZE     , buff_PAL},
		{(u32)0x06000000, VRAM_SIZE    , buff_VRAM},
		{(u32)0x07000000, OAM_SIZE     , buff_OAM},
		{(u32)0x08000000, ROM_SIZE     , buff_ROM},
		{(u32)0x09000000, ROM_SIZE / 2 , buff_ROM + ROM_SIZE / 2},
		{(u32)0x0A000000, ROM_SIZE     , buff_ROM},
		{(u32)0x0B000000, ROM_SIZE / 2 , buff_ROM + ROM_SIZE / 2},
		{(u32)0x0C000000, ROM_SIZE     , buff_ROM},
		{(u32)0x0D000000, ROM_SIZE / 2 , buff_ROM + ROM_SIZE / 2},
		{(u32)0x0E000000, SRAM_SIZE    , buff_SRAM},
		{(u32)0x0F000000, 0            , nullptr},
	};
	
public:
	u8* validate_offset(u32 offset) const;
	u8* validate_range(u32 offset1, u32 offset2) const;

	u8 operator[](u32) const;	

	u16 get16(u32 offset) const;
	u32 get32(u32 offset) const;

	void set_at(u32 offset, u8 value);

	void write(u32 offset, const void* data, u32 size);
	void fill(u32 offset1, u32 offset2, u32 value);

	~Memory();

	
public:
	static const u32 BIOS_SIZE = (u32)0x4000;
	static const u32 EWRAM_SIZE = (u32)0x40000;
	static const u32 IWRAM_SIZE = (u32)0x8000;
	static const u32 IO_SIZE = (u32)0x3FF; // (u32)0x400?
	static const u32 PAL_SIZE = (u32)0x400;
	static const u32 VRAM_SIZE = (u32)0x18000;
	static const u32 OAM_SIZE = (u32)0x400;
	static const u32 ROM_SIZE = (u32)0x2000000;
	static const u32 SRAM_SIZE = (u32)0x10000;	

	static const u32 BIOS_OFFSET  = (u32)0x00000000;
	static const u32 EWRAM_OFFSET = (u32)0x02000000;
	static const u32 IWRAM_OFFSET = (u32)0x03000000;
	static const u32 IO_OFFSET    = (u32)0x04000000;
	static const u32 PAL_OFFSET   = (u32)0x05000000;
	static const u32 VRAM_OFFSET  = (u32)0x06000000;
	static const u32 OAM_OFFSET   = (u32)0x07000000;
	static const u32 ROM0_OFFSET  = (u32)0x08000000;
	static const u32 ROM1_OFFSET  = (u32)0x0A000000;
	static const u32 ROM2_OFFSET  = (u32)0x0C000000;
	static const u32 SRAM_OFFSET  = (u32)0x0E000000;
};

class MemoryDump
{
private:
	const Memory* memory;
	u8* dumped_buffer = nullptr;
	u32 size = 0;
public:
	enum class DumpType
	{
		BIOS    = 0,
		EWRAM   = 2,
		IWRAM   = 3,
		IO      = 4,
		PALETTE = 5,
		VRAM    = 6,
		OAM     = 7,
		ROM     = 8,
		SRAM    = 14
	};

	MemoryDump(const Memory* memory, DumpType dump_type);	

	void write_to_file(const std::string& filename);
	
	~MemoryDump();
};

class InvalidMemoryAccess : public std::exception
{
private:
	u32 offset = 0;
public:
	InvalidMemoryAccess(const char* msg = "Invalid memory access");
	InvalidMemoryAccess(const char* msg, u32 offset);
};