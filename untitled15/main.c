/*
    Name 1: Saptarshi Mondal
    UTEID 1: sm72999
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1,DRMUX0,
    SR1MUX1,SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX2,MARMUX1,MARMUX0,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    SR1ALUMUX1,SR1ALUMUX0,
    LD_PSR,
    LD_SSP,
    LD_USP,
    SPMUX,
    GATESP,
    LD_VECTOR,
    GATEPSR,
    GATEPCMUX,
    LD_VA,
/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
                                      (x[J3] << 3) + (x[J2] << 2) +
                                      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return((x[MARMUX2] << 2)+(x[MARMUX1] << 1) + x[MARMUX0]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); }
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetSR1ALUMUX(int *x)     { return((x[SR1ALUMUX1] << 1) + x[SR1ALUMUX0]); }
int GetLD_PSR(int *x)        { return(x[LD_PSR]); }
int GetLD_SSP(int *x)        { return(x[LD_SSP]); }
int GetLD_USP(int *x)        { return(x[LD_USP]); }
int GetSPMUX(int *x)        { return(x[SPMUX]); }
int GetGATESP(int *x)        { return(x[GATESP]); }
int GETLD_VECTOR(int *x)        { return(x[LD_VECTOR]); }
int GETGATE_PSR(int *x)     {return(x[GATEPSR]);}
int GetGATEPCMUX(int *x)        { return(x[GATEPCMUX]); }
int GETLD_VA(int *x)         { return(x[LD_VA]); }
/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
/* MODIFY: you should add here any other registers you need to implement virtual memory */
    int USP;
    int PSR;
    int iflag;
    int eflag;
    int Vector;
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

/* MODIFY: you can add more initialization code HERE */

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int iflag_global = 0;
int mar_call = 0; // sets the value to which state of the mar was being executed when the translation began
void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */
    int CONDCHECK = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
    int JCHECK = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
    int IRDCHECK = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    if(IRDCHECK == 0) {
        if (CONDCHECK == 2) {
            if (CURRENT_LATCHES.BEN == 0)
                NEXT_LATCHES.STATE_NUMBER = JCHECK;
            else
                NEXT_LATCHES.STATE_NUMBER = JCHECK + 4;
        } else if (CONDCHECK == 0) {
            if(Low16bits(CURRENT_LATCHES.eflag) == 1)
            {
                NEXT_LATCHES.eflag = 0;
                if(((Low16bits(CURRENT_LATCHES.PSR)& 0x8000 )>> 15) == 1)
                    NEXT_LATCHES.STATE_NUMBER = JCHECK + 16;
                else
                    NEXT_LATCHES.STATE_NUMBER = JCHECK;
            }
                /*  else if (Low16bits(CURRENT_LATCHES.iflag) == 1)
                  {
                      NEXT_LATCHES.iflag = 0;
                      NEXT_LATCHES.STATE_NUMBER = 55;
                  }*/
            else if((JCHECK == 33) && CYCLE_COUNT >= 300 && iflag_global == 0)
            {
                iflag_global = 1;
                NEXT_LATCHES.STATE_NUMBER=55;
            }
            else if((((Low16bits(CURRENT_LATCHES.PSR)& 0x8000 )>> 15) == 1) && ((CURRENT_LATCHES.STATE_NUMBER == 59)||(JCHECK==37)))
                NEXT_LATCHES.STATE_NUMBER = JCHECK + 16;
            else {
                NEXT_LATCHES.STATE_NUMBER = JCHECK;}
        } else if (CONDCHECK == 1) {
            int checkready = CURRENT_LATCHES.READY;
            if (checkready == 0)
                NEXT_LATCHES.STATE_NUMBER = JCHECK;
            else
                NEXT_LATCHES.STATE_NUMBER = JCHECK + 2;
        } else if (CONDCHECK == 3) {
            int checkIR = Low16bits(CURRENT_LATCHES.IR);
            checkIR = (((1 << 1) - 1) & (checkIR >> (12 - 1)));
            if (checkIR == 0)
                NEXT_LATCHES.STATE_NUMBER = JCHECK;
            else
                NEXT_LATCHES.STATE_NUMBER = JCHECK + 1;
        }
    }
    else
    {
        int a = (((1 << 4) - 1) & (CURRENT_LATCHES.IR >> (13 - 1)));
        NEXT_LATCHES.STATE_NUMBER = a;
    }
    for(int i = 0; i<50; i++)
    {
        NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER][i];
    }
    NEXT_LATCHES.READY  = 0;
    NEXT_LATCHES.BEN = 0;

}
int track = 0;

int GATEMARMUXVALUE;
int GATEPCVALUE;
int GATEALUVALUE;
int GATESHFVALUE;
int GateMDRVALUE;
int adderresult;
int inmuxvalue;
int GATESPVALUE;
int GATEVECTORVALUE;
int GATEPSRVALUE;

void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
    int checkreadwrite = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
    // int datasize = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
    int mar0 = Low16bits(CURRENT_LATCHES.MAR) & 0x0001;
    if(checkreadwrite == 1)
    {
        if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        {
            MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR & 0x00FF;
            MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
            track++;
        }
        if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        {
            if(mar0 == 0)
                MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR & 0x00FF;
            else if(mar0 == 1)
                MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0XFF00)>> 8;
            track++;
        }
    }
    else if(checkreadwrite == 0)
    {
        // int qa = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
        if(GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        {

            if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)==1)
            {
                inmuxvalue = Low16bits(MEMORY[CURRENT_LATCHES.MAR>>1][0]);
                int tem = Low16bits(MEMORY[CURRENT_LATCHES.MAR>>1][1])<<8;
                inmuxvalue = tem + inmuxvalue;
                track++;
            }
            if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
            {
                if(mar0 == 0)
                {
                    inmuxvalue = Low16bits(MEMORY[CURRENT_LATCHES.MAR>>1][0]) ;
                    if((inmuxvalue>>7)== 1)
                        inmuxvalue = inmuxvalue | 0xFF00;
                    else
                        inmuxvalue = inmuxvalue | 0x0000;
                }
                else if(mar0 == 1) {

                    inmuxvalue = Low16bits(MEMORY[CURRENT_LATCHES.MAR >> 1][1]);
                    if((inmuxvalue>>7)== 1)
                        inmuxvalue = inmuxvalue | 0xFF00;
                    else
                        inmuxvalue = inmuxvalue | 0x0000;
                }
                track++;
            }
        }
    }
    if(track == (MEM_CYCLES - 1)  )
    {
        NEXT_LATCHES.READY = 1;
    }
    else if(track == MEM_CYCLES)
    {
        track = 0;
    }
}



void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */
    int sex5,sex6,sex9,sex11,zex8, sr1output=0, srtemp1,srtemp2;
    int a = Low16bits(CURRENT_LATCHES.IR);
    sex5 = (a & 0x001F) | ((a & 0x0010) ? 0xFFE0 : 0);
    sex6 = (a & 0x003F) | ((a & 0x0020) ? 0xFFC0 : 0);
    sex9 = (a & 0x01FF) | ((a & 0x0100) ? 0xFE00 : 0);
    sex11 = (a & 0x07FF) | ((a & 0x0400) ? 0xF800 : 0);
    zex8 = ((a & 0x00FF) | 0x0000)<<1;
    srtemp1 = (((1 << 3) - 1) & (a >> (7 - 1)));
    srtemp2 = (((1 << 3) - 1) & (a >> (10 - 1)));
    if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)== 0)
        sr1output = srtemp2;
    else if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)== 1)
        sr1output = srtemp1;
    else if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
        sr1output = 6;
    int addr2output;
    if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        addr2output = 0;
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        addr2output = sex6;
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
        addr2output = sex9;
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
        addr2output = sex11;
    int leftshiftoutput;
    if(GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        leftshiftoutput = addr2output<<1;
    else
        leftshiftoutput = addr2output;
    int addr1output = 0;
    if(GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)== 0)
        addr1output = Low16bits(CURRENT_LATCHES.PC);
    else if(GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)== 1)
        addr1output = Low16bits(CURRENT_LATCHES.REGS[sr1output]);
    adderresult = addr1output + leftshiftoutput;
    int ldMarmuxcheck = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if(ldMarmuxcheck == 0)
        GATEMARMUXVALUE = zex8;
    else if(ldMarmuxcheck == 1)
        GATEMARMUXVALUE = Low16bits(adderresult);
    else if(ldMarmuxcheck == 2)
        GATEMARMUXVALUE = Low16bits(CURRENT_LATCHES.Vector);
    else if(ldMarmuxcheck == 3)
    {
        int temp1 = (Low16bits(CURRENT_LATCHES.PTBR) & 0xFF00);
        int temp2 = ((Low16bits(CURRENT_LATCHES.VA))&0xFE00);
        temp2 = temp2>>9;
        temp2 = temp2<<1;
        GATEMARMUXVALUE = temp1 + temp2;
    }
    else if(ldMarmuxcheck == 4)
    {
        int temp1 = (Low16bits(CURRENT_LATCHES.MDR)& 0x3E00);
        int temp2 = (Low16bits(CURRENT_LATCHES.VA)& 0x01FF);
        GATEMARMUXVALUE = temp1 + temp2;
    }
    if(GetGATEPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        GATEPCVALUE = Low16bits(CURRENT_LATCHES.PC);
    else if (GetGATEPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        GATEPCVALUE = Low16bits(CURRENT_LATCHES.PC) - 2;
    int SR2MUXVALUE = (((1 << 1) - 1) & (a >> (6 - 1)));
    int SR2REGVALUE = (((1 << 3) - 1) & (a >> (1 - 1)));
    int SR2VALUE;
    if(SR2MUXVALUE == 0)
        SR2VALUE = Low16bits(CURRENT_LATCHES.REGS[SR2REGVALUE]);
    else if(SR2MUXVALUE == 1)
        SR2VALUE = sex5;
    int SR1 = Low16bits(CURRENT_LATCHES.REGS[sr1output]);

    if(GetSR1ALUMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        SR1 = SR1;
    else if(GetSR1ALUMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        SR1 = SR1 + 2;
    else if(GetSR1ALUMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
        SR1 = SR1 -2;
    int GETALUKMUX = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
    if(GETALUKMUX == 0)
    {
        GATEALUVALUE = Low16bits(SR2VALUE + SR1);
        if(GATEALUVALUE > 0xFFFF)
        {
            GATEALUVALUE = Low16bits(GATEALUVALUE & 0xFFFF);
        }
    }
    else if(GETALUKMUX == 1)
    {
        GATEALUVALUE = Low16bits(SR2VALUE & SR1);
        if(GATEALUVALUE > 0xFFFF)
        {
            GATEALUVALUE = Low16bits(GATEALUVALUE & 0xFFFF);
        }
    }
    else if(GETALUKMUX == 2)
    {
        GATEALUVALUE = Low16bits( SR1 ^ SR2VALUE);
        if(GATEALUVALUE > 0xFFFF)
        {
            GATEALUVALUE = Low16bits(GATEALUVALUE & 0xFFFF);
        }
    }
    else if(GETALUKMUX == 3)
    {
        GATEALUVALUE = Low16bits( SR1);
        if(GATEALUVALUE > 0xFFFF)
        {
            GATEALUVALUE = Low16bits(GATEALUVALUE & 0xFFFF);
        }
    }
    int get_shift_direction = (((1 << 2) - 1) & (a >> (5 - 1)));
    int get_shift_value = (((1 << 4) - 1) & (a >> (1 - 1)));
    if(get_shift_direction == 0)
    {
        GATESHFVALUE = Low16bits(SR1<< get_shift_value);
        if(GATESHFVALUE > 0xFFFF)
        {
            GATESHFVALUE = Low16bits(GATESHFVALUE & 0xFFFF);
        }
    }
    else if(get_shift_direction == 1)
    {
        GATESHFVALUE = Low16bits(SR1 >> get_shift_value);
        if(GATESHFVALUE > 0xFFFF)
        {
            GATESHFVALUE = Low16bits( GATESHFVALUE & 0xFFFF);
        }
    }
    else if(get_shift_direction == 3)
    {
        int SR1signbit = (SR1 & 0x8000)>>15;
        if(SR1signbit == 1)
        {
            GATESHFVALUE = Low16bits(SR1>> get_shift_value);
            int value_to_or_by;
            if(get_shift_value == 0)
                value_to_or_by = 0x0000;
            else if (get_shift_value == 1)
                value_to_or_by = 0x8000;
            else if (get_shift_value == 2)
                value_to_or_by = 0xC000;
            else if (get_shift_value == 3)
                value_to_or_by = 0xE000;
            else if (get_shift_value == 4)
                value_to_or_by = 0xF000;
            else if (get_shift_value == 5)
                value_to_or_by = 0xF800;
            else if (get_shift_value == 6)
                value_to_or_by = 0xFC00;
            else if (get_shift_value == 7)
                value_to_or_by = 0xFE00;
            else if (get_shift_value == 8)
                value_to_or_by = 0xFF00;
            else if (get_shift_value == 9)
                value_to_or_by = 0xFF80;
            else if (get_shift_value == 10)
                value_to_or_by = 0xFFC0;
            else if (get_shift_value == 11)
                value_to_or_by = 0xFFE0;
            else if (get_shift_value == 12)
                value_to_or_by = 0xFFF0;
            else if (get_shift_value == 13)
                value_to_or_by = 0xFFF8;
            else if (get_shift_value == 14)
                value_to_or_by = 0xFFFC;
            else if (get_shift_value == 15)
                value_to_or_by = 0xFFFE;
            GATESHFVALUE = Low16bits(GATESHFVALUE | value_to_or_by);
        }
        else if(SR1signbit == 0)
            GATESHFVALUE = Low16bits(SR1>> get_shift_value);
        if(GATESHFVALUE > 0xFFFF)
        {
            GATESHFVALUE = Low16bits(GATESHFVALUE & 0xFFFF);
        }
    }
    if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
    {
        GateMDRVALUE = Low16bits(CURRENT_LATCHES.MDR) ;
    }
    else if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
    {
        int checkmar = Low16bits(CURRENT_LATCHES.MAR) & 0x0001;
        if(checkmar == 0)
        {
            int ruv= Low16bits(CURRENT_LATCHES.MDR)  & 0x00FF;
            int i = (ruv & 0x0080)>> 7;
            if(i == 1)
                GateMDRVALUE = Low16bits(ruv | 0xFF00);
            else
                GateMDRVALUE = Low16bits(ruv);
        }
        else if(checkmar == 1)
        {
            int ruv = Low16bits(CURRENT_LATCHES.MDR) & 0x00FF;
            int i = (ruv & 0x0080)>> 7;
            if(i == 1)
                GateMDRVALUE = Low16bits( ruv | 0xFF00);
            else
                GateMDRVALUE = Low16bits(ruv);
        }
    }
    if(GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        GATESPVALUE = Low16bits(CURRENT_LATCHES.USP);
    else if(GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        GATESPVALUE = Low16bits(CURRENT_LATCHES.SSP);
    GATEPSRVALUE = CURRENT_LATCHES.PSR ;
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */
    if(GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        BUS = GATEALUVALUE;
    else if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)== 1)
        BUS = GATEMARMUXVALUE;
    else if(GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)== 1)
        BUS = GATEPCVALUE;
    else if(GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)== 1)
        BUS = GATESHFVALUE;
    else if(GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)== 1)
        BUS = GateMDRVALUE;
    else if(GetGATESP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        BUS = GATESPVALUE;
    else if(GETGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        BUS = GATEPSRVALUE;
    else
        BUS = 0;
}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */
    if(track == 0) {
        if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            NEXT_LATCHES.IR = Low16bits(BUS);
        if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            NEXT_LATCHES.MAR = Low16bits(BUS);
        if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            int checkdr = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
            int reg= 0;
            if (checkdr == 0) {
                reg = Low16bits(CURRENT_LATCHES.IR) & 0x0E00;
                reg = reg >> 9;
            } else if (checkdr == 1)
                reg = 7;
            else if(checkdr == 2)
                reg = 6;
            NEXT_LATCHES.REGS[reg] = Low16bits(BUS);
        }
        if(GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            NEXT_LATCHES.SSP = Low16bits(GATEALUVALUE);
        if(GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            NEXT_LATCHES.USP = Low16bits(GATEALUVALUE);
        if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            int checkpcmux = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
            if (checkpcmux == 0)
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC) + 2;
            else if (checkpcmux == 1)
                NEXT_LATCHES.PC = Low16bits(BUS);
            else if (checkpcmux == 2)
                NEXT_LATCHES.PC = Low16bits(adderresult);
        }
        if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            int ir11, ir10, ir9;
            ir11 = Low16bits(CURRENT_LATCHES.IR) & 0x0800;
            ir11 = ir11 >> 11;
            ir10 = Low16bits(CURRENT_LATCHES.IR) & 0x0400;
            ir10 = ir10 >> 10;
            ir9 = Low16bits(CURRENT_LATCHES.IR) & 0x0200;
            ir9 = ir9 >> 9;
            NEXT_LATCHES.BEN = (ir11 & Low16bits(CURRENT_LATCHES.N)) + (ir10 & Low16bits(CURRENT_LATCHES.Z)) + (ir9 &Low16bits(CURRENT_LATCHES.P));
        }
        if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            int a = Low16bits(BUS);
            if(a == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else{
                a = a >> 15;
                if(a == 1)
                {
                    NEXT_LATCHES.N = 1;
                    NEXT_LATCHES.Z = 0;
                    NEXT_LATCHES.P = 0;
                }
                else if ( a== 0)
                {
                    NEXT_LATCHES.N = 0;
                    NEXT_LATCHES.Z = 0;
                    NEXT_LATCHES.P = 1;
                }
            }

        }
        int checkmio;
        if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            checkmio = Low16bits(BUS);
        else if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            int checkmar = Low16bits(CURRENT_LATCHES.MAR) & 0x0001;
            if (checkmar == 0)
                checkmio = Low16bits(BUS) & 0x00FF;
            else if (checkmar == 1)
                checkmio = ((Low16bits(BUS) & 0x00FF)<<8);
        }
        int miooutput;
        if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {
            miooutput = Low16bits(checkmio);
        } else if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            miooutput = Low16bits(inmuxvalue);
        if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            NEXT_LATCHES.MDR = Low16bits(miooutput);
        if(GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        {
            if(GetJ(CURRENT_LATCHES.MICROINSTRUCTION) == 37)
                NEXT_LATCHES.PSR = Low16bits( CURRENT_LATCHES.PSR & 32767);
            else
            {
                NEXT_LATCHES.PSR = Low16bits(BUS);
                NEXT_LATCHES.N = (NEXT_LATCHES.PSR &0x0004)>>2;
                NEXT_LATCHES.Z = (NEXT_LATCHES.PSR&0x0002)>>1;
                NEXT_LATCHES.P = (NEXT_LATCHES.PSR&0x0001);
            }
        }
        else {
            NEXT_LATCHES.PSR = CURRENT_LATCHES.PSR;
            if(CURRENT_LATCHES.N == 1)
                NEXT_LATCHES.PSR = NEXT_LATCHES.PSR | 0x0004;
            else if(CURRENT_LATCHES.N == 0)
                NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0xFFFB;
            if(CURRENT_LATCHES.Z == 1)
                NEXT_LATCHES.PSR = NEXT_LATCHES.PSR |0x0002;
            else if(CURRENT_LATCHES.Z == 0)
                NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0xFFFD;
            if(CURRENT_LATCHES.P == 1)
                NEXT_LATCHES.PSR = NEXT_LATCHES.PSR | 0x0001;
            else if(CURRENT_LATCHES.P == 0)
                NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0xFFFE;
        }
        int JCHECK = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
     if((NEXT_LATCHES.MAR %2 != 0) && JCHECK == 26 && CURRENT_LATCHES.STATE_NUMBER!=2 && CURRENT_LATCHES.STATE_NUMBER!=3)
     {
         NEXT_LATCHES.eflag = 1;
         NEXT_LATCHES.STATE_NUMBER = 55;
     }
     else if((((NEXT_LATCHES.PSR &0x8000)>> 15) == 1)&&(((NEXT_LATCHES.MDR&0x0008)>>3) == 0) && JCHECK == 44)
     {
         if( mar_call != 15) {

             NEXT_LATCHES.eflag = 1;
             NEXT_LATCHES.STATE_NUMBER = 55;
         }
     }
     else if(JCHECK == 56 && (((CURRENT_LATCHES.MDR&0x0004)>>2) == 0))
     {
         NEXT_LATCHES.eflag = 1;
         NEXT_LATCHES.STATE_NUMBER = 55;
     }
     else if(((((NEXT_LATCHES.IR & 0xF000 )>> 12)== 10) || (((NEXT_LATCHES.IR & 0xF000 )>> 12)== 11) )  && NEXT_LATCHES.STATE_NUMBER == 32)
     {
         NEXT_LATCHES.eflag =  1;
         NEXT_LATCHES.STATE_NUMBER = 55;
     }
     else
     {
         NEXT_LATCHES.eflag = 0;
     }
      /*  if(NEXT_LATCHES.MAR >= 0X0000 && NEXT_LATCHES.MAR <= 0x2FFF   && ((CURRENT_LATCHES.PSR&0x8000)>>15 != 0) &&
           GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1 && NEXT_LATCHES.STATE_NUMBER!=28)
        {
            NEXT_LATCHES.eflag = 1;
            NEXT_LATCHES.STATE_NUMBER = 55;
        }
        else if(NEXT_LATCHES.MAR %2 != 0 &&(NEXT_LATCHES.STATE_NUMBER == 33 ||NEXT_LATCHES.STATE_NUMBER == 28  || NEXT_LATCHES.STATE_NUMBER== 25 || NEXT_LATCHES.STATE_NUMBER == 23 || NEXT_LATCHES.STATE_NUMBER == 36 || NEXT_LATCHES.STATE_NUMBER == 40 || NEXT_LATCHES.STATE_NUMBER == 48 || NEXT_LATCHES.STATE_NUMBER == 52 || NEXT_LATCHES.STATE_NUMBER == 45 ))
        {
            NEXT_LATCHES.eflag = 1;
            NEXT_LATCHES.STATE_NUMBER = 55;
        }
        else if(((((NEXT_LATCHES.IR & 0xF000 )>> 12)== 10) || (((NEXT_LATCHES.IR & 0xF000 )>> 12)== 11) )  && NEXT_LATCHES.STATE_NUMBER == 32)
        {
            NEXT_LATCHES.eflag =  1;
            NEXT_LATCHES.STATE_NUMBER = 55;
        }
        else
        {
            NEXT_LATCHES.eflag = 0;
        }*/
        if(GETLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        {
             if(CURRENT_LATCHES.MAR %2 != 0)
                NEXT_LATCHES.Vector = 0x0206;
              else if((((CURRENT_LATCHES.PSR &0x8000)>> 15) == 1)&&(((CURRENT_LATCHES.MDR&0x0008)>>3) == 0))
             {
                  NEXT_LATCHES.Vector = 0x0208;
             }
            else if (((CURRENT_LATCHES.MDR&0x0004)>>2) == 0)
             {
                NEXT_LATCHES.Vector = 0x0204;
             }
            else if((((NEXT_LATCHES.IR & 0xF000 )>> 12)== 10) ||  (((NEXT_LATCHES.IR & 0xF000 )>> 12)== 11) )
             {
                NEXT_LATCHES.Vector = 0x020A;
             }

            else
                NEXT_LATCHES.Vector = 0x0202;
        }
        if(NEXT_LATCHES.STATE_NUMBER == 55)
        {
            for(int i = 0; i<50; i++)
            {
                NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER][i];
            }
        }
        if(CYCLE_COUNT == 0)
        {
            NEXT_LATCHES.PSR = 0x8000 | (NEXT_LATCHES.N<<2)|(NEXT_LATCHES.Z<<1)|(NEXT_LATCHES.P);
        }
        if(GETLD_VA(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        {
            NEXT_LATCHES.VA = CURRENT_LATCHES.MAR;
        }
        if(JCHECK == 57)
        {
            NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR | 0x0001;
        }
        if(JCHECK == 58)
        {
            if(mar_call == 7 || mar_call == 3 )
            {
                NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR |0x0002;
            }
            else if( mar_call == 18 || mar_call == 19 || mar_call == 2 || mar_call == 6)
            {
                NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR & 0xFFFD;
            }
        }
        if(JCHECK == 26)
        {
            mar_call = CURRENT_LATCHES.STATE_NUMBER;
        }
        if(CURRENT_LATCHES.STATE_NUMBER == 60)
        {
            if(mar_call == 18)
                NEXT_LATCHES.STATE_NUMBER = 41;
            else if(mar_call == 2)
                NEXT_LATCHES.STATE_NUMBER = 29;
            else if(mar_call == 6)
                NEXT_LATCHES.STATE_NUMBER = 25;
            else if(mar_call == 7)
                NEXT_LATCHES.STATE_NUMBER = 23;
            else if(mar_call == 3)
                NEXT_LATCHES.STATE_NUMBER = 24;
            for(int i = 0; i<50; i++)
            {
                NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER][i];
            }
        }
    }
    if(track != 0)
        BUS = 0;
}