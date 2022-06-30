#include <iostream>

#include "Memory.h"
#include "StorageTransactions.h"
#include "Cpu.h"

int main()
{
    Memory* memory = new Memory();

    StorageTransactions::load_BIOS(memory, "bios\\gba_bios.bin");
    StorageTransactions::load_GBA(memory, "roms\\main.gba");

    Cpu cpu(memory);
    
    for (int i = 0; i < 0x1BC/4; i++)
        cpu.do_cycle();


    MemoryDump(memory, MemoryDump::DumpType::ROM).write_to_file("rom_dump.bin");

    delete memory;    
}

