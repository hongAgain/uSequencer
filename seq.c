#include "seq.h"
#include <stdio.h>

Sequence sequence;

trigger TriggerFault;
trigger TriggerReset;

step Main[MAX_STEPS];
step Reset[MAX_STEPS];
step Fault[MAX_STEPS];

branch ProgramStart = {0, 0, 0};

bool RunEnabled;

int main(int argc, char **argv)
{
	int i;

	//load sequence file from command line
	
	//set default state
	sequence.CurrentProgram = Main;
	sequence.CurrentState = false;
	sequence.DwellTimer = 0;
	sequence.CurrentBranch = ProgramStart;
	watchdog = 0;
	
	//clear all bits
	for (i = 0; i < INPUT_BYTE_COUNT; i++)
	{
		sequence.Bitmap[i] = 0;
	}

	RunEnabled = true;
}

void RunProgram()
{
	int i;

	GetDigitalInputs(sequence.Bitmap + INPUT_START);

	bool last_state;
	
	//check input state
	last_state = sequence.CurrentState;
	sequence.CurrentState = true;
	
	for (i = 0; i < INPUT_BYTE_COUNT; i++)
	{
		sequence.CurrentProgram[sequence.CurrentBranch.dest].masked_inputs[i] =
			(sequence.Bitmap[i] & sequence.CurrentProgram[sequence.CurrentBranch.dest].inputstate.inputs_true[i]) |
			(~sequence.Bitmap[i] & sequence.CurrentProgram[sequence.CurrentBranch.dest].inputstate.inputs_false[i]) |
			(~(sequence.CurrentProgram[sequence.CurrentBranch.dest].inputstate.inputs_true[i] ^ sequence.CurrentProgram[sequence.CurrentBranch.dest].inputstate.inputs_false[i]);
		
		if (sequence.CurrentProgram[sequence.CurrentBranch.dest].masked_inputs[i] != 0xFF)
		{
			//input trigger lost, current state is now false
			sequence.CurrentState = false;
		}
	}
	
	//reset dwell timer on state change
	if (sequence.CurrentState != last_state)
	{
		sequence.DwellTimer = 0;
	}
	
	//check if current step is complete
	if (sequence.CurrentState == true && sequence.DwellTimer >= sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest.dwell)
	{
		//set outputs, go to next step for true
		SetDigitalOutputs(sequence.Bitmap + OUTPUT_START);
		
		if (sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest.flags != 0)
		{
			if (sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest.flags & DEST_FLAG_JSR)
			{
				//not implemented
				printf("JSR feature not implemented. Fault triggered.\n");
				sequence.CurrentProgram = Fault;
				sequence.CurrentBranch = ProgramStart;
			}
			else if (sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest.flags & DEST_FLAG_FAULT)
			{
				sequence.CurrentProgram = Fault;
				sequence.CurrentBranch = ProgramStart;
			}
			else if (sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest.flags & DEST_FLAG_RESET)
			{
				sequence.CurrentProgram = Reset;
				sequence.CurrentBranch = ProgramStart;
			}
			else if (sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest.flags & DEST_FLAG_END_PROGRAM)
			{
				RunEnabled = false;
				sequence.CurrentBranch = ProgramStart;
			}
		}
		else
		{
			sequence.CurrentBranch = sequence.CurrentProgram[sequence.CurrentBranch.dest].true_dest;
		}
		
		sequence.CurrentBranch.dwell = 0;
		watchdog = 0;
	}
	else if (sequence.CurrentState == false && sequence.DwellTimer >= sequence.CurrentProgram[ProgramCounter].false_dest.dwell)
	{
		//go to next step for false
		
		if (sequence.CurrentProgram[sequence.CurrentBranch.dest].false_dest.flags != 0)
		{
			//flags set - special branch
			
			if (sequence.CurrentProgram[sequence.CurrentBranch.dest].false_dest.flags & DEST_FLAG_JSR)
			{
				//not implemented
				printf("JSR feature not implemented. Fault triggered.\n");
				sequence.CurrentProgram = Fault;
				sequence.CurrentBranch = ProgramStart;
			}
			else if (sequence.CurrentProgram[sequence.CurrentBranch.dest].false_dest.flags & DEST_FLAG_FAULT)
			{
				sequence.CurrentProgram = Fault;
				sequence.CurrentBranch = ProgramStart;
			}
			else if (sequence.CurrentProgram[sequence.CurrentBranch.dest].false_dest.flags & DEST_FLAG_RESET)
			{
				sequence.CurrentProgram = Reset;
				sequence.CurrentBranch = ProgramStart;
			}
			else if (sequence.CurrentProgram[sequence.CurrentBranch.dest].false_dest.flags & DEST_FLAG_END_PROGRAM)
			{
				RunEnabled = false;
				sequence.CurrentBranch = ProgramStart;
			}
		}
		else
		{
			sequence.CurrentBranch = sequence.CurrentProgram[sequence.CurrentBranch.dest].false_dest;
		}
		
		sequence.CurrentBranch.dwell = 0;		
		watchdog = 0;
	}
	else
	{
		//step not completed yet
		
		//update dwell timer
		if (sequence.CurrentState = last_state)
		{
			sequence.CurrentBranch.dwell++;
		}
		
		//update watchdog timer
		if (sequence.watchdog > sequence.WatchdogTimeoutTime)
		{
			sequence.FaultLocation = CurrentBranch;
			sequence.CurrentProgram = Fault;
			sequence.CurrentBranch = ProgramStart;
			sequence.watchdog = 0;
		}
		else
		{
			sequence.watchdog++;
		}
	}
	
	return;
}
