	.ORIG x3000
	
	LEA R4,SUMVALS
	LDW R0,R4,#0
	LDB R1,R0,#0
	LDB R2,R0,#0
	LEA R4,MASK1
	LDW R0,R4,#0
	AND R1,R1,R0
	AND R0, R1, x8 ; Store the 2s complement value in R0
	BRnz NotNeg1 ; If number is not 1, branch

	LEA R4, MASK2 ; Load R4 with address of x00F0
	LDW R0, R4, #0 ; Load R0 with x00F0
	ADD R1, R1, R0 ; Sets bits 4:7 to 1 for 2s complement arithmetic

NotNeg1	LEA R4,MASK2
	LDW R0,R4,#0
	AND R2,R2,R0
	RSHFL R2,R2,#4
	AND R0, R2, x8 ; Store the 2s complement value of R0
	BRnz NotNeg2 ; If number is not 1, branch

	LEA R4, MASK2
	LDW R0, R4, #0
	ADD R2, R2, R0 ; Sets bits 4:7 to 1 for 2s complement arithmetic

NotNeg2	ADD R1,R1,R2
	LEA R4,RESULTPTR
	LDW R0,R4,#0
	STB R1,R0,#0
SUMVALS .FILL x3050
MASK1 .FILL x000F
MASK2 .FILL x00F0
RESULTPTR .FILL x3051
	.END