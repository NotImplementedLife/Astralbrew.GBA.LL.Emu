#pragma once
#include "Types.h"
#include <string>

class ThumbInstruction
{
private:	
	u32 code;
	int Rd, Rn, Rm, imm5, imm8, cnst;
public:
	ThumbInstruction(u16 code);

	bool requires_word() const;

	void set_upper_halfword(u16 hw);

	bool is_arithmetic() const;

	std::string to_string() const;

	enum class InstructionType
	{
		UNK,
		LSL,
		LSR,
		ASR,
		ADDr,
		SUBr,
		ADDi3,
		SUBi3,
		MOVi,
		CMPi,
		ADDi8,
		SUBi8,
		MOVr
	};
};

class ThumbDecoder
{
public:
	static ThumbInstruction decode(const u16* buffer);
};