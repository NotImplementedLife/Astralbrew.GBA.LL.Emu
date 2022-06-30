#pragma once

#include "Instruction.h"

#include <map>

class ARMInstruction : public Instruction
{
public:
	enum class Type
	{
		/// <summary>
		/// Unknown opcode
		/// </summary>
		Unknown,		
		
		/// <summary>
		/// ALU, 2nd Op Register, Shiby by Immediate
		/// </summary>
		DataProc_Reg_ShImm,        

		/// <summary>
		/// ALU, 2nd Op Register, Shift by Register
		/// </summary>
		DataProc_Reg_ShReg,

		/// <summary>
		/// ALU, 2nd Op Immediate
		/// </summary>
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

	static std::string alu_name(u8 opcode);

	bool is_valid() const;
public:
	ARMInstruction(u32 address, u32 opcode);
	virtual void decode() override;
	virtual void execute(Cpu* cpu) override;
	virtual std::string to_string(const InstructionFormat& format = DefaultInstructionFormat) override;

private:
	bool valid_alu() const;
};

