     .ORIG X1200
     ADD R6,R6,#-2
     STW R0,R6,#0
     ADD R6,R6,#-2
     STW R1,R6,#0
     ADD R6,R6,#-2
     STW R2,R6,#0
     ADD R6,R6,#-2
     STW R3,R6,#0
     LEA R3,C
     LDW R3,R3,#0
     LEA R0,A
     LDW R0,R0,#0
     LEA R1,B
     LDW R1,R1,#0
LOOP LDW R2,R0,#0
     AND R2,R2,R3
     STW R2,R0,#0
     ADD R0,R0,#2
     ADD R1,R1,#-1
     BRP LOOP
     LDW R3,R6,#0
     ADD R6,R6,#2
     LDW R2,R6,#0
     ADD R6,R6,#2
     LDW R1 R6,#0
     ADD R6,R6,#2
     LDW R0,R6,#0
     ADD R6,R6,#2
     RTI
     HALT
A    .FILL X1000
B    .FILL #128
C    .FILL XFFFE
     .END