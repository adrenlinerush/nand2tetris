// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

  @cnt
  M=0
  @R2
  M=0
(LOOP)
  //Check if I'm Done
  @cnt
  D=M 
  @R1
  D=M-D
  @END
  D;JEQ 

  //Perform Additon of Multiplication
  @R0
  D=M
  @R2
  M=M+D
  
  //Increase my counter
  @cnt
  M=M+1

  //Go Back to the beginning
  @LOOP
  0;JMP

(END)
  @END
  0;JMP
