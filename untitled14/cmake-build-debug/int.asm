	.ORIG x1200
	ADD R6,R6,#-2
	STW R0,R6,#0
	LEA R0,A
	LDW R0,R0,#0
	LDB R5,R0,#0
	ADD R5,R5,#1
	STB R5,R0,#0
	LDW R0,R6,#0
	ADD R6,R6,#2
	RTI
	TRAP x25
A	.FILL x4000
	.END