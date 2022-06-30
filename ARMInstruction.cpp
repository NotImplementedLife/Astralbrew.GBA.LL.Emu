#include "ARMInstruction.h"

#include <vector>
#include <cassert>
using namespace std;

// ALU Opcodes
static const int OPCODE_AND = 0x0;
static const int OPCODE_EOR = 0x1;
static const int OPCODE_SUB = 0x2;
static const int OPCODE_RSB = 0x3;
static const int OPCODE_ADD = 0x4;
static const int OPCODE_ADC = 0x5;
static const int OPCODE_SBC = 0x6;
static const int OPCODE_RSC = 0x7;
static const int OPCODE_TST = 0x8;
static const int OPCODE_TEQ = 0x9;
static const int OPCODE_CMP = 0xA;
static const int OPCODE_CMN = 0xB;
static const int OPCODE_ORR = 0xC;
static const int OPCODE_MOV = 0xD;
static const int OPCODE_BIC = 0xE;
static const int OPCODE_MVN = 0xF;

// MUL Opcodes

static const int OPCODE_MUL   = 0x0;
static const int OPCODE_MLA   = 0x1;
static const int OPCODE_UMAAL = 0x2;
static const int OPCODE_UMULL = 0x4;
static const int OPCODE_UMLAL = 0x5;
static const int OPCODE_SMULL = 0x6;
static const int OPCODE_SMLAL = 0x7;


struct ARMControlBits
{
	u8 h, l;
	u32 val;
};

struct ARMField
{
	u8 h, l;
	const char* name;
};

struct ARMFilterData
{
	ARMInstruction::Type type = ARMInstruction::Type::Unknown;
	vector<ARMControlBits> control_bits;
	vector<ARMField> fields;		

	inline bool test_control_bits(u32 opcode, map<string, u32>& data, ARMInstruction::Type& type) const
	{		
		for (const auto& cb : control_bits)
		{
			if (!__has_bits__(opcode, cb.h, cb.l, cb.val))
			{								
				return false;
			}
		}		
		type = this->type;
		data.clear();
		for (const auto& field : fields)
		{
			assert(field.h >= field.l);
			data[field.name] = __get_bits__(opcode, field.h, field.l);
		}
		
		return true;
	}

};

static const ARMFilterData ARM_INSTR_TYPES[] =
{
	{
		ARMInstruction::Type::DataProc_Reg_ShImm, 
		{ {27,25,0b000}, {4,4,0b0} },
		{ 
			{31,28,"Cond"}, 
			{24,21,"Op"}, 
			{20,20,"S"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,7,"Shift"},
			{6,5,"Typ"},
			{3,0,"Rm"},
		}		
	},
	{
		ARMInstruction::Type::DataProc_Reg_ShReg,
		{ {27,25,0b000}, {7,7,0b0}, {4,4,0b1} },
		{
			{31,28,"Cond"},
			{24,21,"Op"},
			{20,20,"S"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,8,"Rs"},
			{6,5,"Typ"},
			{3,0,"Rm"},
		}
	},
	{
		ARMInstruction::Type::DataProc_Imm,
		{ {27,25,0b001}},
		{
			{31,28,"Cond"},
			{24,21,"Op"},
			{20,20,"S"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,8,"Shift"},
			{7,0,"Immediate"},			
		}
	},
	{
		ARMInstruction::Type::PSR_Imm,
		{ {27,23,0b00110}, {21,20,0b10}},
		{
			{31,28,"Cond"},
			{22,22,"P"},
			{19,16,"Field"},
			{15,12,"Rd"},
			{11,8,"Shift"},
			{7,0,"Immediate"},
		}
	},
	{
		ARMInstruction::Type::PSR_Reg,
		{ {27,23,0b00010}, {20,20,0b0}, {11,4,0b00000000} },
		{
			{31,28,"Cond"},
			{22,22,"P"},
			{21,21,"L"},
			{19,16,"Field"},
			{15,12,"Rd"},			
			{3,0,"Rm"},
		}
	},
	{
		ARMInstruction::Type::BX_BLX,
		{ {27,6,0b0001001011111111111100}, {4,4,0b1} },
		{
			{31,28,"Cond"},			
			{5,5,"L"},			
			{3,0,"Rn"},
		}
	},
	{
		ARMInstruction::Type::Multiply,
		{ {27,22,0b000000}, {7,4,0b1001} },
		{
			{31,28,"Cond"},
			{21,21,"A"},
			{20,20,"S"},			
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,8,"Rs"},
			{3,0,"Rm"},
		}
	},
	{
		ARMInstruction::Type::MulLong,
		{ {27,23,0b00001}, {7,4,0b1001} },
		{
			{31,28,"Cond"},
			{22,22,"U"},
			{21,21,"A"},
			{20,20,"S"},
			{19,16,"RdHi"},
			{15,12,"RdLo"},
			{11,8,"Rs"},
			{3,0,"Rm"},
		}
	},
	{
		ARMInstruction::Type::TransSwp12,
		{ {27,23,0b00010}, {21,20,0b00}, {11,4,0b00001001} },
		{
			{31,28,"Cond"},
			{22,22,"B"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{3,0,"Rm"},			
		}
	},
	{
		ARMInstruction::Type::TransReg10,
		{ {27,25,0b000}, {22,22,0b0}, {11,7,0b00001}, {4,4,0b1} },
		{
			{31,28,"Cond"},
			{24,24,"P"},
			{23,23,"U"},			
			{21,21,"W"},
			{20,20,"L"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{6,6,"S"},
			{5,5,"H"},
			{3,0,"Rm"},
		}
	},
	{
		ARMInstruction::Type::TransImm10,
		{ {27,25,0b000}, {22,22,0b1}, {7,7,0b1}, {4,4,0b1} },
		{
			{31,28,"Cond"},
			{24,24,"P"},
			{23,23,"U"},
			{21,21,"W"},
			{20,20,"L"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,8,"OffsetH"},
			{6,6,"S"},
			{5,5,"H"},
			{3,0,"OffsetL"},
		}
	},
	{
		ARMInstruction::Type::TransImm9,
		{ {27,25,0b010} },
		{
			{31,28,"Cond"},
			{24,24,"P"},
			{23,23,"U"},
			{22,22,"B"},
			{21,21,"W"},
			{20,20,"L"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,0,"Offset"},			
		}
	},
	{
		ARMInstruction::Type::TransReg9,
		{ {27,25,0b011}, {4,4,0b0} },
		{
			{31,28,"Cond"},
			{24,24,"P"},
			{23,23,"U"},
			{22,22,"B"},
			{21,21,"W"},
			{20,20,"L"},
			{19,16,"Rn"},
			{15,12,"Rd"},
			{11,7,"Shift"},
			{6,5,"Typ"},
			{3,0,"Rm"},
		}
	},
	{
		ARMInstruction::Type::Undefined,
		{ {27,25,0b011}, {4,4,0b1} },
		{
			{31,28,"Cond"},
		}
	},
	{
		ARMInstruction::Type::BlockTrans,
		{ {27,25,0b100}},
		{
			{31,28,"Cond"},
			{24,24,"P"},
			{23,23,"U"},
			{22,22,"S"},
			{21,21,"W"},
			{20,20,"L"},
			{19,16,"Rn"},
			{15,0,"RegList"},
		}
	},
	{
		ARMInstruction::Type::B_BL_BLX_Offset,
		{ {27,25,0b101}},
		{
			{31,28,"Cond"},
			{24,24,"L"},
			{23,0,"Offset"},
		}
	},
	{
		ARMInstruction::Type::CoDataTrans,
		{ {27,25,0b110}},
		{
			{31,28,"Cond"},
			{24,24,"P"},
			{23,23,"U"},
			{22,22,"N"},
			{21,21,"W"},
			{20,20,"L"},
			{19,16,"Rn"},
			{15,12,"CRd"},
			{11,8,"CP#"},
			{7,0,"Offset"},
		}
	},
	{
		ARMInstruction::Type::CoDataOp,
		{ {27,24,0b1110}, {4,4,0b0}},
		{
			{31,28,"Cond"},
			{23,20,"CPopc"},
			{19,16,"CRn"},
			{15,12,"CRd"},
			{11,8,"CP#"},
			{7,5,"CP"},
			{3,0,"CRm"},
		}
	},
	{
		ARMInstruction::Type::CoRegTrans,
		{ {27,24,0b1110}, {4,4,0b1}},
		{
			{31,28,"Cond"},
			{23,21,"CPopc"},
			{20,20,"L"},
			{19,16,"CRn"},
			{15,12,"Rd"},
			{11,8,"CP#"},
			{7,5,"CP"},
			{3,0,"CRm"},
		}
	},
	{
		ARMInstruction::Type::SWI,
		{ {27,24,0b1111}, {4,4,0b1}},
		{
			{31,28,"Cond"},
			{23,0, "Ignored"}
		}
	},
};

static const int ARM_INSTR_TYPES_SIZE = sizeof(ARM_INSTR_TYPES) / sizeof(ARMFilterData);

ARMInstruction::ARMInstruction(u32 address, u32 opcode) : opcode(opcode) 
{
	this->address = address;
}

void ARMInstruction::decode()
{
	type = ARMInstruction::Type::Unknown;
	int interpretation_cnt = 0;	
	for (const auto& filter : ARM_INSTR_TYPES)
	{
		if (filter.test_control_bits(opcode, data, type))
		{						
			interpretation_cnt += is_valid();
		}
	}		
	if (interpretation_cnt > 1)
	{
		throw std::exception("Same opcode decodes to multiple valid instructions.");
	}
}


bool ARMInstruction::is_valid() const
{
	switch (type)
	{
	case ARMInstruction::Type::Unknown:
		break;
	case ARMInstruction::Type::DataProc_Reg_ShImm: // Bit 25 = 0, Bit 4 = 0
	{
		if (!valid_alu()) return false;		

		break;
	}
	case ARMInstruction::Type::DataProc_Reg_ShReg: // Bit 25 = 0, Bit 4 = 1
	{
		if (!valid_alu()) return false;

		break;
	}
	case ARMInstruction::Type::DataProc_Imm: // Bit25 = 1
	{
		if (!valid_alu()) return false;

		break;
	}
	case ARMInstruction::Type::PSR_Imm:
		break;
	case ARMInstruction::Type::PSR_Reg:
		break;
	case ARMInstruction::Type::BX_BLX:
		break;
	case ARMInstruction::Type::Multiply:
		break;
	case ARMInstruction::Type::MulLong:
		break;
	case ARMInstruction::Type::TransSwp12:
		break;
	case ARMInstruction::Type::TransReg10:
		break;
	case ARMInstruction::Type::TransImm10:
		break;
	case ARMInstruction::Type::TransImm9:
		break;
	case ARMInstruction::Type::TransReg9:
		break;
	case ARMInstruction::Type::Undefined:
		break;
	case ARMInstruction::Type::BlockTrans:
		break;
	case ARMInstruction::Type::B_BL_BLX_Offset:
		break;
	case ARMInstruction::Type::CoDataTrans:
		break;
	case ARMInstruction::Type::CoDataOp:
		break;
	case ARMInstruction::Type::CoRegTrans:
		break;
	case ARMInstruction::Type::SWI:
		break;
	default:
		break;
	}
	return true;
}

void ARMInstruction::execute(Cpu* cpu)
{

}

#include <sstream>

std::string ARMInstruction::to_string(const InstructionFormat& format) const
{
	string result = "";

	string instr_name = "";
	string op_1 = "";
	string op_2 = "";

	if (format.show_address)
	{		
		result += string_format("%08X : ", address);
	}

	if (format.show_opcode)
	{
		result += string_format("%08X | ", opcode);
	}	

	switch (type)
	{
	case ARMInstruction::Type::Unknown:
		instr_name = "???";
		break;
	case ARMInstruction::Type::DataProc_Reg_ShImm:
	{
		instr_name = alu_name(data.at("Op"));
		break;
	}
	case ARMInstruction::Type::DataProc_Reg_ShReg:
		instr_name = alu_name(data.at("Op"));
		break;
	case ARMInstruction::Type::DataProc_Imm:
		instr_name = alu_name(data.at("Op"));
		break;
	case ARMInstruction::Type::PSR_Imm:
		instr_name = "[PSR_Imm]";
		break;
	case ARMInstruction::Type::PSR_Reg:
		instr_name = "[PSR_Reg]";
		break;
	case ARMInstruction::Type::BX_BLX:
	{		
		instr_name = data.at("L") == 0 ? "BX" : "BLX";						
		op_1 = string_format("R%i",data.at("Rn"));

		break;
	}
	case ARMInstruction::Type::Multiply:		
	{
		instr_name = mul_name(data.at("Op"));

		break;
	}
	case ARMInstruction::Type::MulLong:
	{
		instr_name = mul_name(data.at("Op"));

		break;
	}
	case ARMInstruction::Type::TransSwp12:
		instr_name = "[TransSwp12]";
		break;
	case ARMInstruction::Type::TransReg10:
		instr_name = "[TransReg10]";
		break;
	case ARMInstruction::Type::TransImm10:
		instr_name = "[TransImm10]";
		break;
	case ARMInstruction::Type::TransImm9:
		instr_name = "[TransImm9]";
		break;
	case ARMInstruction::Type::TransReg9:
		instr_name = "[TransReg9]";
		break;
	case ARMInstruction::Type::Undefined:
		instr_name = "[Undefined]";
		break;
	case ARMInstruction::Type::BlockTrans:
		instr_name = "BlockTrans";
		break;

	case ARMInstruction::Type::B_BL_BLX_Offset:
	{
		if (data.at("L") == 1)
			instr_name = "BL";
		else
			instr_name = "B";
		instr_name += condition_suffix(data.at("Cond"));

		s32 n24 = data.at("Offset") & 0x000F0000 ? (0xFFF00000 | data.at("Offset")) : data.at("Offset");
		u32 offset = address + 8 + 4 * n24;

		op_1 = "Lxx_0x" + string_format("%X",offset);
		break;
	}

	case ARMInstruction::Type::CoDataTrans:
		instr_name = "CoDataTrans";
		break;
	case ARMInstruction::Type::CoDataOp:
		instr_name = "CoDataOp";
		break;
	case ARMInstruction::Type::CoRegTrans:
		instr_name = "CoRegTrans";
		break;
	case ARMInstruction::Type::SWI:
		instr_name = "SWI";
		break;
	default:
		break;
	}

	result += instr_name;
	if (op_1 != "") result += " " + op_1;
	
	return result;
}

std::string ARMInstruction::alu_name(u8 opcode)
{
	switch (opcode)
	{
	case OPCODE_AND: return "AND";
	case OPCODE_EOR: return "EOR"; // Rd, Rn, Op2; XOR logical       // Rd = Rn XOR Op2
	case OPCODE_SUB: return "SUB"; // Rd, Rn, Op2; *; subtract          // Rd = Rn - Op2
	case OPCODE_RSB: return "RSB"; // Rd, Rn, Op2;*; subtract reversed // Rd = Op2 - Rn
	case OPCODE_ADD: return "ADD"; // Rd, Rn, Op2; *; add               // Rd = Rn + Op2
	case OPCODE_ADC: return "ADC"; // Rd, Rn, Op2; *; add with carry    // Rd = Rn + Op2 + Cy
	case OPCODE_SBC: return "SBC"; // Rd, Rn, Op2; *; sub with carry    // Rd = Rn - Op2 + Cy - 1
	case OPCODE_RSC: return "RSC"; // Rd, Rn, Op2; *; sub cy.reversed  // Rd = Op2 - Rn + Cy - 1
	case OPCODE_TST: return "TST"; // Rn, Op2; test            Void = Rn AND Op2
	case OPCODE_TEQ: return "TEQ"; // Rn, Op2; test exclusive  Void = Rn XOR Op2
	case OPCODE_CMP: return "CMP"; // Rn, Op2;*; compare         Void = Rn - Op2
	case OPCODE_CMN: return "CMN"; // Rn, Op2; *; compare neg.Void = Rn + Op2
	case OPCODE_ORR: return "ORR"; // Rd, Rn, Op2; OR logical        // Rd = Rn OR Op2
	case OPCODE_MOV: return "MOV"; // Rd, Op2; move              // Rd = Op2
	case OPCODE_BIC: return "BIC"; // Rd, Rn, Op2; bit clear         // Rd = Rn AND NOT Op2
	case OPCODE_MVN: return "MVN"; // Rd, Op2; not // Rd = NOT Op2
	}
	throw std::exception(string_format("Unknown ALU opcode : %u", opcode).c_str());
}

std::string ARMInstruction::mul_name(u8 opcode)
{
	switch (opcode)
	{
	case OPCODE_MUL: return "MUL";
	case OPCODE_MLA: return "MLA"; 
	case OPCODE_UMAAL: return "UMAAL";
	case OPCODE_UMULL: return "UMULL"; 
	case OPCODE_UMLAL: return "UMLAL"; 
	case OPCODE_SMULL: return "SMULL"; 
	case OPCODE_SMLAL: return "SMLAL"; 
	}
	throw std::exception(string_format("Unknown MUL opcode : %u",opcode).c_str());
}


bool ARMInstruction::valid_alu() const
{
	u32 alu_opcode = data.at("Op");	

	if (0x8 <= alu_opcode && alu_opcode <= 0xB)	
	{
		// S - Set Condition Codes (0=No, 1=Yes) (Must be 1 for opcode 8-B)
		u32 S = data.at("S");
		if (S == 0)
		{
			return false;
		}

		// Rd - Destination Register (R0..R15) (including PC=R15)
		// Must be 0000b(or 1111b) for CMP / CMN / TST / TEQ{ P }.
		u32 Rd = data.at("Rd");
		if (Rd != 0x0 && Rd != 0xF)
		{
			return false;
		}
	}	
	


	return true;
}