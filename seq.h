#ifndef __SEQ_H__
#define __SEQ_H__

#ifndef __cplusplus
 #define bool uint8_t
 #define false 0
 #define true (!false)
#endif

#include <stdint.h>

//step tick time length in milliseconds
//serves as timebase for all dwell timers
#define TIMEBASE 100;

#define DIGITAL_INPUT_COUNT 64
#define DIGITAL_OUTPUT_COUNT 64
#define INTERNAL_BITS 128

#define INPUT_BYTE_COUNT ((DIGITAL_INPUT_COUNT + DIGITAL_OUTPUT_COUNT + INTERNAL_BITS) / 8)
#define OUTPUT_BYTE_COUNT ((DIGITAL_OUTPUT_COUNT + DIGITAL_INTERNAL_BITS) / 8)

#define DIGITAL_INPUT_BITMAP_SIZE INPUT_COUNT / 8

#define DEST_FLAG_JSR 1
#define DEST_FLAG_FAULT 2
#define DEST_FLAG_RESET 4
#define DEST_FLAG_END_PROGRAM 8

#define FLAGS uint8_t
#define DEST uint8_t
#define DWELL uint16_t

#define MAX_STEPS 256

#define INPUTS_START 0
#define OUTPUTS_START INPUTS_START + 8
#define INTERNAL_BITS_START OUTPUTS_START + 8

//functions that will fetch the actual physical inputs and set the actual physical outputs
extern uint8_t *DigitalInputs;
extern uint8_t *DigitalOutputs;
extern uint8_t *InternalBits;

extern void GetDigitalInputs();
extern void SetDigitalOutputs();

typedef struct
{
	uint8_t inputs_true[INPUT_BYTE_COUNT];
	uint8_t inputs_false[INPUT_BYTE_COUNT];
} trigger;

typedef struct
{
	FLAGS flags;
	DEST dest;
	DWELL dwell;
} branch;

typedef struct
{
	trigger input_state;
	uint8_t outputs[OUTPUT_BYTE_COUNT];
	uint8_t outputmask[OUTPUT_BYTE_COUNT];
	branch false_dest;
	branch true_dest;
} step;

typedef struct
{
	step *CurrentProgram;
	DWELL watchdog;
	DWELL WatchdogTimeoutTime;
	branch FaultLocation;
	bool CurrentState;
	branch CurrentBranch;
	uint8_t masked_inputs[INPUT_BYTE_COUNT];
	uint8_t masked_outputs[OUTPUT_BYTE_COUNT];
	uint8_t Bitmap[INPUT_BYTE_COUNT];
} Sequence;

#endif

