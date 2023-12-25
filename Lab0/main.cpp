/*
    Name 1: Logan Liberty
    Name 2: Saptarshi Mondal
    UTEID 1: LGL624
    UTEID 2: SM72999
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

// Error code 1: undefined label
// Error code 2: invalid opcode
// Error code 3: invalid constant
// Error code 4: other error

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
const char* a = ".orig";
const char* b = ".end";
int orig = -1;
int endflag = 0;
int origflag = 0;
int count;

#define MAX_LINE_LENGTH 255
enum
{
    DONE,
    OK,
    EMPTY_LINE
};

typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];    // Question for the reader: Why do we need to add 1? Answer: Sentinel value at the end of array
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int trac=0;

FILE* infile = NULL;
FILE* outfile = NULL;

// toNum function from Lab 1 Functions
int toNum(char * pStr)
{
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )				/* decimal */
    {
        pStr++;
        if( *pStr == '-' )				/* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("toNum Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )	/* hex     */
    {
        pStr++;
        if( *pStr == '-' )				/* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))
            {
                printf("toNum Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        printf( "toNum Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}

// toNum function from Lab 1 Functions
int toNumInt(char * pStr)
{
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )				/* decimal */
    {
        pStr++;
        if( *pStr == '-' )				/* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("toNum Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )	/* hex     */
    {
        pStr++;
        if( *pStr == '-' )				/* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))
            {
                printf("toNum Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        printf( "toNumInt Error: invalid label, %s\n", orig_pStr);
        exit(1);
    }
}

void callorig(char** pArg1)
{
    char *a1 = *pArg1;
    int size = strlen(a1); //Total size of string
    if(a1[size-1]=='\r')
        a1[size-1] = '\0';
    orig = toNum(a1);
    if(orig%2!=0 || orig <0 || orig > 0xFFFF)
    {
        printf( "callorig Error: invalid constant, %d\n", orig);
        exit(3);
    }
    count = orig-1;
}

// If valid opcode, return 1
// If invalid opcode, return -1
int isOpcode(char* opcode)
{
    if ((strcmp(opcode, "add") == 0)
        || (strcmp(opcode, "and") == 0)
        || (strcmp(opcode, "br") == 0)
        || (strcmp(opcode, "brn") == 0)
        || (strcmp(opcode, "brz") == 0)
        || (strcmp(opcode, "brp") == 0)
        || (strcmp(opcode, "brzp") == 0)
        || (strcmp(opcode, "brnp") == 0)
        || (strcmp(opcode, "brnz") == 0)
        || (strcmp(opcode, "brnzp") == 0)
        || (strcmp(opcode, "halt") == 0)
        || (strcmp(opcode, "jmp") == 0)
        || (strcmp(opcode, "jsr") == 0)
        || (strcmp(opcode, "jsrr") == 0)
        || (strcmp(opcode, "ldb") == 0)
        || (strcmp(opcode, "ldw") == 0)
        || (strcmp(opcode, "lea") == 0)
        || (strcmp(opcode, "nop") == 0)
        || (strcmp(opcode, "not") == 0)
        || (strcmp(opcode, "ret") == 0)
        || (strcmp(opcode, "lshf") == 0)
        || (strcmp(opcode, "rshfl") == 0)
        || (strcmp(opcode, "rshfa") == 0)
        || (strcmp(opcode, "rti") == 0)
        || (strcmp(opcode, "stb") == 0)
        || (strcmp(opcode, "stw") == 0)
        || (strcmp(opcode, "trap") == 0)
        || (strcmp(opcode, "halt") == 0)
        || (strcmp(opcode, "xor") == 0)
            )
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

// readAndParse function from Lab 1 Functions
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
           *lPtr != '\n' )
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n\r ," ) ) )
        return( EMPTY_LINE );
    int chks = strlen(lPtr);
    if(lPtr[chks-1]=='\r')
        lPtr[chks-1]='\0';
    if(strcmp(lPtr,a)==0)
    {
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

        *pArg1 = lPtr;
        callorig(pArg1);
        return( OK );
    }
    if(strcmp(lPtr,b)==0)
    {
        endflag = 1;
        return( DONE );

    }
    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) )
        {
            *pArg2 = lPtr;
            return( OK );}

    }
    return( OK );
}

// Fills out symbol table with all labels
void createsymb(FILE* infile)
{
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
            *lArg2, *lArg3, *lArg4;

    int lRet;
    do
    {
        lRet = readAndParse( infile, lLine, &lLabel,
                             &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE )
        { if(orig>-1) {
                int chks = strlen(lLabel);
                if(lLabel[chks-1]=='\r')
                    lLabel[chks-1]='\0';
                if (chks > 12)
                {
                    printf( "createsymb Error: other error, label longer than 12 chars \n");
                    exit(4);
                }
                if (strlen(lLabel) > 0) {
                    int i = 0;
                    if (i < trac) {
                        for (int i = 0; i < trac; i++) {
                            if (strcmp(symbolTable[i].label, lLabel) == 0)
                                exit(4);
                        }
                    }
                    if (lLabel[0] == 'x' || isdigit(lLabel[0]) != 0)
                    {
                        printf( "createsymb Error: other error, \n");
                        exit(4);
                    }
                    if (strcmp(lLabel, "in") == 0 || strcmp(lLabel, "out") == 0 || strcmp(lLabel, "getc") == 0 || strcmp(lLabel, "puts") == 0)
                    {
                        printf( "createsymb Error: label is IN/OUT/GETC/PUTS, \n");
                        exit(4);
                    }
                    for (int i = 0; i < strlen(lLabel); i++) {
                        if (isalnum(lLabel[i]) == 0)
                        {
                            printf( "createsymb Error: other error, \n");
                            exit(4);
                        }
                    }
                    if(strcmp(lLabel,"halt")!=0 && strcmp(lLabel,"ret")!=0 && strcmp(lLabel,"nop")!=0 && strcmp(lLabel,"rti\r")!=0)
                    {symbolTable[trac].address = count;
                        strcpy(symbolTable[trac].label, lLabel);
                        trac++;}
                }

                count++;
                if (count > 0xFFFF)
                {
                    printf( "createsymb Error: PC is higher than 0xFFFF, PC = %d\n", count);
                    exit(3);
                }
            }
        }
    } while( lRet != DONE );
    if(orig<0)
    {
        printf( "createsymb Error: other error, orig = %d\n", orig);
        exit(4);
    }
    else
        fprintf( outfile, "0x%.4X\n", orig );
    if(endflag==0)
    {
        printf( "createsymb Error: other error, endflag = %d\n", endflag);
        exit(4);
    }
}

// readAndparse function from Lab 1 Functions
int readAndParse1( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
           *lPtr != '\n' )
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n\r ," ) ) )
        return( EMPTY_LINE );
    int chks = strlen(lPtr);
    if(lPtr[chks-1]=='\r')
        lPtr[chks-1]='\0';
    if(strcmp(lPtr,a)==0)
    {
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
        origflag = 1;
        strcpy(*pOpcode,a);
        return( OK );
    }
    if(strcmp(lPtr,b)==0)
    {
        return (DONE);
    }
    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }

    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg1 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}

// Function converts input line into char* output in format x0000
char* convert(char* pOpcode, char*  pArg1, char*  pArg2, char* pArg3, char* pArg4, char* t)
{
    strcpy(t,"");
    if(strcmp(pOpcode,"add")==0) {
        strcpy(t,"0x1");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")!=0 && strcmp(pArg4,"")==0) {
            int size = strlen(pArg3);//Total size of string
            if(pArg3[size-1]=='\r')
                pArg3[size-1] = '\0';
            if (pArg1[0] == 'r' && pArg2[0] == 'r' && pArg3[0] == 'r') {
                char temp3[2];
                if(strcmp(pArg3,"r0")==0)
                    strcpy(temp3,"0");
                else if(strcmp(pArg3,"r1")==0)
                    strcpy(temp3,"1");
                else if(strcmp(pArg3,"r2")==0)
                    strcpy(temp3,"2");
                else if(strcmp(pArg3,"r3")==0)
                    strcpy(temp3,"3");
                else if(strcmp(pArg3,"r4")==0)
                    strcpy(temp3,"4");
                else if(strcmp(pArg3,"r5")==0)
                    strcpy(temp3,"5");
                else if(strcmp(pArg3,"r6")==0)
                    strcpy(temp3,"6");
                else if(strcmp(pArg3,"r7")==0)
                    strcpy(temp3,"7");
                else
                {
                    exit(4);
                }
                char temp2[2];
                int f1 =0;
                if(strcmp(pArg2,"r0")==0)
                    strcpy(temp2,"0");
                else if(strcmp(pArg2,"r1")==0)
                    strcpy(temp2,"4");
                else if(strcmp(pArg2,"r2")==0)
                    strcpy(temp2,"8");
                else if(strcmp(pArg2,"r3")==0)
                    strcpy(temp2,"C");
                else if(strcmp(pArg2,"r4")==0) {
                    strcpy(temp2, "0");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r5")==0) {
                    strcpy(temp2, "4");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r6")==0)
                {
                    strcpy(temp2, "8");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r7")==0)
                {
                    strcpy(temp2, "C");
                    f1 = 1;
                }
                else
                {
                    exit(4);
                }
                char temp1[2];
                if(f1==0)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"0");
                    else if(strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"2");
                    else if(strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"4");
                    else if(strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"6");
                    else if(strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"8");
                    else if(strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"A");
                    else if(strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"C");
                    else if(strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"E");
                    else
                    {
                        exit(4);
                    }
                }
                else if(f1 == 1)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"1");
                    else if(strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"3");
                    else if(strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"5");
                    else if(strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"7");
                    else if(strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"9");
                    else if(strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"B");
                    else if(strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"D");
                    else if(strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"F");
                    else
                    {
                        exit(4);
                    }
                }
                strncat(t,temp1,1);
                strncat(t,temp2,1);
                strncat(t,temp3,1);
                return t;
            }
            else {
                int d = toNum(pArg3);
                if (d > 15 || d < -16)
                    exit(3);
                char temp3[2];
                int f1 = 0;
                if(d == -16)
                {
                    strcpy(temp3,"0");
                    f1 = 1;
                }
                else  if(d == -15)
                {
                    strcpy(temp3,"1");
                    f1 = 1;
                }
                else  if(d == -14)
                {
                    strcpy(temp3,"2");
                    f1 = 1;
                }
                else  if(d == -13)
                {
                    strcpy(temp3,"3");
                    f1 = 1;
                }
                else  if(d == -12)
                {
                    strcpy(temp3,"4");
                    f1 = 1;
                }
                else  if(d == -11)
                {
                    strcpy(temp3,"5");
                    f1 = 1;
                }
                else  if(d == -10)
                {
                    strcpy(temp3,"6");
                    f1 = 1;
                }
                else  if(d == -9)
                {
                    strcpy(temp3,"7");
                    f1 = 1;
                }
                else  if(d == -8)
                {
                    strcpy(temp3,"8");
                    f1 = 1;
                }
                else  if(d == -7)
                {
                    strcpy(temp3,"9");
                    f1 = 1;
                }
                else  if(d == -6)
                {
                    strcpy(temp3,"A");
                    f1 = 1;
                }
                else  if(d == -5)
                {
                    strcpy(temp3,"B");
                    f1 = 1;
                }
                else  if(d == -4)
                {
                    strcpy(temp3,"C");
                    f1 = 1;
                }
                else  if(d == -3)
                {
                    strcpy(temp3,"D");
                    f1 = 1;
                }
                else  if(d == -2)
                {
                    strcpy(temp3,"E");
                    f1 = 1;
                }
                else  if(d == -1)
                {
                    strcpy(temp3,"F");
                    f1 = 1;
                }
                else  if(d == 0)
                {
                    strcpy(temp3,"0");
                }
                else  if(d == 1)
                {
                    strcpy(temp3,"1");
                }
                else  if(d == 2)
                {
                    strcpy(temp3,"2");
                }
                else  if(d == 3)
                {
                    strcpy(temp3,"3");
                }
                else  if(d == 4)
                {
                    strcpy(temp3,"4");
                }
                else  if(d == 5)
                {
                    strcpy(temp3,"5");
                }
                else  if(d == 6)
                {
                    strcpy(temp3,"6");
                }
                else  if(d == 7)
                {
                    strcpy(temp3,"7");
                }
                else  if(d == 8)
                {
                    strcpy(temp3,"8");
                }
                else  if(d == 9)
                {
                    strcpy(temp3,"9");
                }
                else  if(d == 10)
                {
                    strcpy(temp3,"A");
                }
                else  if(d == 11)
                {
                    strcpy(temp3,"B");
                }
                else  if(d == 12)
                {
                    strcpy(temp3,"C");
                }
                else  if(d == 13)
                {
                    strcpy(temp3,"D");
                }
                else  if(d == 14)
                {
                    strcpy(temp3,"E");
                }
                else  if(d == 15)
                {
                    strcpy(temp3,"F");
                }
                char temp2[2];
                int f2 = 0;
                if(f1== 0)
                {
                    if(strcmp(pArg2,"r0")==0)
                    {
                        strcpy(temp2,"2");
                    }
                    else if(strcmp(pArg2,"r1")==0)
                    {
                        strcpy(temp2,"6");
                    }
                    else if(strcmp(pArg2,"r2")==0)
                    {
                        strcpy(temp2,"A");
                    }
                    else if(strcmp(pArg2,"r3")==0)
                    {
                        strcpy(temp2,"E");
                    }
                    else if(strcmp(pArg2,"r4")==0)
                    {
                        strcpy(temp2,"2");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r5")==0)
                    {
                        strcpy(temp2,"6");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r6")==0)
                    {
                        strcpy(temp2,"A");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r7")==0)
                    {
                        strcpy(temp2,"E");
                        f2 = 1;
                    }
                    else
                        exit(4);
                }
                else if(f1 == 1)
                {
                    if(strcmp(pArg2,"r0")==0)
                    {
                        strcpy(temp2,"3");
                    }
                    else if(strcmp(pArg2,"r1")==0)
                    {
                        strcpy(temp2,"7");
                    }
                    else if(strcmp(pArg2,"r2")==0)
                    {
                        strcpy(temp2,"B");
                    }
                    else if(strcmp(pArg2,"r3")==0)
                    {
                        strcpy(temp2,"F");
                    }
                    else if(strcmp(pArg2,"r4")==0)
                    {
                        strcpy(temp2,"3");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r5")==0)
                    {
                        strcpy(temp2,"7");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r6")==0)
                    {
                        strcpy(temp2,"B");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r7")==0)
                    {
                        strcpy(temp2,"F");
                        f2 = 1;
                    }
                    else
                    {
                        exit(4);
                    }
                }
                char temp1[2]; /*whats the ptoblem here*/
                if(f2 == 1)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"1");
                    else if (strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"3");
                    else if (strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"5");
                    else if (strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"7");
                    else if (strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"9");
                    else if (strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"B");
                    else if (strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"D");
                    else if (strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"F");
                    else
                    {
                        exit(4);
                    }
                }
                else if(f2 ==0)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"0");
                    else if (strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"2");
                    else if (strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"4");
                    else if (strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"6");
                    else if (strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"8");
                    else if (strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"A");
                    else if (strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"C");
                    else if (strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"E");
                    else
                    {
                        exit(4);
                    }
                }
                strncat(t,temp1,1);
                strncat(t,temp2,1);
                strncat(t,temp3,1);
                return t;
            }
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"and")==0)
    {
        strcpy(t,"0x5");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")!=0 && strcmp(pArg4,"")==0) {
            int size = strlen(pArg3);//Total size of string
            if(pArg3[size-1]=='\r')
                pArg3[size-1] = '\0';
            if (pArg1[0] == 'r' && pArg2[0] == 'r' && pArg3[0] == 'r') {
                char temp3[2];
                if(strcmp(pArg3,"r0")==0)
                    strcpy(temp3,"0");
                else if(strcmp(pArg3,"r1")==0)
                    strcpy(temp3,"1");
                else if(strcmp(pArg3,"r2")==0)
                    strcpy(temp3,"2");
                else if(strcmp(pArg3,"r3")==0)
                    strcpy(temp3,"3");
                else if(strcmp(pArg3,"r4")==0)
                    strcpy(temp3,"4");
                else if(strcmp(pArg3,"r5")==0)
                    strcpy(temp3,"5");
                else if(strcmp(pArg3,"r6")==0)
                    strcpy(temp3,"6");
                else if(strcmp(pArg3,"r7")==0)
                    strcpy(temp3,"7");
                else
                {
                    exit(4);
                }
                char temp2[2];
                int f1 =0;
                if(strcmp(pArg2,"r0")==0)
                    strcpy(temp2,"0");
                else if(strcmp(pArg2,"r1")==0)
                    strcpy(temp2,"4");
                else if(strcmp(pArg2,"r2")==0)
                    strcpy(temp2,"8");
                else if(strcmp(pArg2,"r3")==0)
                    strcpy(temp2,"C");
                else if(strcmp(pArg2,"r4")==0) {
                    strcpy(temp2, "0");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r5")==0) {
                    strcpy(temp2, "4");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r6")==0)
                {
                    strcpy(temp2, "8");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r7")==0)
                {
                    strcpy(temp2, "C");
                    f1 = 1;
                }
                else
                {
                    exit(4);
                }
                char temp1[2];
                if(f1==0)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"0");
                    else if(strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"2");
                    else if(strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"4");
                    else if(strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"6");
                    else if(strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"8");
                    else if(strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"A");
                    else if(strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"C");
                    else if(strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"E");
                    else
                    {
                        exit(4);
                    }
                }
                else if(f1 == 1)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"1");
                    else if(strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"3");
                    else if(strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"5");
                    else if(strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"7");
                    else if(strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"9");
                    else if(strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"B");
                    else if(strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"D");
                    else if(strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"F");
                    else
                    {
                        exit(4);
                    }
                }
                strncat(t,temp1,1);
                strncat(t,temp2,1);
                strncat(t,temp3,1);
                return t;
            }
            else {
                int d = toNum(pArg3);
                if (d > 15 || d < -16)
                    exit(3);
                char temp3[2];
                int f1 = 0;
                if(d == -16)
                {
                    strcpy(temp3,"0");
                    f1 = 1;
                }
                else  if(d == -15)
                {
                    strcpy(temp3,"1");
                    f1 = 1;
                }
                else  if(d == -14)
                {
                    strcpy(temp3,"2");
                    f1 = 1;
                }
                else  if(d == -13)
                {
                    strcpy(temp3,"3");
                    f1 = 1;
                }
                else  if(d == -12)
                {
                    strcpy(temp3,"4");
                    f1 = 1;
                }
                else  if(d == -11)
                {
                    strcpy(temp3,"5");
                    f1 = 1;
                }
                else  if(d == -10)
                {
                    strcpy(temp3,"6");
                    f1 = 1;
                }
                else  if(d == -9)
                {
                    strcpy(temp3,"7");
                    f1 = 1;
                }
                else  if(d == -8)
                {
                    strcpy(temp3,"8");
                    f1 = 1;
                }
                else  if(d == -7)
                {
                    strcpy(temp3,"9");
                    f1 = 1;
                }
                else  if(d == -6)
                {
                    strcpy(temp3,"A");
                    f1 = 1;
                }
                else  if(d == -5)
                {
                    strcpy(temp3,"B");
                    f1 = 1;
                }
                else  if(d == -4)
                {
                    strcpy(temp3,"C");
                    f1 = 1;
                }
                else  if(d == -3)
                {
                    strcpy(temp3,"D");
                    f1 = 1;
                }
                else  if(d == -2)
                {
                    strcpy(temp3,"E");
                    f1 = 1;
                }
                else  if(d == -1)
                {
                    strcpy(temp3,"F");
                    f1 = 1;
                }
                else  if(d == 0)
                {
                    strcpy(temp3,"0");
                }
                else  if(d == 1)
                {
                    strcpy(temp3,"1");
                }
                else  if(d == 2)
                {
                    strcpy(temp3,"2");
                }
                else  if(d == 3)
                {
                    strcpy(temp3,"3");
                }
                else  if(d == 4)
                {
                    strcpy(temp3,"4");
                }
                else  if(d == 5)
                {
                    strcpy(temp3,"5");
                }
                else  if(d == 6)
                {
                    strcpy(temp3,"6");
                }
                else  if(d == 7)
                {
                    strcpy(temp3,"7");
                }
                else  if(d == 8)
                {
                    strcpy(temp3,"8");
                }
                else  if(d == 9)
                {
                    strcpy(temp3,"9");
                }
                else  if(d == 10)
                {
                    strcpy(temp3,"A");
                }
                else  if(d == 11)
                {
                    strcpy(temp3,"B");
                }
                else  if(d == 12)
                {
                    strcpy(temp3,"C");
                }
                else  if(d == 13)
                {
                    strcpy(temp3,"D");
                }
                else  if(d == 14)
                {
                    strcpy(temp3,"E");
                }
                else  if(d == 15)
                {
                    strcpy(temp3,"F");
                }
                char temp2[2];
                int f2 = 0;
                if(f1== 0)
                {
                    if(strcmp(pArg2,"r0")==0)
                    {
                        strcpy(temp2,"2");
                    }
                    else if(strcmp(pArg2,"r1")==0)
                    {
                        strcpy(temp2,"6");
                    }
                    else if(strcmp(pArg2,"r2")==0)
                    {
                        strcpy(temp2,"A");
                    }
                    else if(strcmp(pArg2,"r3")==0)
                    {
                        strcpy(temp2,"E");
                    }
                    else if(strcmp(pArg2,"r4")==0)
                    {
                        strcpy(temp2,"2");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r5")==0)
                    {
                        strcpy(temp2,"6");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r6")==0)
                    {
                        strcpy(temp2,"A");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r7")==0)
                    {
                        strcpy(temp2,"E");
                        f2 = 1;
                    }
                    else
                        exit(4);
                }
                else if(f1 == 1)
                {
                    if(strcmp(pArg2,"r0")==0)
                    {
                        strcpy(temp2,"3");
                    }
                    else if(strcmp(pArg2,"r1")==0)
                    {
                        strcpy(temp2,"7");
                    }
                    else if(strcmp(pArg2,"r2")==0)
                    {
                        strcpy(temp2,"B");
                    }
                    else if(strcmp(pArg2,"r3")==0)
                    {
                        strcpy(temp2,"F");
                    }
                    else if(strcmp(pArg2,"r4")==0)
                    {
                        strcpy(temp2,"3");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r5")==0)
                    {
                        strcpy(temp2,"7");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r6")==0)
                    {
                        strcpy(temp2,"B");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r7")==0)
                    {
                        strcpy(temp2,"F");
                        f2 = 1;
                    }
                    else
                    {
                        exit(4);
                    }
                }
                char temp1[2];
                if(f2 == 1)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"1");
                    else if (strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"3");
                    else if (strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"5");
                    else if (strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"7");
                    else if (strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"9");
                    else if (strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"B");
                    else if (strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"D");
                    else if (strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"F");
                    else
                    {
                        exit(4);
                    }
                }
                else if(f2 ==0)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"0");
                    else if (strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"2");
                    else if (strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"4");
                    else if (strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"6");
                    else if (strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"8");
                    else if (strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"A");
                    else if (strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"C");
                    else if (strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"E");
                    else
                    {
                        exit(4);
                    }
                }
                strncat(t,temp1,1);
                strncat(t,temp2,1);
                strncat(t,temp3,1);
                return t;
            }
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"jmp")==0)
    {
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")==0)
        {
            int size = strlen(pArg1);//Total size of string
            if(pArg1[size-1]=='\r')
                pArg1[size-1] = '\0';
            if(strcmp(pArg1,"r0")==0)
            {
                strcpy(t,"0xC000");
                return t;
            }
            else if(strcmp(pArg1,"r1")==0)
            {
                strcpy(t,"0xC040");
                return t;
            }
            else if(strcmp(pArg1,"r2")==0)
            {
                strcpy(t,"0xC080");
                return t;
            }
            else if(strcmp(pArg1,"r3")==0)
            {
                strcpy(t,"0xC0C0");
                return t;
            }
            else if(strcmp(pArg1,"r4")==0)
            {
                strcpy(t,"0xC100");
                return t;
            }
            else if(strcmp(pArg1,"r5")==0)
            {
                strcpy(t,"0xC140");
                return t;
            }
            else if(strcmp(pArg1,"r6")==0)
            {
                strcpy(t,"0xC180");
                return t;
            }
            else if(strcmp(pArg1,"r7")==0)
            {
                strcpy(t,"0xC1C0");
                return t;
            }
            else
            {
                exit(4);
            }
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"ret")==0)
    {
        if(strcmp(pArg1,"")!=0)
        {
            exit(4);
        }
        else
        {
            strcpy(t,"0xC1C0");
            return t;
        }
    }
    else if(strcmp(pOpcode,"jsrr")==0)
    {
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")==0 )
        {
            int size = strlen(pArg1);//Total size of string
            if(pArg1[size-1]=='\r')
                pArg1[size-1] = '\0';
            if(strcmp(pArg1,"r0")==0)
            {
                strcpy(t,"0x4000");
                return t;
            }
            else if(strcmp(pArg1,"r1")==0)
            {
                strcpy(t,"0x4040");
                return t;
            }
            else if(strcmp(pArg1,"r2")==0)
            {
                strcpy(t,"0x4080");
                return t;
            }
            else if(strcmp(pArg1,"r3")==0)
            {
                strcpy(t,"0x40C0");
                return t;
            }
            else if(strcmp(pArg1,"r4")==0)
            {
                strcpy(t,"0x4100");
                return t;
            }
            else if(strcmp(pArg1,"r5")==0)
            {
                strcpy(t,"0x4140");
                return t;
            }
            else if(strcmp(pArg1,"r6")==0)
            {
                strcpy(t,"0x4180");
                return t;
            }
            else if(strcmp(pArg1,"r7")==0)
            {
                strcpy(t,"0x41C0");
                return t;
            }
            else
            {
                exit(4);
            }
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"not")==0)
    {
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")==0)
        {
            int size = strlen(pArg2);//Total size of string
            if(pArg2[size-1]=='\r')
                pArg2[size-1] = '\0';
            strcpy(t,"0x9");
            char t2[3];
            int f = 0;
            if(strcmp(pArg2,"r0")==0)
            {
                strcpy(t2,"3F");
            }
            else if(strcmp(pArg2,"r1")==0)
            {
                strcpy(t2,"7F");
            }
            else if(strcmp(pArg2,"r2")==0)
            {
                strcpy(t2,"BF");
            }
            else if(strcmp(pArg2,"r3")==0)
            {
                strcpy(t2,"FF");
            }
            else if(strcmp(pArg2,"r4")==0)
            {
                strcpy(t2,"3F");
                f = 1;
            }
            else if(strcmp(pArg2,"r5")==0)
            {
                strcpy(t2,"7F");
                f = 1;
            }
            else if(strcmp(pArg2,"r6")==0)
            {
                strcpy(t2,"BF");
                f = 1;
            }
            else if(strcmp(pArg2,"r7")==0)
            {
                strcpy(t2,"FF");
                f = 1;
            }
            else
            {
                exit(4);
            }
            char t1[2];
            if(f==0)
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(t1,"0");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(t1,"2");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(t1,"4");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(t1,"6");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(t1,"8");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(t1,"A");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(t1,"C");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(t1,"E");
                }
                else
                {
                    exit(4);
                }
            }
            else if(f == 1)
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(t1,"1");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(t1,"3");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(t1,"5");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(t1,"7");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(t1,"9");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(t1,"B");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(t1,"D");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(t1,"F");
                }
                else
                {
                    exit(4);
                }
            }
            strncat(t,t1,1);
            strncat(t,t2,2);
            return t;
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"rti")==0)
    {
        if(strcmp(pArg1,"")==0) {
            strcpy(t, "0x8000");
            return t;
        }
        else
            exit(4);
    }
    else if(strcmp(pOpcode,"nop")==0)
    {
        if(strcmp(pArg1,"")==0)
        {
            strcpy(t,"0x0000");
            return t;
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"trap")==0)
    {
        strcpy(t,"0xF0");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")==0)
        {
            int size = strlen(pArg1);//Total size of string
            if(pArg1[size-1]=='\r')
                pArg1[size-1] = '\0';
            if(toNum(pArg1)<0)
                exit(4);
            if(toNum(pArg1)>255)
                exit(3);
            if (pArg1[0] == 'x')
            {
                for(int i = 0; i< strlen(pArg1);i++)
                    pArg1[i]=pArg1[i+1];
            }
            strncat(t,pArg1,2);
            return t;
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"xor")==0) {
        strcpy(t,"0x9");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")!=0 && strcmp(pArg4,"")==0) {
            int size = strlen(pArg3);//Total size of string
            if(pArg3[size-1]=='\r')
                pArg3[size-1] = '\0';
            if (pArg1[0] == 'r' && pArg2[0] == 'r' && pArg3[0] == 'r') {
                char temp3[2];
                if(strcmp(pArg3,"r0")==0)
                    strcpy(temp3,"0");
                else if(strcmp(pArg3,"r1")==0)
                    strcpy(temp3,"1");
                else if(strcmp(pArg3,"r2")==0)
                    strcpy(temp3,"2");
                else if(strcmp(pArg3,"r3")==0)
                    strcpy(temp3,"3");
                else if(strcmp(pArg3,"r4")==0)
                    strcpy(temp3,"4");
                else if(strcmp(pArg3,"r5")==0)
                    strcpy(temp3,"5");
                else if(strcmp(pArg3,"r6")==0)
                    strcpy(temp3,"6");
                else if(strcmp(pArg3,"r7")==0)
                    strcpy(temp3,"7");
                else
                {
                    exit(4);
                }
                char temp2[2];
                int f1 =0;
                if(strcmp(pArg2,"r0")==0)
                    strcpy(temp2,"0");
                else if(strcmp(pArg2,"r1")==0)
                    strcpy(temp2,"4");
                else if(strcmp(pArg2,"r2")==0)
                    strcpy(temp2,"8");
                else if(strcmp(pArg2,"r3")==0)
                    strcpy(temp2,"C");
                else if(strcmp(pArg2,"r4")==0) {
                    strcpy(temp2, "0");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r5")==0) {
                    strcpy(temp2, "4");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r6")==0)
                {
                    strcpy(temp2, "8");
                    f1 = 1;
                }
                else if(strcmp(pArg2,"r7")==0)
                {
                    strcpy(temp2, "C");
                    f1 = 1;
                }
                else
                {
                    exit(4);
                }
                char temp1[2];
                if(f1==0)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"0");
                    else if(strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"2");
                    else if(strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"4");
                    else if(strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"6");
                    else if(strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"8");
                    else if(strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"A");
                    else if(strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"C");
                    else if(strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"E");
                    else
                    {
                        exit(4);
                    }
                }
                else if(f1 == 1)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"1");
                    else if(strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"3");
                    else if(strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"5");
                    else if(strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"7");
                    else if(strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"9");
                    else if(strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"B");
                    else if(strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"D");
                    else if(strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"F");
                    else
                    {
                        exit(4);
                    }
                }
                strncat(t,temp1,1);
                strncat(t,temp2,1);
                strncat(t,temp3,1);
                return t;
            }
            else {
                int d = toNum(pArg3);
                if (d > 15 && d < -16)
                    exit(3);
                char temp3[2];
                int f1 = 0;
                if(d == -16)
                {
                    strcpy(temp3,"0");
                    f1 = 1;
                }
                else  if(d == -15)
                {
                    strcpy(temp3,"1");
                    f1 = 1;
                }
                else  if(d == -14)
                {
                    strcpy(temp3,"2");
                    f1 = 1;
                }
                else  if(d == -13)
                {
                    strcpy(temp3,"3");
                    f1 = 1;
                }
                else  if(d == -12)
                {
                    strcpy(temp3,"4");
                    f1 = 1;
                }
                else  if(d == -11)
                {
                    strcpy(temp3,"5");
                    f1 = 1;
                }
                else  if(d == -10)
                {
                    strcpy(temp3,"6");
                    f1 = 1;
                }
                else  if(d == -9)
                {
                    strcpy(temp3,"7");
                    f1 = 1;
                }
                else  if(d == -8)
                {
                    strcpy(temp3,"8");
                    f1 = 1;
                }
                else  if(d == -7)
                {
                    strcpy(temp3,"9");
                    f1 = 1;
                }
                else  if(d == -6)
                {
                    strcpy(temp3,"A");
                    f1 = 1;
                }
                else  if(d == -5)
                {
                    strcpy(temp3,"B");
                    f1 = 1;
                }
                else  if(d == -4)
                {
                    strcpy(temp3,"C");
                    f1 = 1;
                }
                else  if(d == -3)
                {
                    strcpy(temp3,"D");
                    f1 = 1;
                }
                else  if(d == -2)
                {
                    strcpy(temp3,"E");
                    f1 = 1;
                }
                else  if(d == -1)
                {
                    strcpy(temp3,"F");
                    f1 = 1;
                }
                else  if(d == 0)
                {
                    strcpy(temp3,"0");
                }
                else  if(d == 1)
                {
                    strcpy(temp3,"1");
                }
                else  if(d == 2)
                {
                    strcpy(temp3,"2");
                }
                else  if(d == 3)
                {
                    strcpy(temp3,"3");
                }
                else  if(d == 4)
                {
                    strcpy(temp3,"4");
                }
                else  if(d == 5)
                {
                    strcpy(temp3,"5");
                }
                else  if(d == 6)
                {
                    strcpy(temp3,"6");
                }
                else  if(d == 7)
                {
                    strcpy(temp3,"7");
                }
                else  if(d == 8)
                {
                    strcpy(temp3,"8");
                }
                else  if(d == 9)
                {
                    strcpy(temp3,"9");
                }
                else  if(d == 10)
                {
                    strcpy(temp3,"A");
                }
                else  if(d == 11)
                {
                    strcpy(temp3,"B");
                }
                else  if(d == 12)
                {
                    strcpy(temp3,"C");
                }
                else  if(d == 13)
                {
                    strcpy(temp3,"D");
                }
                else  if(d == 14)
                {
                    strcpy(temp3,"E");
                }
                else  if(d == 15)
                {
                    strcpy(temp3,"F");
                }
                char temp2[2];
                int f2 = 0;
                if(f1== 0)
                {
                    if(strcmp(pArg2,"r0")==0)
                    {
                        strcpy(temp2,"2");
                    }
                    else if(strcmp(pArg2,"r1")==0)
                    {
                        strcpy(temp2,"6");
                    }
                    else if(strcmp(pArg2,"r2")==0)
                    {
                        strcpy(temp2,"A");
                    }
                    else if(strcmp(pArg2,"r3")==0)
                    {
                        strcpy(temp2,"E");
                    }
                    else if(strcmp(pArg2,"r4")==0)
                    {
                        strcpy(temp2,"2");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r5")==0)
                    {
                        strcpy(temp2,"6");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r6")==0)
                    {
                        strcpy(temp2,"A");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r7")==0)
                    {
                        strcpy(temp2,"E");
                        f2 = 1;
                    }
                    else
                        exit(4);
                }
                else if(f1 == 1)
                {
                    if(strcmp(pArg2,"r0")==0)
                    {
                        strcpy(temp2,"3");
                    }
                    else if(strcmp(pArg2,"r1")==0)
                    {
                        strcpy(temp2,"7");
                    }
                    else if(strcmp(pArg2,"r2")==0)
                    {
                        strcpy(temp2,"B");
                    }
                    else if(strcmp(pArg2,"r3")==0)
                    {
                        strcpy(temp2,"F");
                    }
                    else if(strcmp(pArg2,"r4")==0)
                    {
                        strcpy(temp2,"3");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r5")==0)
                    {
                        strcpy(temp2,"7");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r6")==0)
                    {
                        strcpy(temp2,"B");
                        f2 = 1;
                    }
                    else if(strcmp(pArg2,"r7")==0)
                    {
                        strcpy(temp2,"F");
                        f2 = 1;
                    }
                    else
                    {
                        exit(4);
                    }
                }
                char temp1[2];
                if(f2 == 1)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"1");
                    else if (strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"3");
                    else if (strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"5");
                    else if (strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"7");
                    else if (strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"9");
                    else if (strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"B");
                    else if (strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"D");
                    else if (strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"F");
                    else
                    {
                        exit(4);
                    }
                }
                else if(f2 ==0)
                {
                    if(strcmp(pArg1,"r0")==0)
                        strcpy(temp1,"0");
                    else if (strcmp(pArg1,"r1")==0)
                        strcpy(temp1,"2");
                    else if (strcmp(pArg1,"r2")==0)
                        strcpy(temp1,"4");
                    else if (strcmp(pArg1,"r3")==0)
                        strcpy(temp1,"6");
                    else if (strcmp(pArg1,"r4")==0)
                        strcpy(temp1,"8");
                    else if (strcmp(pArg1,"r5")==0)
                        strcpy(temp1,"A");
                    else if (strcmp(pArg1,"r6")==0)
                        strcpy(temp1,"C");
                    else if (strcmp(pArg1,"r7")==0)
                        strcpy(temp1,"E");
                    else
                    {
                        exit(4);
                    }
                }
                strncat(t,temp1,1);
                strncat(t,temp2,1);
                strncat(t,temp3,1);
                return t;
            }
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"lshf")==0)
    {
        strcpy(t,"0xD");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")!=0 && strcmp(pArg4,"")==0)
        {
            int size = strlen(pArg3);//Total size of string
            if(pArg3[size-1]=='\r')
                pArg3[size-1] = '\0';
            int numtemp = toNum(pArg3);
            if(numtemp<0 || numtemp > 15)
                exit(3);
            char temp3[2];
            if(numtemp == 0)
            {
                strcpy(temp3,"0");
            }
            else if(numtemp == 1)
            {
                strcpy(temp3,"1");
            }
            else if(numtemp == 2)
            {
                strcpy(temp3,"2");
            }
            else if(numtemp == 3)
            {
                strcpy(temp3,"3");
            }
            else if(numtemp == 4)
            {
                strcpy(temp3,"4");
            }
            else if(numtemp == 5)
            {
                strcpy(temp3,"5");
            }
            else if(numtemp == 6)
            {
                strcpy(temp3,"6");
            }
            else if(numtemp == 7)
            {
                strcpy(temp3,"7");
            }
            else if(numtemp == 8)
            {
                strcpy(temp3,"8");
            }
            else if(numtemp == 9)
            {
                strcpy(temp3,"9");
            }
            else if(numtemp == 10)
            {
                strcpy(temp3,"A");
            }
            else if(numtemp == 11)
            {
                strcpy(temp3,"B");
            }
            else if(numtemp == 12)
            {
                strcpy(temp3,"C");
            }
            else if(numtemp == 13)
            {
                strcpy(temp3,"D");
            }
            else if(numtemp == 14)
            {
                strcpy(temp3,"E");
            }
            else if(numtemp == 15)
            {
                strcpy(temp3,"F");
            }
            char temp2[2];int f = 0;
            if(strcmp(pArg2,"r0")==0)
            {
                strcpy(temp2,"0");
            }
            else if(strcmp(pArg2,"r1")==0)
            {
                strcpy(temp2,"4");
            }
            else if(strcmp(pArg2,"r2")==0)
            {
                strcpy(temp2,"8");
            }
            else if(strcmp(pArg2,"r3")==0)
            {
                strcpy(temp2,"C");
            }
            else if(strcmp(pArg2,"r4")==0)
            {
                strcpy(temp2,"0");
                f =1;
            }
            else if(strcmp(pArg2,"r5")==0)
            {
                strcpy(temp2,"4");
                f =1;
            }
            else if(strcmp(pArg2,"r6")==0)
            {
                strcpy(temp2,"8");
                f =1;
            }
            else if(strcmp(pArg2,"r7")==0)
            {
                strcpy(temp2,"C");
                f =1;
            }
            else
            {
                exit(4);
            }
            char temp1[2];
            if(f== 0)
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(temp1,"0");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(temp1,"2");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(temp1,"4");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(temp1,"6");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(temp1,"8");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(temp1,"A");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(temp1,"C");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(temp1,"E");
                }
                else
                {
                    exit(4);
                }
            }
            else
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(temp1,"1");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(temp1,"3");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(temp1,"5");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(temp1,"7");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(temp1,"9");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(temp1,"B");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(temp1,"D");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(temp1,"F");
                }
                else
                {
                    exit(4);
                }
            }
            strncat(t,temp1,1);
            strncat(t,temp2,1);
            strncat(t,temp3,1);
            return t;
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"rshfl")==0)
    {
        strcpy(t,"0xD");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")!=0 && strcmp(pArg4,"")==0)
        {
            int size = strlen(pArg3);//Total size of string
            if(pArg3[size-1]=='\r')
                pArg3[size-1] = '\0';
            int numtemp = toNum(pArg3);
            if(numtemp<0 || numtemp > 15)
                exit(3);
            char temp3[2];
            if(numtemp == 0)
            {
                strcpy(temp3,"0");
            }
            else if(numtemp ==1)
            {
                strcpy(temp3,"1");
            }
            else if(numtemp == 2)
            {
                strcpy(temp3,"2");
            }
            else if(numtemp == 3)
            {
                strcpy(temp3,"3");
            }
            else if(numtemp == 4)
            {
                strcpy(temp3,"4");
            }
            else if(numtemp == 5)
            {
                strcpy(temp3,"5");
            }
            else if(numtemp == 6)
            {
                strcpy(temp3,"6");
            }
            else if(numtemp == 7)
            {
                strcpy(temp3,"7");
            }
            else if(numtemp == 8)
            {
                strcpy(temp3,"8");
            }
            else if(numtemp == 9)
            {
                strcpy(temp3,"9");
            }
            else if(numtemp == 10)
            {
                strcpy(temp3,"A");
            }
            else if(numtemp == 11)
            {
                strcpy(temp3,"B");
            }
            else if(numtemp == 12)
            {
                strcpy(temp3,"C");
            }
            else if(numtemp == 13)
            {
                strcpy(temp3,"D");
            }
            else if(numtemp == 14)
            {
                strcpy(temp3,"E");
            }
            else if(numtemp == 15)
            {
                strcpy(temp3,"F");
            }
            char temp2[2];int f = 0;
            if(strcmp(pArg2,"r0")==0)
            {
                strcpy(temp2,"1");
            }
            else if(strcmp(pArg2,"r1")==0)
            {
                strcpy(temp2,"5");
            }
            else if(strcmp(pArg2,"r2")==0)
            {
                strcpy(temp2,"9");
            }
            else if(strcmp(pArg2,"r3")==0)
            {
                strcpy(temp2,"D");
            }
            else if(strcmp(pArg2,"r4")==0)
            {
                strcpy(temp2,"1");
                f =1;
            }
            else if(strcmp(pArg2,"r5")==0)
            {
                strcpy(temp2,"5");
                f =1;
            }
            else if(strcmp(pArg2,"r6")==0)
            {
                strcpy(temp2,"9");
                f =1;
            }
            else if(strcmp(pArg2,"r7")==0)
            {
                strcpy(temp2,"D");
                f =1;
            }
            else
            {
                exit(4);
            }
            char temp1[2];
            if(f== 0)
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(temp1,"0");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(temp1,"2");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(temp1,"4");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(temp1,"6");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(temp1,"8");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(temp1,"A");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(temp1,"C");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(temp1,"E");
                }
                else
                {
                    exit(4);
                }
            }
            else
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(temp1,"1");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(temp1,"3");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(temp1,"5");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(temp1,"7");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(temp1,"9");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(temp1,"B");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(temp1,"D");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(temp1,"F");
                }
                else
                {
                    exit(4);
                }
            }
            strncat(t,temp1,1);
            strncat(t,temp2,1);
            strncat(t,temp3,1);
            return t;
        }
        else
        {
            exit(4);
        }
    }
    else if(strcmp(pOpcode,"rshfa")==0)
    {
        strcpy(t,"0xD");
        if(strcmp(pArg1,"")!=0 && strcmp(pArg2,"")!=0 && strcmp(pArg3,"")!=0 && strcmp(pArg4,"")==0)
        {
            int size = strlen(pArg3);//Total size of string
            if(pArg3[size-1]=='\r')
                pArg3[size-1] = '\0';
            int numtemp = toNum(pArg3);
            if(numtemp<0 || numtemp > 15)
                exit(3);
            char temp3[2];
            if(numtemp == 0)
            {
                strcpy(temp3,"0");
            }
            else if(numtemp == 1)
            {
                strcpy(temp3,"1");
            }
            else if(numtemp == 2)
            {
                strcpy(temp3,"2");
            }
            else if(numtemp == 3)
            {
                strcpy(temp3,"3");
            }
            else if(numtemp == 4)
            {
                strcpy(temp3,"4");
            }
            else if(numtemp == 5)
            {
                strcpy(temp3,"5");
            }
            else if(numtemp == 6)
            {
                strcpy(temp3,"6");
            }
            else if(numtemp == 7)
            {
                strcpy(temp3,"7");
            }
            else if(numtemp == 8)
            {
                strcpy(temp3,"8");
            }
            else if(numtemp == 9)
            {
                strcpy(temp3,"9");
            }
            else if(numtemp == 10)
            {
                strcpy(temp3,"A");
            }
            else if(numtemp == 11)
            {
                strcpy(temp3,"B");
            }
            else if(numtemp == 12)
            {
                strcpy(temp3,"C");
            }
            else if(numtemp == 13)
            {
                strcpy(temp3,"D");
            }
            else if(numtemp == 14)
            {
                strcpy(temp3,"E");
            }
            else if(numtemp == 15)
            {
                strcpy(temp3,"F");
            }
            char temp2[2];int f = 0;
            if(strcmp(pArg2,"r0")==0)
            {
                strcpy(temp2,"3");
            }
            else if(strcmp(pArg2,"r1")==0)
            {
                strcpy(temp2,"7");
            }
            else if(strcmp(pArg2,"r2")==0)
            {
                strcpy(temp2,"B");
            }
            else if(strcmp(pArg2,"r3")==0)
            {
                strcpy(temp2,"F");
            }
            else if(strcmp(pArg2,"r4")==0)
            {
                strcpy(temp2,"3");
                f =1;
            }
            else if(strcmp(pArg2,"r5")==0)
            {
                strcpy(temp2,"7");
                f =1;
            }
            else if(strcmp(pArg2,"r6")==0)
            {
                strcpy(temp2,"B");
                f =1;
            }
            else if(strcmp(pArg2,"r7")==0)
            {
                strcpy(temp2,"F");
                f =1;
            }
            else
            {
                exit(4);
            }
            char temp1[2];
            if(f== 0)
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(temp1,"0");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(temp1,"2");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(temp1,"4");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(temp1,"6");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(temp1,"8");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(temp1,"A");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(temp1,"C");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(temp1,"E");
                }
                else
                {
                    exit(4);
                }
            }
            else
            {
                if(strcmp(pArg1,"r0")==0)
                {
                    strcpy(temp1,"1");
                }
                else if(strcmp(pArg1,"r1")==0)
                {
                    strcpy(temp1,"3");
                }
                else if(strcmp(pArg1,"r2")==0)
                {
                    strcpy(temp1,"5");
                }
                else if(strcmp(pArg1,"r3")==0)
                {
                    strcpy(temp1,"7");
                }
                else if(strcmp(pArg1,"r4")==0)
                {
                    strcpy(temp1,"9");
                }
                else if(strcmp(pArg1,"r5")==0)
                {
                    strcpy(temp1,"B");
                }
                else if(strcmp(pArg1,"r6")==0)
                {
                    strcpy(temp1,"D");
                }
                else if(strcmp(pArg1,"r7")==0)
                {
                    strcpy(temp1,"F");
                }
                else
                {
                    exit(4);
                }
            }
            strncat(t,temp1,1);
            strncat(t,temp2,1);
            strncat(t,temp3,1);
            return t;
        }
        else
        {
            exit(4);
        }
    }
    else
    {
        printf( "convert Error: invalid opcode, %s\n", pOpcode);
        exit(2);
    }
}

// Function converts input line into into int equivalent
int convertInt(char* pOpcode, char* pArg1, char* pArg2, char* pArg3, char* pArg4)
{
    int convertTemp;
    int labelAddress;

    if (strcmp(pOpcode, "jsr")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // JSR opcode in decimal
        convertTemp = 18432;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto JSRLabelFound;
            }
        }
        // If no label is found, exit with error code 4
        exit(1);
        JSRLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 1023, exit with error code 3
        if (labelAddress > 1023 || labelAddress < -1024)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (2048 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "br")==0)
    {   int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BR opcode in decimal
        convertTemp = 3584;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRLabelFound;
            }
        }

        // If no label is found, exit with error code 4
        exit(1);
        BRLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brn")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRn opcode in decimal
        convertTemp = 2048;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRnLabelFound;
            }
        }

        // If no label is found, exit with error code 4
        exit(1);
        BRnLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brz")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRz opcode in decimal
        convertTemp = 1024;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRzLabelFound;
            }
        }


        // If no label is found, exit with error code 4
        exit(1);
        BRzLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brp")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRp opcode in decimal
        convertTemp = 512;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRpLabelFound;
            }
        }
        // If no label is found, exit with error code 4
        exit(1);
        BRpLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brzp")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRzp opcode in decimal
        convertTemp = 1536;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRzpLabelFound;
            }
        }

        // If no label is found, exit with error code 1
        exit(1);
        BRzpLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brnp")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRnp opcode in decimal
        convertTemp = 2560;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRnpLabelFound;
            }
        }

        // If no label is found, exit with error code 4
        exit(1);
        BRnpLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brnz")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRnz opcode in decimal
        convertTemp = 3072;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRnzLabelFound;
            }
        }

        // If no label is found, exit with error code 4
        exit(1);
        BRnzLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "brnzp")==0)
    {
        int size = strlen(pArg1);
        if(pArg1[size-1]=='\r')
            pArg1[size-1]='\0';
        // BRnzp opcode in decimal
        convertTemp = 3584;
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);

        for (int i = 0; i < trac; i++)
        {
            if (strcmp(symbolTable[i].label, pArg1) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto BRnzpLabelFound;
            }
        }



        // If no label is found, exit with error code 4
        exit(1);
        BRnzpLabelFound:

        // Converts labelAddress into offset value from PC
        labelAddress = (labelAddress - count);

        // If additional arguments, exit with error code 4
        if (strcmp(pArg2, "") !=0)
        {
            exit(4);
        }

        // If labelAddress offset from count is larger than 255, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (512 + labelAddress);
        }

        return (convertTemp + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "ldb")==0)
    {
        // If 4 arguments, exit with error code 4
        if (strcmp(pArg4, "") !=0)
        {
            exit(4);
        }
        int size = strlen(pArg3);
        if(pArg3[size-1]=='\r')
            pArg3[size-1]='\0';
        // LDB opcode in decimal
        convertTemp = 8192;

        // Fill in value of direct register for LDB
        int ldbDR;
        if (strcmp(pArg1, "r0")==0)
        {
            ldbDR = 0;
        }
        else if (strcmp(pArg1, "r1")==0)
        {
            ldbDR = 512;
        }
        else if (strcmp(pArg1, "r2")==0)
        {
            ldbDR = 1024;
        }
        else if (strcmp(pArg1, "r3")==0)
        {
            ldbDR = 1536;
        }
        else if (strcmp(pArg1, "r4")==0)
        {
            ldbDR = 2048;
        }
        else if (strcmp(pArg1, "r5")==0)
        {
            ldbDR = 2560;
        }
        else if (strcmp(pArg1, "r6")==0)
        {
            ldbDR = 3072;
        }
        else if (strcmp(pArg1, "r7")==0)
        {
            ldbDR = 3584;
        }
        else
        {
            // If no direct register found, exit with error code 4
            exit(4);
        }

        // Fill in value of base register for LDB
        int ldbBaseR;
        if (strcmp(pArg2, "r0")==0)
        {
            ldbBaseR = 0;
        }
        else if (strcmp(pArg2, "r1")==0)
        {
            ldbBaseR = 64;
        }
        else if (strcmp(pArg2, "r2")==0)
        {
            ldbBaseR = 128;
        }
        else if (strcmp(pArg2, "r3")==0)
        {
            ldbBaseR = 192;
        }
        else if (strcmp(pArg2, "r4")==0)
        {
            ldbBaseR = 256;
        }
        else if (strcmp(pArg2, "r5")==0)
        {
            ldbBaseR = 320;
        }
        else if (strcmp(pArg2, "r6")==0)
        {
            ldbBaseR = 384;
        }
        else if (strcmp(pArg2, "r7")==0)
        {
            ldbBaseR = 448;
        }
        else
        {
            // If no base register found, exit with error code 4
            exit(4);
        }

        // Fill in value of base offset
        for (int i = 0; i < trac; i++)
        {
            // Checks if the base offset is a label
            if (strcmp(symbolTable[i].label, pArg3) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto LDBLabelFound;
            }
        }
        // Checks if base offset is a valid number
        labelAddress = toNumInt(pArg3);
        goto LDBOffsetFound;

        LDBLabelFound:
        // Convert the label address into a direct offset
        labelAddress = (labelAddress - count);
        LDBOffsetFound:
        // If base offset is too large, exit with error code 3
        if (labelAddress > 31 || labelAddress < -32)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (64 + labelAddress);
        }

        // Return sum of all calculated pieces
        return (convertTemp + ldbDR + ldbBaseR + labelAddress);
    }
    else if(strcmp(pOpcode, "ldw")==0)
    {
        // If 4 arguments, exit with error code 4
        if (strcmp(pArg4, "") !=0)
        {
            exit(4);
        }

        // LDW opcode in decimal
        convertTemp = 24576;

        // Fill in value of direct register for LDW
        int ldwDR;
        if (strcmp(pArg1, "r0")==0)
        {
            ldwDR = 0;
        }
        else if (strcmp(pArg1, "r1")==0)
        {
            ldwDR = 512;
        }
        else if (strcmp(pArg1, "r2")==0)
        {
            ldwDR = 1024;
        }
        else if (strcmp(pArg1, "r3")==0)
        {
            ldwDR = 1536;
        }
        else if (strcmp(pArg1, "r4")==0)
        {
            ldwDR = 2048;
        }
        else if (strcmp(pArg1, "r5")==0)
        {
            ldwDR = 2560;
        }
        else if (strcmp(pArg1, "r6")==0)
        {
            ldwDR = 3072;
        }
        else if (strcmp(pArg1, "r7")==0)
        {
            ldwDR = 3584;
        }
        else
        {
            // If no direct register found, exit with error code 4
            exit(4);
        }

        // Fill in value of base register for LDW
        int ldwBaseR;
        if (strcmp(pArg2, "r0")==0)
        {
            ldwBaseR = 0;
        }
        else if (strcmp(pArg2, "r1")==0)
        {
            ldwBaseR = 64;
        }
        else if (strcmp(pArg2, "r2")==0)
        {
            ldwBaseR = 128;
        }
        else if (strcmp(pArg2, "r3")==0)
        {
            ldwBaseR = 192;
        }
        else if (strcmp(pArg2, "r4")==0)
        {
            ldwBaseR = 256;
        }
        else if (strcmp(pArg2, "r5")==0)
        {
            ldwBaseR = 320;
        }
        else if (strcmp(pArg2, "r6")==0)
        {
            ldwBaseR = 384;
        }
        else if (strcmp(pArg2, "r7")==0)
        {
            ldwBaseR = 448;
        }
        else
        {
            // If no base register found, exit with error code 4
            exit(4);
        }
        int size = strlen(pArg3);
        if(pArg3[size-1]=='\r')
            pArg3[size-1]='\0';
        // Fill in value of base offset
        for (int i = 0; i < trac; i++)
        {
            // Checks if the base offset is a label
            if (strcmp(symbolTable[i].label, pArg3) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto LDWLabelFound;
            }
        }
        // Checks if base offset is a valid number
        labelAddress = toNumInt(pArg3);
        goto LDWOffsetFound;

        LDWLabelFound:
        // Convert the label address into a direct offset
        labelAddress = (labelAddress - count);
        LDWOffsetFound:
        // If base offset is too large, exit with error code 3
        if (labelAddress > 31 || labelAddress < -32)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (64 + labelAddress);
        }

        // Return sum of all calculated pieces
        return (convertTemp + ldwDR + ldwBaseR + labelAddress);
    }
    else if(strcmp(pOpcode, "lea")==0)
    {
        // If 3 arguments, exit with error code 4
        if (strcmp(pArg3,"")!=0)
        {
            exit(4);
        }
        int chks = strlen(pArg2);
        if(pArg2[chks-1]=='\r')
            pArg2[chks-1]='\0';
        // LEA opcode in decimal
        convertTemp = 57344;

        // Fill in value of direct register for LEA
        int leaDR;
        if (strcmp(pArg1, "r0")==0)
        {
            leaDR = 0;
        }
        else if (strcmp(pArg1, "r1")==0)
        {
            leaDR = 512;
        }
        else if (strcmp(pArg1, "r2")==0)
        {
            leaDR = 1024;
        }
        else if (strcmp(pArg1, "r3")==0)
        {
            leaDR = 1536;
        }
        else if (strcmp(pArg1, "r4")==0)
        {
            leaDR = 2048;
        }
        else if (strcmp(pArg1, "r5")==0)
        {
            leaDR = 2560;
        }
        else if (strcmp(pArg1, "r6")==0)
        {
            leaDR = 3072;
        }
        else if (strcmp(pArg1, "r7")==0)
        {
            leaDR = 3584;
        }
        else
        {
            // If no direct register found, exit with error code 4
            exit(4);
        }
        if(pArg2[0]=='x' || pArg2[0]=='#')
            exit(4);
        // Fill in value of PC offset
        for (int i = 0; i < trac; i++)
        {
            // Checks if the base offset is a label
            if (strcmp(symbolTable[i].label, pArg2) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto LEALabelFound;
            }
        }
        // if not a valid label
        exit(1);

        LEALabelFound:
        // Convert the label address into a direct offset
        labelAddress = (labelAddress - count);

        // If base offset is too large, exit with error code 3
        if (labelAddress > 255 || labelAddress < -256)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (64 + labelAddress);
        }

        // Return sum of all calculated pieces
        return (convertTemp + leaDR + (labelAddress - 1));
    }
    else if(strcmp(pOpcode, "stb")==0)
    {
        // If 4 arguments, exit with error code 4
        if (strcmp(pArg4, "") !=0)
        {
            exit(4);
        }
        int size = strlen(pArg3);
        if(pArg3[size-1]=='\r')
            pArg3[size-1]='\0';
        // STB opcode in decimal
        convertTemp = 12288;

        // Fill in value of storage register for STB
        int stbSR;
        if (strcmp(pArg1, "r0")==0)
        {
            stbSR = 0;
        }
        else if (strcmp(pArg1, "r1")==0)
        {
            stbSR = 512;
        }
        else if (strcmp(pArg1, "r2")==0)
        {
            stbSR = 1024;
        }
        else if (strcmp(pArg1, "r3")==0)
        {
            stbSR = 1536;
        }
        else if (strcmp(pArg1, "r4")==0)
        {
            stbSR = 2048;
        }
        else if (strcmp(pArg1, "r5")==0)
        {
            stbSR = 2560;
        }
        else if (strcmp(pArg1, "r6")==0)
        {
            stbSR = 3072;
        }
        else if (strcmp(pArg1, "r7")==0)
        {
            stbSR = 3584;
        }
        else
        {
            // If no storage register found, exit with error code 4
            exit(4);
        }

        // Fill in value of base register for STB
        int stbBaseR;
        if (strcmp(pArg2, "r0")==0)
        {
            stbBaseR = 0;
        }
        else if (strcmp(pArg2, "r1")==0)
        {
            stbBaseR = 64;
        }
        else if (strcmp(pArg2, "r2")==0)
        {
            stbBaseR = 128;
        }
        else if (strcmp(pArg2, "r3")==0)
        {
            stbBaseR = 192;
        }
        else if (strcmp(pArg2, "r4")==0)
        {
            stbBaseR = 256;
        }
        else if (strcmp(pArg2, "r5")==0)
        {
            stbBaseR = 320;
        }
        else if (strcmp(pArg2, "r6")==0)
        {
            stbBaseR = 384;
        }
        else if (strcmp(pArg2, "r7")==0)
        {
            stbBaseR = 448;
        }
        else
        {
            // If no base register found, exit with error code 4
            exit(4);
        }

        // Fill in value of base offset
        for (int i = 0; i < trac; i++)
        {
            // Checks if the base offset is a label
            if (strcmp(symbolTable[i].label, pArg3) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto STBLabelFound;
            }
        }
        // Checks if base offset is a valid number
        labelAddress = toNumInt(pArg3);
        goto STBOffsetFound;

        STBLabelFound:
        // Convert the label address into a direct offset
        labelAddress = (labelAddress - count);
        STBOffsetFound:
        // If base offset is too large, exit with error code 3
        if (labelAddress > 31 || labelAddress < -32)
        {
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (64 + labelAddress);
        }

        // Return sum of all calculated pieces
        return (convertTemp + stbSR + stbBaseR + labelAddress);
    }
    else if(strcmp(pOpcode, "stw")==0)
    {
        // If 4 arguments, exit with error code 4
        if (strcmp(pArg4, "") !=0)
        {
            printf( "convertInt Error: unexpected operand, %s\n", pArg4);
            exit(4);
        }
        int size = strlen(pArg3);
        if(pArg3[size-1]=='\r')
            pArg3[size-1]='\0';
        // STW opcode in decimal
        convertTemp = 28672;

        // Fill in value of storage register for STW
        int stwSR;
        if (strcmp(pArg1, "r0")==0)
        {
            stwSR = 0;
        }
        else if (strcmp(pArg1, "r1")==0)
        {
            stwSR = 512;
        }
        else if (strcmp(pArg1, "r2")==0)
        {
            stwSR = 1024;
        }
        else if (strcmp(pArg1, "r3")==0)
        {
            stwSR = 1536;
        }
        else if (strcmp(pArg1, "r4")==0)
        {
            stwSR = 2048;
        }
        else if (strcmp(pArg1, "r5")==0)
        {
            stwSR = 2560;
        }
        else if (strcmp(pArg1, "r6")==0)
        {
            stwSR = 3072;
        }
        else if (strcmp(pArg1, "r7")==0)
        {
            stwSR = 3584;
        }
        else
        {
            // If no storage register found, exit with error code 4
            printf( "convertInt Error: invalid operand, %s\n", pArg1);
            exit(4);
        }

        // Fill in value of base register for STW
        int stwBaseR;
        if (strcmp(pArg2, "r0")==0)
        {
            stwBaseR = 0;
        }
        else if (strcmp(pArg2, "r1")==0)
        {
            stwBaseR = 64;
        }
        else if (strcmp(pArg2, "r2")==0)
        {
            stwBaseR = 128;
        }
        else if (strcmp(pArg2, "r3")==0)
        {
            stwBaseR = 192;
        }
        else if (strcmp(pArg2, "r4")==0)
        {
            stwBaseR = 256;
        }
        else if (strcmp(pArg2, "r5")==0)
        {
            stwBaseR = 320;
        }
        else if (strcmp(pArg2, "r6")==0)
        {
            stwBaseR = 384;
        }
        else if (strcmp(pArg2, "r7")==0)
        {
            stwBaseR = 448;
        }
        else
        {
            // If no base register found, exit with error code 4
            printf( "convertInt Error: invalid operand, %s\n", pArg2);
            exit(4);
        }

        // Fill in value of base offset
        for (int i = 0; i < trac; i++)
        {
            // Checks if the base offset is a label
            if (strcmp(symbolTable[i].label, pArg3) ==0)
            {
                labelAddress = symbolTable[i].address;
                goto STWLabelFound;
            }
        }
        // Checks if base offset is a valid number
        labelAddress = toNumInt(pArg3);
        goto STWOffsetFound;

        STWLabelFound:
        // Convert the label address into a direct offset
        labelAddress = (labelAddress - count);
        STWOffsetFound:
        // If base offset is too large, exit with error code 3
        if (labelAddress > 31 || labelAddress < -32)
        {
            printf( "convertInt Error: invalid offset, %s\n", labelAddress);
            exit(3);
        }

        // If labelAddress offset is negative, flip the bits for 2s complement
        if (labelAddress < 0)
        {
            labelAddress = (64 + labelAddress);
        }

        // Return sum of all calculated pieces
        return (convertTemp + stwSR + stwBaseR + labelAddress);
    }
    else if(strcmp(pOpcode, "halt")==0)
    {
        // If additional arguments, exit with error code 4
        if (strcmp(pArg1, "") !=0)
        {
            printf( "convertInt Error: unexpected operand, %s\n", pArg1);
            exit(4);
        }

        return 0xF025;
    }
        // If no valid opcode is found, exit with error code 2
    else if(strcmp(pOpcode,".orig")!=0  && strcmp(pOpcode, ".end") != 0 && strcmp(pOpcode, ".fill") != 0 )
    {
        printf( "convertInt Error: invalid opcode, %s\n", pOpcode);
        exit(2);
    }
    else
    {
        printf( "convertInt Error: invalid opcode, %s\n", pOpcode);
        exit(2);
    }
}

void parse(FILE* infile)
{
    char* t = (char*) malloc(7);
    createsymb(infile);
    rewind(infile);
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
            *lArg2, *lArg3, *lArg4;
    count = orig;
    int lRet;
    do
    {
        lRet = readAndParse1( infile, lLine, &lLabel,
                              &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            if(origflag == 1) {
                if (strcmp(lOpcode, ".orig") != 0 && strcmp(lOpcode, ".end") != 0 && strcmp(lOpcode, ".fill") != 0) {
                    int a7;
                    if (strcmp(lOpcode, "add") == 0 || strcmp(lOpcode, "and") == 0 || strcmp(lOpcode, "jmp") == 0 ||
                        strcmp(lOpcode, "ret") == 0 || strcmp(lOpcode, "jsrr") == 0 || strcmp(lOpcode, "not") == 0 ||
                        strcmp(lOpcode, "rti") == 0 || strcmp(lOpcode, "nop") == 0 || strcmp(lOpcode, "trap") == 0 ||
                        strcmp(lOpcode, "xor") == 0 || strcmp(lOpcode, "lshf") == 0 || strcmp(lOpcode, "rshfa") == 0 ||
                        strcmp(lOpcode, "rshfl") == 0) {
                        char* a6 ;
                        a6 = convert(lOpcode, lArg1, lArg2, lArg3, lArg4 , t);
                        if (a6[0] == '0')
                        {
                            for(int i = 0; i< strlen(a6); i++)
                            {
                                a6[i]=a6[i+1];
                            }
                        }
                        a7 = toNum(a6);
                        fprintf( outfile, "0x%.4X\n", a7 );
                    } else if(strcmp(lOpcode, "br") == 0 || strcmp(lOpcode, "brn") == 0 || strcmp(lOpcode, "brz") == 0 ||
                              strcmp(lOpcode, "brp") == 0 || strcmp(lOpcode, "brnz") == 0 || strcmp(lOpcode, "brnp") == 0 ||
                              strcmp(lOpcode, "brnzp") == 0 || strcmp(lOpcode, "brnp") == 0 || strcmp(lOpcode, "jsr") == 0 ||
                              strcmp(lOpcode, "ldb") == 0 || strcmp(lOpcode, "ldw") == 0 || strcmp(lOpcode, "lea") == 0 ||
                              strcmp(lOpcode, "stb") == 0 ||strcmp(lOpcode, "stw") == 0  || strcmp(lOpcode, "halt") == 0 ) {
                        a7 = convertInt(lOpcode, lArg1, lArg2, lArg3, lArg4);
                        fprintf( outfile, "0x%.4X\n", a7 );
                    }
                    count++;
                    if (count > 0xFFFF)
                    {
                        printf( "parse Error: PC is higher than 0xFFFF, PC = %d\n", count);
                        exit(3);
                    }
                }
                else if(strcmp(lOpcode,".fill")==0){
                    if(strcmp(lArg2,"")!=0)
                    {
                        printf( "parse Error: unexpected operand, %s\n",lArg2);
                        exit(4);
                    }
                    if(strcmp(lArg1,"")==0)
                    {
                        printf( "parse Error: missing operand, %s\n", lArg1);
                        exit(4);
                    }
                    int chks = strlen(lArg1);
                    if(lArg1[chks-1]=='\r')
                        lArg1[chks-1]='\0';
                    int a8 = toNum(lArg1);
                    fprintf( outfile, "0x%.4X\n", a8 );
                }
                else
                {
                    exit(2);
                }
            }
        }
    } while( lRet != DONE );
    free(t);
}


int main() {

    // Open source file
    infile = fopen("test.txt", "r");

    // Open output file
    outfile = fopen("output.txt", "w");

    // If no file exists, exit with error code 4
    if (!infile) {
        printf("Error: Cannot open file \n");
        exit(4);
    }
    if (!outfile) {
        printf("Error: Cannot open file \n");
        exit(4);
    }

    // Parse through the source file and print to output file
    parse(infile);

    // Close source and output file
    fclose(infile);
    fclose(outfile);

    printf("Output is printed");

    return 0;
}
