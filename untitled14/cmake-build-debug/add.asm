	.ORIG X3000
	;ADD R7,R7,#1
	LEA R0,LABEL
	JSRR R0
;	LEA R0,A
;	LDW R0,R0,#0
;	AND R1,R1,#0
;	ADD R1,R1,#1
;	STW R1,R0,#0
;	AND R1,R1,#0
;	AND R2,R2,#0
;	AND R3,R3,#0
;	ADD R3,R3,#10
;	ADD R3,R3,#10
;	LEA R0,B
;	LDW R0,R0,#0
;LOOP	LDB R1,R0,#0
 ;       LDB R4,R0,#1
;	ADD R2,R2,R1
;	ADD R2,R2,R4
;	ADD R0,R0,#2
;	ADD R3,R3,#-2
;	BRP LOOP
;	LEA R0,C
;	LDW R0,R0,#0
;	STW R2,R0,#0
;	AND R4,R4,#0
;	STW R2,R4,#0
;	;LEA R4,F
	;LDW R4,R4,#0
	;STW R2,R4,#1	
	;.FILL XA000
	;.FILL XB000
	HALT
LABEL ADD R0,R0,#1
      RET
A	.FILL X4000
B	.FILL XC000
C	.FILL XC014
D	.FILL XFF00
E	.FILL X00FF
F	.FILL X3100
	.END