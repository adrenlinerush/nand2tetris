// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

  //Boolean to whether I'm fillinig or clearing screen

  @bf
  M=0
(RESET)
  @8192
  D=A
  @cnt
  M=D
  @SCREEN
  D=A
  @pixels
  M=D


(LOOP)

  //Check Keyboard, if keystroke jump to fill
  @KBD
  D=M
  @FILL
  D;JNE

  //Was I black and need to reset
  @bf
  D=M
  @bf
  M=0
  @RESET
  D;JGT

  //whitetime
  @pixels
  D=M
  A=D
  M=0

  @INC
  0;JMP
 
(FILL)
  //Was I white and need to reset
  @bf
  D=M
  @bf
  M=1
  @RESET
  D;JEQ

  //black time
  @pixels
  D=M
  A=D
  M=-1


(INC)
  @cnt
  D=M
  @LOOP
  D;JEQ
  @cnt
  M=M-1
  @pixels
  M=M+1

  @LOOP
  0;JMP

   



