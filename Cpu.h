#pragma once
#include "Memory.h"
#include "Instruction.h"
#include <iostream>

class Instruction;

class Cpu
{
public:
	enum class InstructionState
	{
		Thumb = 0,
		ARM = 1
	};
private:
	Memory* memory;

	// registers
	u32 R[16];
	u32 R_fiq[7]; // 8+index
	u32 R_svc[2]; // 13,14
	u32 R_abt[2]; // 13,14
	u32 R_irq[2]; // 13,14
	u32 R_und[2]; // 13,14

	u32 CPSR;
	u32 SPSR_fiq;
	u32 SPSR_svc;
	u32 SPSR_abt;
	u32 SPSR_irq;
	u32 SPSR_und;

	u32& PC = R[15];
	InstructionState instruction_state = InstructionState::ARM;

	Instruction* pipeline[3] = { nullptr, nullptr, nullptr };
public:
	Cpu(Memory* memory);

	void do_cycle();	

};

