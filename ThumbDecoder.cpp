#include "ThumbDecoder.h"

#include <sstream>
#include <iostream>

struct __IntructionTeller16
{
	u16 bits;
	u16 mask;
	ThumbInstruction::InstructionType type;
};

__IntructionTeller16 __InstrTeller16[];
extern const int __InstrTeller16Count;

ThumbInstruction::InstructionType tell_instruction16(u16 code)
{
	for (int i = 0; i < __InstrTeller16Count; i++)
	{
		if ((code & __InstrTeller16[i].mask) == __InstrTeller16[i].bits)
			return __InstrTeller16[i].type;
	}
	return ThumbInstruction::InstructionType::UNK;
}

ThumbInstruction::ThumbInstruction(u16 code)
{
	this->code = code;	
}

bool ThumbInstruction::requires_word() const
{
	return (code & 0xE000) == 0xE000;
}

void ThumbInstruction::set_upper_halfword(u16 hw)
{
	std::cout << std::hex<< code << ' ';
	code |= hw << 16;
	std::cout << std::hex<< code << '\n';
}

bool ThumbInstruction::is_arithmetic() const
{
	return (u16)code < 0x2000; // 000xx...(16bit) < 0010_00..
}



ThumbInstruction ThumbDecoder::decode(const u16* buffer)
{
	return ThumbInstruction(0);
}


std::string ThumbInstruction::to_string() const
{
	std::stringstream ss;
	
	ss << "[";
	ss.fill('0');			
	ss.width(8);
	ss << std::hex << std::fixed << code;
	ss.clear();
	ss << "] ";

	auto itype = tell_instruction16(code);

	switch (itype)
	{	
	case ThumbInstruction::InstructionType::LSL:
		ss << "LSL";  break;
	case ThumbInstruction::InstructionType::LSR:
		ss << "LSR"; break;
	case ThumbInstruction::InstructionType::ASR:
		ss << "ASR"; break;
	case ThumbInstruction::InstructionType::ADDr:
		ss << "ADD"; break;
	case ThumbInstruction::InstructionType::SUBr:
		ss << "SUB"; break;
	case ThumbInstruction::InstructionType::ADDi3:
		ss << "ADD"; break;
	case ThumbInstruction::InstructionType::SUBi3:
		ss << "SUB"; break;
	case ThumbInstruction::InstructionType::MOVi:
		ss << "MOV"; break;
	case ThumbInstruction::InstructionType::CMPi:
		ss << "CMP"; break;
	case ThumbInstruction::InstructionType::ADDi8:
		ss << "ADD"; break;
	case ThumbInstruction::InstructionType::SUBi8:
		ss << "SUB"; break;
	case ThumbInstruction::InstructionType::MOVr:
		ss << "MOV"; break;
	case ThumbInstruction::InstructionType::UNK:
		ss << "[Unknown]"; break;
	}

	return ss.str();
}

// Atrihmetic instructions 00xxxx_yyyyyyyyyyy
const u16 b_ARITH = (u16)0b0000000000000000;
const u16 m_ARITH = (u16)0b1100000000000000;


// When Opcode is 0b00000, and bits[8:6] are 0b000, this is an encoding for MOV (register, Thumb).
const u16 b_MOVr  = b_ARITH | (u16)0b0000000000000000;
const u16 m_MOVR  = m_ARITH | (u16)0b0011111111000000;


const u16 b_LSL     = b_ARITH | (u16)0b00000000000000; // Shift left
const u16 b_LSR     = b_ARITH | (u16)0b00100000000000; // Shift right
const u16 b_ASR     = b_ARITH | (u16)0b01000000000000; // Arithmetic shift right
const u16 b_MOVi    = b_ARITH | (u16)0b10000000000000; // Arithmetic shift right
const u16 b_CMPi    = b_ARITH | (u16)0b10100000000000; // Arithmetic shift right
const u16 b_ADDi8   = b_ARITH | (u16)0b11000000000000; // Arithmetic shift right
const u16 b_SUBi8   = b_ARITH | (u16)0b11100000000000; // Arithmetic shift right
const u16 m_00111xx = m_ARITH | (u16)0b11100000000000;

const u16 b_ADDr    = b_ARITH | (u16)0b01100000000000; // Add register
const u16 b_SUBr    = b_ARITH | (u16)0b01101000000000; // Sub register
const u16 b_ADDi3   = b_ARITH | (u16)0b01110000000000; // Add 3-bit immediate
const u16 b_SUBi3   = b_ARITH | (u16)0b01111000000000; // Sub 3-bit immediate
const u16 m_0011111 = m_ARITH | (u16)0b11111000000000;


#define __tell_instr16__(name, mask) { b_##name  , m_##mask , ThumbInstruction::InstructionType::name  }

__IntructionTeller16 __InstrTeller16[] =
{
	// Arithmetic instructions
	__tell_instr16__(MOVr , MOVR),
	__tell_instr16__(LSL  , 00111xx),
	__tell_instr16__(LSR  , 00111xx),
	__tell_instr16__(ASR  , 00111xx),
	__tell_instr16__(ADDr , 0011111),	
	__tell_instr16__(SUBr , 0011111),	
	__tell_instr16__(ADDi3, 0011111),	
	__tell_instr16__(SUBi3, 0011111),	
	__tell_instr16__(MOVi  , 00111xx),
	__tell_instr16__(CMPi  , 00111xx),
	__tell_instr16__(ADDi8 , 00111xx),
	__tell_instr16__(SUBi8 , 00111xx),
};

const int __InstrTeller16Count = sizeof(__InstrTeller16) / sizeof(__IntructionTeller16);