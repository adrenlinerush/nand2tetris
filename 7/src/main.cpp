#include <writer.h>
#include <parser.h>
#include <regex>
#include <string>

int main (int argc, char *argv[])
{
  //printf ("Arguments passed: %i\n", argc);
  if (argc < 2) { printf("Must pass .vm file!\n"); return -1; }
  std::string vmFilename = argv[1];
  printf ("Processing file: %s\n", vmFilename.c_str());
  std::string filePrefix = std::regex_replace(vmFilename, std::regex(".vm"), "");
  printf ("Binary file will output as: %s.asm\n", filePrefix.c_str());

  Parser* vmParser = new Parser(vmFilename);
  Writer* asmWriter = new Writer(filePrefix);
  while (vmParser->hasMoreLines()) {
	  vmParser->advance();
	  printf ("Command type: %i\n",vmParser->getCommandType());
	  switch (vmParser->getCommandType()){
		  case c::C_ARITHMETIC:
			  printf ("C_ARITHMETIC\n");
			  asmWriter->writeArithmetic(vmParser->getArg1());
			  break;
		  case c::C_PUSH:
		  case c::C_POP:
			  printf ("C_PUSH or C_POP\n");
			  asmWriter->writePushPop(vmParser->getCommandType(), 
					  vmParser->getArg1(), vmParser->getArg2());
			  break;

			
	  }
  }
  asmWriter->close();
}
