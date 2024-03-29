#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <stdio.h>
#include <regex>
#include <iostream>

// trim from start (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

std::map<std::string, int> symbolTable;
std::map<int, std::string> commands;
std::map<int, std::string> instructions;
int symbolCnt = 16;


std::string parseAInstruction(std::string command);
std::string parseCInstruction(std::string command);
std::string parseComputation(std::string computation);
std::string parseDestination(std::string dest);
std::string parseJump(std::string cond);

void prepopulateSymbols()
{
  symbolTable["R0"]=0;
  symbolTable["R1"]=1;
  symbolTable["R2"]=2;
  symbolTable["R3"]=3;
  symbolTable["R4"]=4;
  symbolTable["R5"]=5;
  symbolTable["R6"]=6;
  symbolTable["R7"]=7;
  symbolTable["R8"]=8;
  symbolTable["R9"]=9;
  symbolTable["R10"]=10;
  symbolTable["R11"]=11;
  symbolTable["R12"]=12;
  symbolTable["R13"]=13;
  symbolTable["R14"]=14;
  symbolTable["R15"]=15;
  symbolTable["SP"]=0;
  symbolTable["LCL"]=1;
  symbolTable["ARG"]=2;
  symbolTable["THIS"]=3;
  symbolTable["THAT"]=4;
  symbolTable["SCREEN"]=16384;
  symbolTable["KEYBOARD"]=24576;
}


//read into symbolTable and commands maps
void readFile(std::string asmfile)
{
  printf ("Reading file...\n");
  int i = 0;
  std::ifstream asmcode(asmfile);
  std::string line;
  while (std::getline(asmcode, line))
  {
    printf ("Processing line: %s\n", line.c_str());
    trim(line);
    if ((line[0] == '/') && (line[1] == '/')) {
      printf ("Comment...\n");
    } else if ((line.find("@") != std::string::npos) || (line.find("=") != std::string::npos || (line.find(";") != std::string::npos))) {
      printf ("Processing as command...\n");
      unsigned comment = line.find("//");
      if (comment != std::string::npos)
      {
        line=line.substr(0,comment);
      } 
      trim(line);
      std::cout << line << "\n";
      commands[i]=line;  
      i++;  
    } else if ((line.find("(") != std::string::npos) && (line.find(")") != std::string::npos)) {
      printf ("Processing pre-defined symbol.\n");
      unsigned start = line.find("(");
      unsigned end = line.find(")");
      std::string symbol = line.substr (start+1, end-start-1);
      printf ("%s\n",symbol.c_str());
      symbolTable[symbol]=i;
    }
  }
  asmcode.close();
}

//parse individual commands
void parseCommands()
{
  std::string binary;
  int n = commands.size();
  for (int c =0; c < n; c++)
  {
    std::string command=commands[c];
    if (command[0] == '@')
    {
      //A instruction
      binary = parseAInstruction(command);
    } else {
      //C instruction
      binary = parseCInstruction(command);
    }
    instructions[c] = binary;
  }  
}

//create machine code for A inst
std::string parseAInstruction(std::string command)
{
  std::string addr = command.substr(1);
  char* e;
  long iaddr = strtol(addr.c_str(), &e, 10);
  if (*e) {
    // lookup symbol
    printf("Looking up symbol: %s\n",addr.c_str());
    if (symbolTable.find(addr) == symbolTable.end()) {
      printf("Symbol not found.\nAssigning to next ram position: %i\n", symbolCnt);
      symbolTable[addr] = symbolCnt;
      iaddr = symbolCnt;
      symbolCnt++;
    } else {
      printf("Symbol found.\n");
      iaddr = symbolTable[addr];
    }
  } 
  std::string instruction = "0" + std::bitset<15>(iaddr).to_string();
  return instruction;
}


//create machine code for C instruction by separating and calling section fucnctions
std::string parseCInstruction(std::string command)
{
  printf("parseCInstruction called with command: %s\n", command.c_str());
  std::string instruction = "111";
  unsigned s = command.find("=");
  unsigned j = command.find(";");
  //printf ("Value of s: %s\n",s);
  if ((s != -1) && (j == -1)) {
    printf("Not a jump.\n");
    std::string dest = command.substr(0,s);
    printf("Destination: %s\n",dest.c_str());
    std::string destbin = parseDestination(dest);
    printf("Binary: %s\n",destbin.c_str());
    std::string comp = command.substr(s+1);
    printf("Computation: %s\n",comp.c_str());
    std::string compbin = parseComputation(comp);
    printf("Binary: %s\n",compbin.c_str());
    instruction+=compbin;
    instruction+=destbin;
    instruction+="000";
  } else {
    printf("Jump Command.\n");
    std::string comp = command.substr(0,j);
    printf("Computation: %s\n",comp.c_str());
    std::string compbin = parseComputation(comp);
    printf("Binary: %s\n",compbin.c_str());
    std::string cond = command.substr(j+1);
    printf("Jump Conditon: %s\n",cond.c_str());
    std::string jmp = parseJump(cond);
    printf("Binary: %s\n",jmp.c_str());
    instruction+=compbin;
    instruction+="000";
    instruction+=jmp;
  }
  return instruction;
}

//create 6 bits that gets sent to ALU and a bit
std::string parseComputation(std::string computation)
{
  if (computation == "0") { return "0101010"; }
  if (computation == "1") { return "0111111"; }
  if (computation == "-1") { return "0111010"; }
  if (computation == "D") { return "0001100"; }
  if (computation == "A") { return "0110000"; }
  if (computation == "!D") { return "0001101"; }
  if (computation == "!A") { return "0110001"; }
  if (computation == "-D") { return "0001111"; }
  if (computation == "-A") { return "0110011"; }
  if (computation == "D+1") { return "0011111"; }
  if (computation == "A+1") { return "0110111"; }
  if (computation == "D-1") { return "0001110"; }
  if (computation == "A-1") { return "0110010"; }
  if (computation == "D+A") { return "0000010"; }
  if (computation == "D-A") { return "0010011"; }
  if (computation == "A-D") { return "0000111"; }
  if (computation == "D&A") { return "0000000"; }
  if (computation == "D|A") { return "0010101"; }
  if (computation == "M") { return "1110000"; }
  if (computation == "!M") { return "1110001"; }
  if (computation == "-M") { return "1110011"; }
  if (computation == "M+1") { return "1110111"; }
  if (computation == "M-1") { return "1110010"; }
  if (computation == "D+M") { return "1000010"; }
  if (computation == "D-M") { return "1010011"; }
  if (computation == "M-D") { return "1000111"; }
  if (computation == "D&M") { return "1000000"; }
  if (computation == "D|M") { return "1010101"; }
  return "0000000";
}

//parse destination 3 bits
std::string parseDestination(std::string dest)
{
  std::string A = "0";
  std::string D = "0";
  std::string M = "0";
  if (dest.find("A") != std::string::npos) { A="1"; }
  if (dest.find("D") != std::string::npos) { D="1"; }
  if (dest.find("M") != std::string::npos) { M="1"; }
  return A+D+M;
}

//parse jump 3 bits
std::string parseJump(std::string cond)
{
  if (cond == "JGT") { return "001"; }
  if (cond == "JEQ") { return "010"; }
  if (cond == "JGE") { return "011"; }
  if (cond == "JLT") { return "100"; }
  if (cond == "JNE") { return "101"; }
  if (cond == "JLE") { return "110"; }
  if (cond == "JMP") { return "111"; } 
  return "000";
}


//write machine code to file
void writeHackFile(std::string filename)
{
  std::ofstream hack;
  hack.open (filename);
  int lines = instructions.size();
  for (int i=0; i < lines; i++) {
     hack << instructions[i] << "\n";
  }
  hack.close();
}


int main (int argc, char *argv[])
{
  std::string asmFilename = argv[1];
  //std::string asmFilename = "add/Add.asm";
  printf ("Processing file: %s\n", asmFilename.c_str());
  std::string hackFilename = std::regex_replace(asmFilename, std::regex("asm"), "hack"); 
  printf ("Binary file will output as: %s\n", hackFilename.c_str());

  prepopulateSymbols();
  readFile(asmFilename);
  parseCommands();
  writeHackFile(hackFilename);
}
