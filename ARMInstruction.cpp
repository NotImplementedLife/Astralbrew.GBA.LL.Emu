#include "ARMInstruction.h"

#include <vector>
#include <cassert>
using namespace std;

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
				type = ARMInstruction::Type::Unknown;
				return false;
			}
		}
		std::cout << "HERE\n";
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
		ARMInstruction::Type::DataProc_Reg,
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
	for (const auto& filter : ARM_INSTR_TYPES)
	{
		if (filter.test_control_bits(opcode, data, type))
		{
			break;
		}
	}		
}

void ARMInstruction::execute(Cpu* cpu)
{

}

#include <sstream>

std::string ARMInstruction::to_string(const InstructionFormat& format)
{
	std::string result = "";

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
		result += "???";
		break;
	case ARMInstruction::Type::DataProc_Reg:
		result += "DataProd_Reg";
		break;
	case ARMInstruction::Type::DataProc_Reg_ShReg:
		result += "DataProd_ShReg";
		break;
	case ARMInstruction::Type::DataProc_Imm:
		result += "DataProd_Imm";
		break;
	case ARMInstruction::Type::PSR_Imm:
		result += "PSR_Imm";
		break;
	case ARMInstruction::Type::PSR_Reg:
		result += "PSR_Reg";
		break;
	case ARMInstruction::Type::BX_BLX:
		result += "BX_BLX";
		break;
	case ARMInstruction::Type::Multiply:
		result += "Multiply";
		break;
	case ARMInstruction::Type::MulLong:
		result += "MulLong";
		break;
	case ARMInstruction::Type::TransSwp12:
		result += "TransSwp12";
		break;
	case ARMInstruction::Type::TransReg10:
		result += "TransReg10";
		break;
	case ARMInstruction::Type::TransImm10:
		result += "TransImm10";
		break;
	case ARMInstruction::Type::TransImm9:
		result += "TransImm9";
		break;
	case ARMInstruction::Type::TransReg9:
		result += "TransReg9";
		break;
	case ARMInstruction::Type::Undefined:
		result += "Undefined";
		break;
	case ARMInstruction::Type::BlockTrans:
		result += "BlockTrans";
		break;
	case ARMInstruction::Type::B_BL_BLX_Offset:
		result += "B_BL_BLX_Offset";
		break;
	case ARMInstruction::Type::CoDataTrans:
		result += "CoDataTrans";
		break;
	case ARMInstruction::Type::CoDataOp:
		result += "CoDataOp";
		break;
	case ARMInstruction::Type::CoRegTrans:
		result += "CoRegTrans";
		break;
	case ARMInstruction::Type::SWI:
		result += "SWI";
		break;
	default:
		break;
	}

	
	return result;
}
