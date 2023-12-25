/*
    Name 1: Logan Liberty
    Name 2: Saptarshi Mondal
    UTEID 1: LGL624
    UTEID 2: SM72999
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

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
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
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
/* Purpose   : Simulate the LC-3b until HALTed                 */
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

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
        program_base = word >> 1;
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

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
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

    // Error Checking
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

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

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

// Returns the desired number of isolated bits from an input value
int bitReturn (int number, int returnSize, int position)
{
    return (((1 << returnSize) - 1) & (number >> position));
}

void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */

    // Store full 16 bit instruction into 'c'
    //printf("PC = 0x%x\n", CURRENT_LATCHES.PC);

    int a = MEMORY[CURRENT_LATCHES.PC >> 1][0];
    int b = MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8;
    int c = a + b;

    // 'd' holds 4 bit value of opcode
    //int d =  (((1 << 4) - 1) & (c >> (13 - 1)));
    int d = bitReturn(c, 4, 12);

    // ADD opcode
    if(d == 1)
    {
        // gets number 5 value -> e
        int e =  (((1 << 1) - 1) & (c >> (6 - 1)));
        // register add
        if(e == 0)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            int h = (((1 << 3) - 1) & (c >> (1 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[h]);
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[h] + CURRENT_LATCHES.REGS[g];
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);
        }
            // offset add
        else if(e == 1)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            short h = (((1 << 5) - 1) & (c >> (1 - 1)));
            short h1 = (((1 << 1) - 1) & (c >> (5 - 1)));
            if(h1 == 1)
            {
                h = h + 0xFFE0;
            }
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[g] + h;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if(NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if(NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if(NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);
        }
    }

        // AND opcode
    else if(d == 5)
    {
        int e =  (((1 << 1) - 1) & (c >> (6 - 1)));
        if(e == 0)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            int h = (((1 << 3) - 1) & (c >> (1 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[h]);
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[h] & CURRENT_LATCHES.REGS[g];
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
        else if(e == 1)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            short h = (((1 << 5) - 1) & (c >> (1 - 1)));
            short h1 = (((1 << 1) - 1) & (c >> (5 - 1)));
            if(h1 == 1)
            {
                h = h + 0xFFE0;
            }
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[g] & h;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if(NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if(NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if(NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);
        }
    }

        // NOT, XOR opcode
    else if(d == 9)
    {
        int e =  (((1 << 1) - 1) & (c >> (6 - 1)));
        int e1 = (((1 << 6) - 1) & (c >> (1 - 1)));
        if(e==0 && e1 != 63)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            int h = (((1 << 3) - 1) & (c >> (1 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[h]);
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[h] ^ CURRENT_LATCHES.REGS[g];
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
        else if(e== 1 && e1!=63)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            short h = (((1 << 5) - 1) & (c >> (1 - 1)));
            short h1 = (((1 << 1) - 1) & (c >> (5 - 1)));
            if(h1 == 1)
            {
                h = h + 0xFFE0;
            }
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[g] ^ h;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
        else if(e1==63)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = ~CURRENT_LATCHES.REGS[g] ;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
    }

        // LSHF, RSHFL, RSHFA opcode
    else if(d == 13 )
    {
        int e =  (((1 << 2) - 1) & (c >> (5 - 1)));
        if(e== 0)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            int h =  (((1 << 4) - 1) & (c >> (1 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[g] << h;
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
        else if(e==1)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            int h =  (((1 << 4) - 1) & (c >> (1 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[g]);
            NEXT_LATCHES.REGS[f] = (int)((unsigned int)CURRENT_LATCHES.REGS[g] >> h);
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
        else if(e==3)
        {
            int f = (((1 << 3) - 1) & (c >> (10 - 1)));
            int g = (((1 << 3) - 1) & (c >> (7 - 1)));
            int h =  (((1 << 4) - 1) & (c >> (1 - 1)));
            Low16bits(CURRENT_LATCHES.REGS[g]);
            int sigBit = bitReturn(CURRENT_LATCHES.REGS[g], 1, 15);
            NEXT_LATCHES.REGS[f] = CURRENT_LATCHES.REGS[g] >> h;
            if (sigBit == 1)
            {
                NEXT_LATCHES.REGS[f] |= 0x8000;
            }
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
            if((short)NEXT_LATCHES.REGS[f] > 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
            else if((short)NEXT_LATCHES.REGS[f] == 0)
            {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else if((short)NEXT_LATCHES.REGS[f] < 0)
            {
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 0;
            }

            NEXT_LATCHES.REGS[f] = Low16bits(NEXT_LATCHES.REGS[f]);

        }
    }

        // JMP, RET opcode
    else if(d == 12)
    {
        int e =  (((1 << 3) - 1) & (c >> (7 - 1)));
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[e];
    }

        // BR opcode
    else if(d == 0)
    {
        // Increment program counter
        NEXT_LATCHES.PC += 2;

        // Separate the N Z P bits
        int Nbit = (((1 << 1) - 1) & (c >> (12 - 1)));
        int Zbit = (((1 << 1) - 1) & (c >> (11 - 1)));
        int Pbit = (((1 << 1) - 1) & (c >> (10 - 1)));

        // Isolate the offset
        signed short BRoffset = (((1 << 9) - 1) & (c >> 0));

        // Convert offset into 2s complement
        if ( (((1 << 1) - 1) & (BRoffset >> 8)) == 1)
        {
            BRoffset += 0xFE00;
        }

        // Left shift the offset by one
        BRoffset = (BRoffset << 1);

        // Compare branch condition with current NZP bits
        // Check if N bits match
        if (Nbit == 1)
        {
            if (Nbit == CURRENT_LATCHES.N)
            {
                // Add the offset to the program counter
                NEXT_LATCHES.PC += BRoffset;
            }
        }
        // Check if Z bits match
        if (Zbit == 1)
        {
            if (Zbit == CURRENT_LATCHES.Z)
            {
                // Add the offset to the program counter
                NEXT_LATCHES.PC += BRoffset;
            }
        }
        // Check if P bits match
        if (Pbit == 1)
        {
            if (Pbit == CURRENT_LATCHES.P)
            {
                // Add the offset to the program counter
                NEXT_LATCHES.PC += BRoffset;
            }
        }

    }

        // JSR, JSRR opcode
    else if(d == 4)
    {
        // Check bit 11 for JSR or JSRR operation
        int bitCheck = bitReturn(c, 1, 11);

        // Increment program counter
        NEXT_LATCHES.PC += 2;

        // JSR
        if (bitCheck == 1)
        {
            // Isolate the offset
            signed short JSRoffset = bitReturn(c, 11, 0);

            // Convert offset into 2s complement
            if (bitReturn(JSRoffset, 1, 11) == 1)
            {
                JSRoffset += 0xF800;
            }

            // Left shift JSRoffset by one
            JSRoffset = (JSRoffset << 1);

            // Store next address into R7
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;

            // Increment program counter by offset
            NEXT_LATCHES.PC += JSRoffset;
        }
            // JSRR
        else if(bitCheck == 0)
        {
            // Get baseR
            int JSRRbaseR = bitReturn(c, 3, 6);

            // Store address of baseR into R7
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;

            // Increment program counter by offset in baseR
            NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[JSRRbaseR];
        }

    }

        // LDB opcode
    else if(d == 2)
    {
        // Get destination register number
        int DR = bitReturn(c, 3, 9);

        // Get base register number
        int baseR = bitReturn(c, 3, 6);

        // Get value of offset
        signed short LDBoffset = bitReturn(c, 6, 0);

        // Convert offset into 16 bit 2s complement
        if (bitReturn(LDBoffset, 1, 5) == 1)
        {
            LDBoffset += 0xFFE0;
        }

        // Value at offset + base register memory address
        signed short loadVal = MEMORY[(LDBoffset + CURRENT_LATCHES.REGS[baseR]) >> 1][0];

        // Sign extending loadVal to 16 bits
        if (bitReturn(loadVal, 1, 7) == 1)
        {
            loadVal += 0xFF00;
        }

        // Store loadVal into destination register
        NEXT_LATCHES.REGS[DR] = Low16bits(loadVal);

        // Update condition codes based on value in DR
        if (loadVal > 0)
        {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        else if (loadVal == 0)
        {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if (loadVal < 0)
        {
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }

        // Increment program counter
        NEXT_LATCHES.PC += 2;

    }

        // LDW opcode
    else if(d == 6)
    {
        // Get destination register number
        int DR = bitReturn(c, 3, 9);

        // Get base register number
        int baseR = bitReturn(c, 3, 6);

        // Get value of offset
        signed short LDWoffset = bitReturn(c, 6, 0);

        // Convert offset into 16 bit 2s complement
        if (bitReturn(LDWoffset, 1, 5) == 1)
        {
            LDWoffset += 0xFFE0;
        }

        // Left shift offset by one
        LDWoffset = (LDWoffset << 1);

        // Value at offset + base register memory address
        int loadVal = ((MEMORY[(LDWoffset + CURRENT_LATCHES.REGS[baseR]) >> 1][1] << 8) + (MEMORY[(LDWoffset + CURRENT_LATCHES.REGS[baseR]) >> 1][0]));

        // Store loadVal into destination register
        NEXT_LATCHES.REGS[DR] = Low16bits(loadVal);

        // Update condition codes based on value in DR
        if (loadVal > 0)
        {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        else if (loadVal == 0)
        {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if (loadVal < 0)
        {
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }

        // Increment program counter
        NEXT_LATCHES.PC += 2;
    }

        // LEA opcode
    else if(d == 14)
    {

        // Increment program counter
        NEXT_LATCHES.PC += 2;

        // Get destination register number
        int DR = bitReturn(c, 3, 9);

        // Get value of offset
        signed short LEAoffset = bitReturn(c, 9, 0);

        // Convert offset into 16 bit 2s complement
        if (bitReturn(LEAoffset, 1, 8) == 1)
        {
            LEAoffset += 0xFE00;
        }

        // Left shift offset by one
        LEAoffset = (LEAoffset << 1);

        // Set value of destination register to offset + next PC
        NEXT_LATCHES.REGS[DR] = (LEAoffset + NEXT_LATCHES.PC);

    }

        // STB opcode
    else if(d == 3)
    {
        // Get source register number
        int SR = bitReturn(c, 3, 9);

        // Get base register number
        int baseR = bitReturn(c, 3, 6);

        // Get value of offset
        signed short STBoffset = bitReturn(c, 6, 0);

        // Convert offset into 16 bit 2s complement
        if (bitReturn(STBoffset, 1, 5) == 1)
        {
            STBoffset += 0xFFE0;
        }

        // Low 8 bits of SR are put into storeVal
        int storeVal = (CURRENT_LATCHES.REGS[SR] & 0x00FF);

        // Low 8 bits of SR are stored at offset + baseR
        MEMORY[(CURRENT_LATCHES.REGS[baseR] + STBoffset) >> 1][0] = storeVal;

        // Increment program counter
        NEXT_LATCHES.PC += 2;

    }

        // STW opcode
    else if(d == 7)
    {
        // Get source register number
        int SR = bitReturn(c, 3, 9);

        // Get base register number
        int baseR = bitReturn(c, 3, 6);

        // Get value of offset
        signed short STWoffset = bitReturn(c, 6, 0);

        // Convert offset into 16 bit 2s complement
        if (bitReturn(STWoffset, 1, 5) == 1)
        {
            STWoffset += 0xFFE0;
        }

        // Left shit the offset by one
        STWoffset = (STWoffset << 1);

        // Bits 0-7 of SR are put into storeValLow
        // Bits 8-15 of SR are put into storeValHigh
        int storeValLow = (CURRENT_LATCHES.REGS[SR] & 0x00FF);
        int storeValHigh = (CURRENT_LATCHES.REGS[SR] & 0xFF00)>>8;

        // Value at storeVal is stored at baseR + offset
        MEMORY[(CURRENT_LATCHES.REGS[baseR] + STWoffset) >> 1][0] = storeValLow;
        MEMORY[(CURRENT_LATCHES.REGS[baseR] + STWoffset) >> 1][1] = storeValHigh;

        // Increment program counter
        NEXT_LATCHES.PC += 2;
    }

        // TRAP opcode
    else if(d == 15)
    {
        // TRAP should always store a 0 into next PC

        // Increment program counter
        NEXT_LATCHES.PC += 2;

        // R7 loaded with next program counter
        NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;

        // Isolate trap vector
        unsigned short trapVect = bitReturn(c, 8, 0);

        // Left shift trap vector by one bit
        trapVect = (trapVect << 1);

        // Load value at trapVect address in memory
        trapVect = MEMORY[trapVect][0] + (MEMORY[trapVect][1] << 8);

        // Store value of trapVect into program counter
        NEXT_LATCHES.PC = trapVect;
    }
}