#pragma once

#include "Instruction.h"

#include <map>

class ARMInstruction : public Instruction
{
public:
	enum class Type
	{
		Unknown,
		DataProc_Reg,
		DataProc_Reg_ShReg,
		DataProc_Imm,
		PSR_Imm,
		PSR_Reg,
		BX_BLX,
		Multiply,
		MulLong,		
		TransSwp12,
		TransReg10,
		TransImm10,
		TransImm9,
		TransReg9,
		Undefined,
		BlockTrans,
		B_BL_BLX_Offset,
		CoDataTrans,
		CoDataOp,
		CoRegTrans,
		SWI,
	};
private:
	u32 opcode;

	Type type = Type::Unknown;	
	std::map<std::string, u32> data;
public:
	ARMInstruction(u32 address, u32 opcode);
	virtual void decode() override;
	virtual void execute(Cpu* cpu) override;
	virtual std::string to_string(const InstructionFormat& format = DefaultInstructionFormat) override;
};

