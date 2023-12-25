/*
    Name 1: Saptarshi Mondal
    UTEID 1: sm72999
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator - Lab 6                                   */
/*                                                             */
/*   EE 460N -- Spring 2013                                    */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/
void FETCH_stage();
void DE_stage();
void AGEX_stage();
void MEM_stage();
void SR_stage();
/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define TRUE  1
#define FALSE 0

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
/* control signals from the control store */
enum CS_BITS {
    SR1_NEEDED,
    SR2_NEEDED,
    DRMUX,

    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    LSHF1,
    ADDRESSMUX,
    SR2MUX,
    ALUK1, ALUK0,
    ALU_RESULTMUX,

    BR_OP,
    UNCOND_OP,
    TRAP_OP,
    BR_STALL,

    DCACHE_EN,
    DCACHE_RW,
    DATA_SIZE,

    DR_VALUEMUX1, DR_VALUEMUX0,
    LD_REG,
    LD_CC,
    NUM_CONTROL_STORE_BITS

} CS_BITS;


enum AGEX_CS_BITS {

    AGEX_ADDR1MUX,
    AGEX_ADDR2MUX1, AGEX_ADDR2MUX0,
    AGEX_LSHF1,
    AGEX_ADDRESSMUX,
    AGEX_SR2MUX,
    AGEX_ALUK1, AGEX_ALUK0,
    AGEX_ALU_RESULTMUX,

    AGEX_BR_OP,
    AGEX_UNCOND_OP,
    AGEX_TRAP_OP,
    AGEX_BR_STALL,
    AGEX_DCACHE_EN,
    AGEX_DCACHE_RW,
    AGEX_DATA_SIZE,

    AGEX_DR_VALUEMUX1, AGEX_DR_VALUEMUX0,
    AGEX_LD_REG,
    AGEX_LD_CC,
    NUM_AGEX_CS_BITS
} AGEX_CS_BITS;

enum MEM_CS_BITS {
    MEM_BR_OP,
    MEM_UNCOND_OP,
    MEM_TRAP_OP,
    MEM_BR_STALL,
    MEM_DCACHE_EN,
    MEM_DCACHE_RW,
    MEM_DATA_SIZE,

    MEM_DR_VALUEMUX1, MEM_DR_VALUEMUX0,
    MEM_LD_REG,
    MEM_LD_CC,
    NUM_MEM_CS_BITS
} MEM_CS_BITS;

enum SR_CS_BITS {
    SR_DR_VALUEMUX1, SR_DR_VALUEMUX0,
    SR_LD_REG,
    SR_LD_CC,
    NUM_SR_CS_BITS
} SR_CS_BITS;


/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int Get_SR1_NEEDED(int *x)     { return (x[SR1_NEEDED]); }
int Get_SR2_NEEDED(int *x)     { return (x[SR2_NEEDED]); }
int Get_DRMUX(int *x)          { return (x[DRMUX]);}
int Get_DE_BR_OP(int *x)       { return (x[BR_OP]); }
int Get_ADDR1MUX(int *x)       { return (x[AGEX_ADDR1MUX]); }
int Get_ADDR2MUX(int *x)       { return ((x[AGEX_ADDR2MUX1] << 1) + x[AGEX_ADDR2MUX0]); }
int Get_LSHF1(int *x)          { return (x[AGEX_LSHF1]); }
int Get_ADDRESSMUX(int *x)     { return (x[AGEX_ADDRESSMUX]); }
int Get_SR2MUX(int *x)          { return (x[AGEX_SR2MUX]); }
int Get_ALUK(int *x)           { return ((x[AGEX_ALUK1] << 1) + x[AGEX_ALUK0]); }
int Get_ALU_RESULTMUX(int *x)  { return (x[AGEX_ALU_RESULTMUX]); }
int Get_BR_OP(int *x)          { return (x[MEM_BR_OP]); }
int Get_UNCOND_OP(int *x)      { return (x[MEM_UNCOND_OP]); }
int Get_TRAP_OP(int *x)        { return (x[MEM_TRAP_OP]); }
int Get_DCACHE_EN(int *x)      { return (x[MEM_DCACHE_EN]); }
int Get_DCACHE_RW(int *x)      { return (x[MEM_DCACHE_RW]); }
int Get_DATA_SIZE(int *x)      { return (x[MEM_DATA_SIZE]); }
int Get_DR_VALUEMUX1(int *x)   { return ((x[SR_DR_VALUEMUX1] << 1 ) + x[SR_DR_VALUEMUX0]); }
int Get_AGEX_LD_REG(int *x)    { return (x[AGEX_LD_REG]); }
int Get_AGEX_LD_CC(int *x)     { return (x[AGEX_LD_CC]); }
int Get_MEM_LD_REG(int *x)     { return (x[MEM_LD_REG]); }
int Get_MEM_LD_CC(int *x)      { return (x[MEM_LD_CC]); }
int Get_SR_LD_REG(int *x)      { return (x[SR_LD_REG]); }
int Get_SR_LD_CC(int *x)       { return (x[SR_LD_CC]); }
int Get_DE_BR_STALL(int *x)    { return (x[BR_STALL]); }
int Get_AGEX_BR_STALL(int *x)  { return (x[AGEX_BR_STALL]); }
int Get_MEM_BR_STALL(int *x)   { return (x[MEM_BR_STALL]); }



/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][NUM_CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/
/* The LC-3b register file.                                      */
/***************************************************************/
#define LC3b_REGS 8
int REGS[LC3b_REGS];
/***************************************************************/
/* architectural state */
/***************************************************************/
int  PC,  	/* program counter */
N,		/* n condition bit */
Z = 1,	/* z condition bit */
P;		/* p condition bit */
/***************************************************************/
/* LC-3b State info.                                             */
/***************************************************************/

typedef struct PipeState_Entry_Struct{

    /* DE latches */
    int DE_NPC,
            DE_IR,
            DE_V,
    /* AGEX lateches */
    AGEX_NPC,
            AGEX_SR1,
            AGEX_SR2,
            AGEX_CC,
            AGEX_IR,
            AGEX_DRID,
            AGEX_V,
            AGEX_CS[NUM_AGEX_CS_BITS],
    /* MEM latches */
    MEM_NPC,
            MEM_ALU_RESULT,
            MEM_ADDRESS,
            MEM_CC,
            MEM_IR,
            MEM_DRID,
            MEM_V,
            MEM_CS[NUM_MEM_CS_BITS],
    /* SR latches */
    SR_NPC,
            SR_DATA,
            SR_ALU_RESULT,
            SR_ADDRESS,
            SR_IR,
            SR_DRID,
            SR_V,
            SR_CS[NUM_SR_CS_BITS];

} PipeState_Entry;

/* data structure for latch */
PipeState_Entry PS, NEW_PS;

/* simulator signal */
int RUN_BIT;

/* Internal stall signals */
int   dep_stall,
        v_de_br_stall,
        v_agex_br_stall,
        v_mem_br_stall,
        mem_stall,
        icache_r;
int v_mem_ld_cc,v_mem_ld_reg;
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
    printf("rdump            -  dump the architectural state    \n");
    printf("idump            -  dump the internal state         \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

void print_CS(int *CS, int num)
{
    int ii ;
    for ( ii = 0 ; ii < num; ii++) {
        printf("%d",CS[ii]);
    }
    printf("\n");
}
/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {
    NEW_PS = PS;
    SR_stage();
    MEM_stage();
    AGEX_stage();
    DE_stage();
    FETCH_stage();
    PS = NEW_PS;
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
        if (PC == 0x0000) {
            cycle();
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
    if ((RUN_BIT == FALSE) || (PC == 0x0000)) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }
    printf("Simulating...\n\n");
    /* initialization */
    while (PC != 0x0000)
        cycle();
    cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a region of memory to the output file.     */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%04x..0x%04x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%04x..0x%04x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current architectural state  to the       */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent architectural state :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count : %d\n", CYCLE_COUNT);
    printf("PC          : 0x%04x\n", PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    printf("Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        printf("%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent architectural state :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC          : 0x%04x\n", PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : idump                                           */
/*                                                             */
/* Purpose   : Dump current internal state to the              */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void idump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent architectural state :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count     : %d\n", CYCLE_COUNT);
    printf("PC              : 0x%04x\n", PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    printf("Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        printf("%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    printf("\n");

    printf("------------- Stall Signals -------------\n");
    printf("ICACHE_R        :  %d\n", icache_r);
    printf("DEP_STALL       :  %d\n", dep_stall);
    printf("V_DE_BR_STALL   :  %d\n", v_de_br_stall);
    printf("V_AGEX_BR_STALL :  %d\n", v_agex_br_stall);
    printf("MEM_STALL       :  %d\n", mem_stall);
    printf("V_MEM_BR_STALL  :  %d\n", v_mem_br_stall);
    printf("\n");

    printf("------------- DE   Latches --------------\n");
    printf("DE_NPC          :  0x%04x\n", PS.DE_NPC );
    printf("DE_IR           :  0x%04x\n", PS.DE_IR );
    printf("DE_V            :  %d\n", PS.DE_V);
    printf("\n");

    printf("------------- AGEX Latches --------------\n");
    printf("AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
    printf("AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
    printf("AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
    printf("AGEX_CC         :  %d\n", PS.AGEX_CC );
    printf("AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
    printf("AGEX_DRID       :  %d\n", PS.AGEX_DRID);
    printf("AGEX_CS         :  ");
    for ( k = 0 ; k < NUM_AGEX_CS_BITS; k++) {
        printf("%d",PS.AGEX_CS[k]);
    }
    printf("\n");
    printf("AGEX_V          :  %d\n", PS.AGEX_V);
    printf("\n");

    printf("------------- MEM  Latches --------------\n");
    printf("MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
    printf("MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
    printf("MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS );
    printf("MEM_CC          :  %d\n", PS.MEM_CC );
    printf("MEM_IR          :  0x%04x\n", PS.MEM_IR );
    printf("MEM_DRID        :  %d\n", PS.MEM_DRID);
    printf("MEM_CS          :  ");
    for ( k = 0 ; k < NUM_MEM_CS_BITS; k++) {
        printf("%d",PS.MEM_CS[k]);
    }
    printf("\n");
    printf("MEM_V           :  %d\n", PS.MEM_V);
    printf("\n");

    printf("------------- SR   Latches --------------\n");
    printf("SR_NPC          :  0x%04x\n", PS.SR_NPC );
    printf("SR_DATA         :  0x%04x\n", PS.SR_DATA );
    printf("SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
    printf("SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
    printf("SR_IR           :  0x%04x\n", PS.SR_IR );
    printf("SR_DRID         :  %d\n", PS.SR_DRID);
    printf("SR_CS           :  ");
    for ( k = 0 ; k < NUM_SR_CS_BITS; k++) {
        printf("%d",PS.SR_CS[k]);
    }
    printf("\n");
    printf("SR_V            :  %d\n", PS.SR_V);

    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file,"\nCurrent architectural state :\n");
    fprintf(dumpsim_file,"-------------------------------------\n");
    fprintf(dumpsim_file,"Cycle Count     : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file,"PC              : 0x%04x\n", PC);
    fprintf(dumpsim_file,"CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    fprintf(dumpsim_file,"Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        fprintf(dumpsim_file,"%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- Stall Signals -------------\n");
    fprintf(dumpsim_file,"ICACHE_R        :  %d\n", icache_r);
    fprintf(dumpsim_file,"DEP_STALL       :  %d\n", dep_stall);
    fprintf(dumpsim_file,"V_DE_BR_STALL   :  %d\n", v_de_br_stall);
    fprintf(dumpsim_file,"V_AGEX_BR_STALL :  %d\n", v_agex_br_stall);
    fprintf(dumpsim_file,"MEM_STALL       :  %d\n", mem_stall);
    fprintf(dumpsim_file,"V_MEM_BR_STALL  :  %d\n", v_mem_br_stall);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- DE   Latches --------------\n");
    fprintf(dumpsim_file,"DE_NPC          :  0x%04x\n", PS.DE_NPC );
    fprintf(dumpsim_file,"DE_IR           :  0x%04x\n", PS.DE_IR );
    fprintf(dumpsim_file,"DE_V            :  %d\n", PS.DE_V);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- AGEX Latches --------------\n");
    fprintf(dumpsim_file,"AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
    fprintf(dumpsim_file,"AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
    fprintf(dumpsim_file,"AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
    fprintf(dumpsim_file,"AGEX_CC         :  %d\n", PS.AGEX_CC );
    fprintf(dumpsim_file,"AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
    fprintf(dumpsim_file,"AGEX_DRID       :  %d\n", PS.AGEX_DRID);
    fprintf(dumpsim_file,"AGEX_CS         :  ");
    for ( k = 0 ; k < NUM_AGEX_CS_BITS; k++) {
        fprintf(dumpsim_file,"%d",PS.AGEX_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"AGEX_V          :  %d\n", PS.AGEX_V);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- MEM  Latches --------------\n");
    fprintf(dumpsim_file,"MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
    fprintf(dumpsim_file,"MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
    fprintf(dumpsim_file,"MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS );
    fprintf(dumpsim_file,"MEM_CC          :  %d\n", PS.MEM_CC );
    fprintf(dumpsim_file,"MEM_IR          :  0x%04x\n", PS.MEM_IR );
    fprintf(dumpsim_file,"MEM_DRID        :  %d\n", PS.MEM_DRID);
    fprintf(dumpsim_file,"MEM_CS          :  ");
    for ( k = 0 ; k < NUM_MEM_CS_BITS; k++) {
        fprintf(dumpsim_file,"%d",PS.MEM_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"MEM_V           :  %d\n", PS.MEM_V);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- SR   Latches --------------\n");
    fprintf(dumpsim_file,"SR_NPC          :  0x%04x\n", PS.SR_NPC );
    fprintf(dumpsim_file,"SR_DATA         :  0x%04x\n",PS.SR_DATA );
    fprintf(dumpsim_file,"SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
    fprintf(dumpsim_file,"SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
    fprintf(dumpsim_file,"SR_IR           :  0x%04x\n", PS.SR_IR );
    fprintf(dumpsim_file,"SR_DRID         :  %d\n", PS.SR_DRID);
    fprintf(dumpsim_file,"SR_CS           :  ");
    for ( k = 0 ; k < NUM_SR_CS_BITS; k++) {
        fprintf(dumpsim_file, "%d",PS.SR_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"SR_V            :  %d\n", PS.SR_V);

    fprintf(dumpsim_file,"\n");
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

        case 'I':
        case 'i':
            idump(dumpsim_file);
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

        for (j = 0; j < NUM_CONTROL_STORE_BITS; j++) {
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


/***************************************************************/
/*                                                             */
/* Procedure : init_state                                      */
/*                                                             */
/* Purpose   : Zero out all latches and registers              */
/*                                                             */
/***************************************************************/
void init_state() {

    memset(&PS, 0 ,sizeof(PipeState_Entry));
    memset(&NEW_PS, 0 , sizeof(PipeState_Entry));

    dep_stall       = 0;
    v_de_br_stall   = 0;
    v_agex_br_stall = 0;
    v_mem_br_stall  = 0;
    mem_stall       = 0;
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1 ;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
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
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (PC == 0) PC  = program_base << 1 ;
    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();

    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    init_state();

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* dcache_access                                               */
/*                                                             */
/***************************************************************/
void dcache_access(int dcache_addr, int *read_word, int write_word, int *dcache_r,
                   int mem_w0, int mem_w1) {

    int addr = dcache_addr >> 1 ;
    int random = CYCLE_COUNT % 9;

    if (!random) {
        *dcache_r = 0;
        *read_word = 0xfeed ;
    }
    else {
        *dcache_r = 1;

        *read_word = (MEMORY[addr][1] << 8) | (MEMORY[addr][0] & 0x00FF);
        if(mem_w0) MEMORY[addr][0] = write_word & 0x00FF;
        if(mem_w1) MEMORY[addr][1] = (write_word & 0xFF00) >> 8;
    }
}
/***************************************************************/
/*                                                             */
/* icache_access                                               */
/*                                                             */
/***************************************************************/
void icache_access(int icache_addr, int *read_word, int *icache_r) {

    int addr = icache_addr >> 1 ;
    int random = CYCLE_COUNT % 13;

    if (!random) {
        *icache_r = 0;
        *read_word = 0xfeed;
    }
    else {
        *icache_r = 1;
        *read_word = MEMORY[addr][1] << 8 | MEMORY[addr][0];
    }
}
/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
        printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);
}


/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   RUN_BIT
   REGS
   MEMORY

   PC
   N
   Z
   P

   dep_stall
   v_de_br_stall
   v_agex_br_stall
   v_mem_br_stall
   mem_stall
   icache_r

   PS
   NEW_PS


   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.


   Begin your code here 	  			       */
/***************************************************************/
#define COPY_AGEX_CS_START 3
#define COPY_MEM_CS_START 9
#define COPY_SR_CS_START  7
int MEM_PCMUX;
int Target_PC;
int Trap_PC;
int AGEX_DRID;
int v_agex_ld_cc;
int v_agex_ld_reg;
int v_agex_br_stall;
int v_de_br_stall;
//int LD_PC;
int MEM_DRID;
int cc;
/* Signals generated by SR stage and needed by previous stages in the
   pipeline are declared below. */
int sr_reg_data,
        sr_n, sr_z, sr_p,
        v_sr_ld_cc,
        v_sr_ld_reg,
        sr_reg_id;


/************************* SR_stage() *************************/
void SR_stage() {

    /* You are given the code for SR_stage to get you started. Look at
       the figure for SR stage to see how this code is implemented. */

    switch (Get_DR_VALUEMUX1(PS.SR_CS))
    {
        case 0:
            sr_reg_data = PS.SR_ADDRESS ;
            break;
        case 1:
            sr_reg_data = PS.SR_DATA ;
            break;
        case 2:
            sr_reg_data = PS.SR_NPC ;
            break;
        case 3:
            sr_reg_data = PS.SR_ALU_RESULT ;
            break;
    }

    sr_reg_id = PS.SR_DRID;
    v_sr_ld_reg = Get_SR_LD_REG(PS.SR_CS) & PS.SR_V;
    v_sr_ld_cc = Get_SR_LD_CC(PS.SR_CS) & PS.SR_V ;

    /* CC LOGIC  */
    sr_n = ((sr_reg_data & 0x8000) ? 1 : 0);
    sr_z = ((sr_reg_data & 0xFFFF) ? 0 : 1);
    sr_p = 0;
    if ((!sr_n) && (!sr_z))
        sr_p = 1;

}


/************************* MEM_stage() *************************/
void MEM_stage() {

    int ii,jj = 0;
    /* your code for MEM stage goes here */
    if(CYCLE_COUNT == 0)
    {
        cc = Low16bits(sr_n)<<2;
        cc = Low16bits(cc)|(Low16bits(sr_z)<<1);
        cc = Low16bits(cc)| Low16bits(sr_p);
    }
    Target_PC = Low16bits(PS.MEM_ADDRESS);
    int least_sig = Low16bits(PS.MEM_ADDRESS)& 0x0001;
    int writeenable0,writeenable1;
    int word_to_write = PS.MEM_ALU_RESULT;
    if(Get_DCACHE_RW(PS.MEM_CS) == 0)
    {
        writeenable0 = 0;
        writeenable1 = 0;
    }
    else
    {
        if((Get_DATA_SIZE(PS.MEM_CS) == 1) && (least_sig%2==0))
        {
            writeenable0 = 1;
            writeenable1 = 1;
        }
        else if((Get_DATA_SIZE(PS.MEM_CS) == 0) && (least_sig%2==0))
        {
            writeenable0 = 1;
            writeenable1 = 0;
        }
        else if((Get_DATA_SIZE(PS.MEM_CS) == 0) && (least_sig%2==1))
        {
            writeenable0 = 0;
            writeenable1 = 1;
            int temp = (word_to_write & 0x00FF)<<8;
            word_to_write = temp |(word_to_write&0x00FF);

        }
    }
    int v_dcache_en;
    if((Get_DCACHE_EN(PS.MEM_CS) == 1) && (PS.MEM_V == 1))
        v_dcache_en = 1;
    else
        v_dcache_en = 0;
   /* if(Get_DATA_SIZE(PS.MEM_CS) == 1)
        data = PS.MEM_ALU_RESULT;
    else
    {
        if(least_sig%2 == 0)
        {
            data = PS.MEM_ALU_RESULT & 0x00FF;
        }
        else
        {
            data = PS.MEM_ALU_RESULT & 0xFF00;
            data = data>>8;
        }
    }*/
    int data_from_dcache;
    int dcache_r;
 if(v_dcache_en == 1)
 {

     dcache_access(PS.MEM_ADDRESS,&data_from_dcache,word_to_write,&dcache_r,writeenable0,writeenable1);
 }
 else
 {
     data_from_dcache = 0x0000;
     dcache_r = 0;
 }
if((dcache_r == 0)&&(v_dcache_en == 1))
    mem_stall = 1;
else
    mem_stall = 0;
int load_into_SR_DATA;
if((Get_DATA_SIZE(PS.MEM_CS) == 1) && (least_sig%2==0))
{
    load_into_SR_DATA = Low16bits(data_from_dcache);
}
else if ((Get_DATA_SIZE(PS.MEM_CS) == 0) && (least_sig%2==0))
{
    load_into_SR_DATA = Low16bits(data_from_dcache) & 0x00FF;
    if((load_into_SR_DATA>>7) == 1)
        load_into_SR_DATA = load_into_SR_DATA | 0xFF00;
    else
        load_into_SR_DATA = load_into_SR_DATA | 0x0000;
}
else if ((Get_DATA_SIZE(PS.MEM_CS) == 0) && (least_sig%2==1))
{
    load_into_SR_DATA = (Low16bits(data_from_dcache) & 0xFF00)>>8;
    if((load_into_SR_DATA>>7) == 1)
        load_into_SR_DATA = load_into_SR_DATA | 0xFF00;
    else
        load_into_SR_DATA = load_into_SR_DATA | 0x0000;
}
Trap_PC = Low16bits(load_into_SR_DATA);
int MEM_IR_11_9;
 MEM_IR_11_9 = (Low16bits(PS.MEM_IR) & 0x0E00)>>9;
 if(PS.MEM_V ==1)
 {
    if(Get_UNCOND_OP(PS.MEM_CS) == 1)
        MEM_PCMUX = 1;
    else if(Get_TRAP_OP(PS.MEM_CS) == 1)
        MEM_PCMUX = 2;
    else if(Get_BR_OP(PS.MEM_CS) == 1)
    {
        if(PS.MEM_CC == MEM_IR_11_9)
            MEM_PCMUX = 1;
        else
            MEM_PCMUX = 0;
    }
 }
 else
 {
     MEM_PCMUX = 0;
 }
 MEM_DRID = Low16bits(PS.MEM_DRID);
 if(PS.MEM_V == 1)
 {
     if(Get_MEM_LD_CC(PS.MEM_CS) == 1)
         v_mem_ld_cc = 1;
     else
         v_mem_ld_cc = 0;
     if(Get_MEM_BR_STALL(PS.MEM_CS) == 1)
         v_mem_br_stall = 1;
     else
         v_mem_br_stall = 0;
     if(Get_MEM_LD_REG(PS.MEM_CS) == 1)
         v_mem_ld_reg = 1;
     else
         v_mem_ld_reg = 0;
 }
 else
 {
     v_mem_br_stall = 0;
     v_mem_ld_cc = 0;
     v_mem_ld_reg = 0;
 }
    /* The code below propagates the control signals from MEM.CS latch
       to SR.CS latch. You still need to latch other values into the
       other SR latches. */
    for (ii=COPY_SR_CS_START; ii < NUM_MEM_CS_BITS; ii++) {
        NEW_PS.SR_CS [jj++] = PS.MEM_CS [ii];
    }
NEW_PS.SR_ADDRESS = Low16bits(PS.MEM_ADDRESS);
    NEW_PS.SR_DATA = Low16bits(load_into_SR_DATA);
    NEW_PS.SR_NPC= Low16bits(PS.MEM_NPC);
    NEW_PS.SR_ALU_RESULT = Low16bits(PS.MEM_ALU_RESULT);
    NEW_PS.SR_IR = Low16bits(PS.MEM_IR);
    NEW_PS.SR_DRID = Low16bits(PS.MEM_DRID);
    NEW_PS.SR_V = Low16bits(PS.MEM_V) &!mem_stall;
}


/************************* AGEX_stage() *************************/
void AGEX_stage() {

    int ii, jj = 0;
    int LD_MEM; /* You need to write code to compute the value of LD.MEM
		 signal */
    /* your code for AGEX stage goes here */
  int output_of_addr1mux;
  if(Get_ADDR1MUX(PS.AGEX_CS) == 0)
      output_of_addr1mux = Low16bits(PS.AGEX_NPC);
  else
      output_of_addr1mux = Low16bits(PS.AGEX_SR1);
  int output_of_addr2mux;
  if(Get_ADDR2MUX(PS.AGEX_CS) == 0)
      output_of_addr2mux = 0;
  else if(Get_ADDR2MUX(PS.AGEX_CS)==1)
  {
      output_of_addr2mux = (Low16bits(PS.AGEX_IR) & 0x003F);
      int temp = (Low16bits(output_of_addr2mux) & 0x0020);
      if(temp == 0)
      {
          output_of_addr2mux = Low16bits(output_of_addr2mux);

      }
      else
      {
          output_of_addr2mux = Low16bits(output_of_addr2mux) | 0xFFC0;
      }
  }
  else if(Get_ADDR2MUX(PS.AGEX_CS)==2)
  {
      output_of_addr2mux = (Low16bits(PS.AGEX_IR) & 0x01FF);
      int temp = (Low16bits(output_of_addr2mux) & 0x0100);
      if(temp == 0)
      {
          output_of_addr2mux = Low16bits(output_of_addr2mux);

      }
      else
      {
          output_of_addr2mux = Low16bits(output_of_addr2mux) | 0xFE00;
      }
  }
  else if(Get_ADDR2MUX(PS.AGEX_CS)==3)
  {
      output_of_addr2mux = (Low16bits(PS.AGEX_IR) & 0x07FF);
      int temp = (Low16bits(output_of_addr2mux) & 0x0400);
      if(temp == 0)
      {
          output_of_addr2mux = Low16bits(output_of_addr2mux);

      }
      else
      {
          output_of_addr2mux = Low16bits(output_of_addr2mux) | 0xF800;
      }
  }
if(Get_LSHF1(PS.AGEX_CS) == 1)
    output_of_addr2mux = Low16bits(output_of_addr2mux)<<1;
int adderesult;
adderesult = Low16bits(output_of_addr1mux)+ Low16bits(output_of_addr2mux);
int zex8;
zex8 = ((Low16bits(PS.AGEX_IR)&0x00FF)|0x0000)<<1;
int addressmuxresult;
if(Get_ADDRESSMUX(PS.AGEX_CS)==1)
    addressmuxresult = Low16bits(adderesult);
else
    addressmuxresult = Low16bits(zex8);
int a = Low16bits(PS.AGEX_IR) & 0x003F;
int shift_value;
int get_shift_direction = (a>>4);
int get_shift_value = a & 0x000F;
    if(get_shift_direction == 0 || get_shift_direction == 2)
    {
        shift_value = Low16bits(PS.AGEX_SR1<< get_shift_value);
        if(shift_value > 0xFFFF)
        {
            shift_value = Low16bits(shift_value & 0xFFFF);
        }
    }
    else if(get_shift_direction == 1)
    {
        shift_value = Low16bits(PS.AGEX_SR1 >> get_shift_value);
        if(shift_value > 0xFFFF)
        {
            shift_value = Low16bits( shift_value & 0xFFFF);
        }
    }
    else if(get_shift_direction == 3)
    {
        int SR1signbit = (Low16bits(PS.AGEX_SR1) & 0x8000)>>15;
        if(SR1signbit == 1)
        {
            shift_value = Low16bits(PS.AGEX_SR1>> get_shift_value);
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
            shift_value = Low16bits(shift_value | value_to_or_by);
        }
        else if(SR1signbit == 0)
            shift_value = Low16bits(PS.AGEX_SR1>> get_shift_value);
        if( shift_value> 0xFFFF)
        {
            shift_value = Low16bits(shift_value & 0xFFFF);
        }
    }
 int SR2MUXOUTPUT;
    if(Get_SR2MUX(PS.AGEX_CS) == 0)
        SR2MUXOUTPUT = Low16bits(PS.AGEX_SR2);
    else
    {
        int temp = (PS.AGEX_IR& 0x0010);
        if(temp == 0)
        {
            SR2MUXOUTPUT = Low16bits(PS.AGEX_IR)& 0x001F;
        }
        else
        {
            SR2MUXOUTPUT = (Low16bits(PS.AGEX_IR)&0x001F)|0xFFE0;
        }
    }
int ALUOUTPUT;
    if(Get_ALUK(PS.AGEX_CS) == 0)
    {
        ALUOUTPUT = Low16bits(PS.AGEX_SR1) +Low16bits(SR2MUXOUTPUT);
    }
    else if(Get_ALUK(PS.AGEX_CS)==1)
    {
        ALUOUTPUT = Low16bits(PS.AGEX_SR1) & Low16bits(SR2MUXOUTPUT);
    }
    else if(Get_ALUK(PS.AGEX_CS) ==2)
    {
        ALUOUTPUT = Low16bits(PS.AGEX_SR1) ^ Low16bits(SR2MUXOUTPUT);
    }
    else
    {
        ALUOUTPUT = Low16bits(SR2MUXOUTPUT);
    }
    int resultALURESULTMUX;
    if(Get_ALU_RESULTMUX(PS.AGEX_CS)==0)
        resultALURESULTMUX = Low16bits(shift_value);
    else
        resultALURESULTMUX = Low16bits(ALUOUTPUT);
AGEX_DRID = Low16bits(PS.AGEX_DRID);
if(PS.AGEX_V == 1) {
    if (Get_AGEX_LD_CC(PS.AGEX_CS) == 1) {
        v_agex_ld_cc = 1;
    }
    else
    {
        v_agex_ld_cc = 0;
    }
    if (Get_AGEX_BR_STALL(PS.AGEX_CS) == 1) {
        v_agex_br_stall = 1;
    }
    else
    {
        v_agex_br_stall = 0;
    }
    if (Get_AGEX_LD_REG(PS.AGEX_CS) == 1) {
        v_agex_ld_reg = 1;
    }
    else
    {
        v_agex_ld_reg = 0;
    }
}
else
{
    v_agex_br_stall = 0;
    v_agex_ld_reg = 0;
    v_agex_ld_cc = 0;
}
if(mem_stall == 0)
    LD_MEM = 1;
else
    LD_MEM = 0;
    if (LD_MEM) {
        /* Your code for latching into MEM latches goes here */
    NEW_PS.MEM_V = Low16bits(PS.AGEX_V);
    NEW_PS.MEM_DRID = Low16bits(PS.AGEX_DRID);
    NEW_PS.MEM_IR = Low16bits(PS.AGEX_IR);
    NEW_PS.MEM_ALU_RESULT = Low16bits(resultALURESULTMUX);
    NEW_PS.MEM_CC = Low16bits(PS.AGEX_CC);
    NEW_PS.MEM_NPC  = Low16bits(PS.AGEX_NPC);
        /* The code below propagates the control signals from AGEX.CS latch
           to MEM.CS latch. */
        for (ii = COPY_MEM_CS_START; ii < NUM_AGEX_CS_BITS; ii++) {
            NEW_PS.MEM_CS [jj++] = PS.AGEX_CS [ii];
        }
        NEW_PS.MEM_ADDRESS = Low16bits(addressmuxresult);
    }
}



/************************* DE_stage() *************************/
void DE_stage() {

    int CONTROL_STORE_ADDRESS;  /* You need to implement the logic to
			         set the value of this variable. Look
			         at the figure for DE stage */
    int ii, jj = 0;
    int LD_AGEX; /* You need to write code to compute the value of
		  LD.AGEX signal */
   if(mem_stall == 1)
       LD_AGEX = 0;
   else
       LD_AGEX = 1;
   int temp1 = (Low16bits(PS.DE_IR)&0xF800)>>10;
   int temp2 = (Low16bits(PS.DE_IR)&0X0020)>>5;
   CONTROL_STORE_ADDRESS = Low16bits(temp1|temp2);
   int value_from_control_store[NUM_CONTROL_STORE_BITS]  ;
   for(int i = 0;i<NUM_CONTROL_STORE_BITS; i++ )
    /* your code for DE stage goes here */
   {
       value_from_control_store[i] =CONTROL_STORE[CONTROL_STORE_ADDRESS][i];
   }

if(PS.DE_V == 1)
{
   if(Get_DE_BR_STALL(value_from_control_store) == 1)
       v_de_br_stall = 1;
   else
       v_de_br_stall = 0;
}
else
    v_de_br_stall = 0;

int SR1 = (Low16bits(PS.DE_IR) & 0x01C0)>>6;
int SR2_temp1 = (Low16bits(PS.DE_IR) & 0x0E00)>>9;
int SR2_temp2 = (Low16bits(PS.DE_IR) & 0x0007);
int SR2IDMUX = (Low16bits(PS.DE_IR) & 0x2000)>>13;
int SR2IDMUXRESULT;
if(SR2IDMUX == 0)
    SR2IDMUXRESULT = Low16bits(SR2_temp2);
else
    SR2IDMUXRESULT = Low16bits(SR2_temp1);
if(PS.DE_V == 1)
{
if(Get_SR1_NEEDED(value_from_control_store) == 1)
{
    if((SR1 == AGEX_DRID)&& (v_agex_ld_reg == 1))
    {
        dep_stall = 1;
    }
    else if((SR1 == MEM_DRID) && (v_mem_ld_reg == 1))
    {
        dep_stall = 1;
    }
    else if((SR1 == sr_reg_id)&&(v_sr_ld_reg == 1))
    {
        dep_stall = 1;
    }
    else
    {
        dep_stall = 0;
    }
}
else if(Get_SR2_NEEDED(value_from_control_store)==1)
{
    if((SR2IDMUXRESULT == AGEX_DRID)&& (v_agex_ld_reg == 1))
    {
        dep_stall = 1;
    }
    else if((SR2IDMUXRESULT == MEM_DRID) && (v_mem_ld_reg == 1))
    {
        dep_stall = 1;
    }
    else if((SR2IDMUXRESULT = sr_reg_id)&&(v_sr_ld_reg == 1))
    {
        dep_stall = 1;
    }
    else
    {
        dep_stall = 0;
    }
}
else if(Get_DE_BR_OP(value_from_control_store)==1)
{
    if((v_agex_ld_cc == 1)||(v_mem_ld_cc==1)||(v_sr_ld_cc==1))
        dep_stall = 1;
    else
        dep_stall = 0;
}
}
else
{
    dep_stall = 0;
}


int DR_MUXRESULT;
int value = Get_DRMUX(value_from_control_store);
if(Get_DRMUX(value_from_control_store) == 0)
{
    DR_MUXRESULT = SR2_temp1;
}
else
{
    DR_MUXRESULT = 7;
}
    int SR1VALUE = Low16bits(REGS[SR1]);
    int SR2VALUE = Low16bits(REGS[SR2IDMUXRESULT]);
if(v_sr_ld_reg == 1)
{
    REGS[sr_reg_id] = Low16bits(sr_reg_data);
}
/*if((sr_reg_id == SR1)&&(v_sr_ld_reg == 1))
{
    REGS[SR1] = sr_reg_data;
}
else if((sr_reg_id == SR2IDMUXRESULT) &&(v_sr_ld_reg==1))
    {
        REGS[SR2IDMUXRESULT] = sr_reg_data;
    }*/


if(mem_stall == 1)
    LD_AGEX = 0;
else
    LD_AGEX = 1;
    if (LD_AGEX) {
        /* Your code for latching into AGEX latches goes here */
    NEW_PS.AGEX_NPC = Low16bits(PS.DE_NPC);
        /* The code below propagates the control signals from the CONTROL
           STORE to the AGEX.CS latch. */
        for (ii = COPY_AGEX_CS_START; ii< NUM_CONTROL_STORE_BITS; ii++) {
            NEW_PS.AGEX_CS[jj++] = CONTROL_STORE[CONTROL_STORE_ADDRESS][ii];
        }
        NEW_PS.AGEX_IR = Low16bits(PS.DE_IR);
        NEW_PS.AGEX_SR1 = Low16bits(SR1VALUE);
        NEW_PS.AGEX_SR2 = Low16bits(SR2VALUE);

            NEW_PS.AGEX_CC = Low16bits(cc);
        NEW_PS.AGEX_DRID = Low16bits(DR_MUXRESULT);
        NEW_PS.AGEX_V = PS.DE_V && !dep_stall;
    }
    if(v_sr_ld_cc == 1) {
        int cc1 = 0;
        cc1 = Low16bits(sr_n) << 2;
        cc1 = Low16bits(cc1) | (Low16bits(sr_z) << 1);
        cc1 = Low16bits(cc1) | Low16bits(sr_p);
        cc = Low16bits(cc1);
        N = Low16bits(sr_n);
        Z= Low16bits(sr_z);
        P = Low16bits(sr_p);
    }

}



/************************* FETCH_stage() *************************/
void FETCH_stage() {

    /* your code for FETCH stage goes here */
    int instruction;
    int value_into_de_pc;
    value_into_de_pc = Low16bits(PC) +2;
    icache_access(PC,&instruction,&icache_r);
    int into_pc;
    if(MEM_PCMUX == 0)
        into_pc = Low16bits(PC)+2;
    else if(MEM_PCMUX == 1)
        into_pc= Low16bits(Target_PC);
    else if(MEM_PCMUX == 2)
        into_pc = Low16bits(Trap_PC);
    int LD_PC;
if(icache_r == 0 )
   LD_PC = 0;
else if((v_de_br_stall == 1 || v_agex_br_stall == 1 || v_mem_br_stall == 1
|| dep_stall ==1 || mem_stall ==1) )
    LD_PC = 0;
else
    LD_PC = 1;
if(MEM_PCMUX !=0)
    LD_PC = 1;

int LD_DE;
if(dep_stall == 1 || mem_stall == 1)
    LD_DE = 0;
else
    LD_DE = 1;
int new_V;
    if((icache_r == 0)&&(v_de_br_stall!=1 || v_agex_br_stall!=1 || v_mem_br_stall!=1 || mem_stall!=1 || dep_stall!=1  ))
        new_V = 0;
    else if((icache_r == 1) && (v_de_br_stall==1 || v_agex_br_stall==1 || v_mem_br_stall==1 || mem_stall==1 || dep_stall==1  ))
        new_V = 0;
    else if(icache_r == 0)
        new_V = 0;
    else
        new_V = 1;
if(LD_DE)
{
    NEW_PS.DE_NPC = Low16bits(value_into_de_pc);
    NEW_PS.DE_IR = Low16bits(instruction);
   NEW_PS.DE_V = Low16bits(new_V);
}
    if(LD_PC == 1)
        PC = Low16bits(into_pc);
}
