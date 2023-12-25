        .ORIG X3000
        LEA R0,A
        LDW R0,R0,#0
        AND R1,R1,#0
        AND R5,R5,#0
        ADD R1,R1,#10
        ADD R1,R1,#10
LOOP    LDB R2,R0,#0
        LDB R3,R0,#1
        ADD R5,R5,R2
        ADD R5,R5,R3
        ADD R0,R0,#2
        ADD R1,R1,#-2
        BRP LOOP
        LEA R0,B
        LDW R0,R0,#0
        STW R5,R0,#0
        JMP R5
        HALT
A       .FILL XC000
B       .FILL XC014
        .END