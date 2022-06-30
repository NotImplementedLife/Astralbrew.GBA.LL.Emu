#pragma once
#include "Cpu.h"
#include "InstructionFormat.h"

#define __bits__(h,l,v) (v<<l)
#define __get_bits__(n,h,l) ((u32)((((u64)n)>>l)&(((1LL<<(h-l+1)))-1)))
#define __has_bits__(n,h,l,v) (__get_bits__(n,h,l)==v)
#define __get_bit__(n,p) ((n>>p)&1)

class Cpu;

class Instruction
{
protected:
	u32 address = 0;
public:
	virtual void decode() = 0;
	virtual void execute(Cpu* cpu) = 0;
	virtual std::string to_string(const InstructionFormat& format = DefaultInstructionFormat);

	static std::string condition_suffix(u8 cond);
};

