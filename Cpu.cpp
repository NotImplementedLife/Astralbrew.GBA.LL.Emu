#include "Cpu.h"
#include "ARMInstruction.h"

Cpu::Cpu(Memory* memory) : memory{ memory }
{
	for (int i = 0; i < 16; i++) R[i] = 0;
	for (int i = 0; i < 7; i++) R_fiq[i] = 0;
	R_svc[0] = R_svc[1] = 0;
	R_abt[0] = R_abt[1] = 0;
	R_irq[0] = R_irq[1] = 0;
	R_und[0] = R_und[1] = 0;	
	CPSR = 0;
	SPSR_fiq = 0;
	SPSR_svc = 0;
	SPSR_abt = 0;
	SPSR_irq = 0;
	SPSR_und = 0;

	PC = 0;
}


void Cpu::do_cycle()
{
	if(instruction_state == Cpu::InstructionState::ARM)
	{ 
		if (pipeline[2] == nullptr)
		{
			pipeline[2] = pipeline[1];
			pipeline[1] = pipeline[0];
			pipeline[0] = new ARMInstruction(PC, memory->get32(PC));
			PC += 4;
		}

		if (pipeline[1])
		{
			pipeline[1]->decode();			
			std::cout << pipeline[1]->to_string() << '\n';
		}

		if (pipeline[2])
		{
			pipeline[2]->execute(this);
			delete pipeline[2];
			pipeline[2] = pipeline[1];
			pipeline[1] = pipeline[0];
			pipeline[0] = new ARMInstruction(PC, memory->get32(PC));
			PC += 4;
		}		

	}
	else
	{

	}	
}