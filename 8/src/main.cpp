#include <writer.h>
#include <parser.h>
#include <regex>
#include <string>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

void processFile(std::string vmFilename, Writer* asmWriter) {
  printf ("Processing file: %s\n", vmFilename.c_str());
  Parser* vmParser = new Parser(vmFilename);
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
		  case c::C_LABEL:
			  asmWriter->writeLabel(vmParser->getArg1());
			  break;
		  case c::C_GOTO:
		  	  asmWriter->writeGoto(vmParser->getArg1());
		  	  break;
		  case c::C_IF:
		  	  asmWriter->writeIf(vmParser->getArg1());
		  	  break;
		  case c::C_FUNCTION:
		  	  asmWriter->writeFunction(vmParser->getArg1(), vmParser->getArg2());
		  	  break;
		  case c::C_RETURN:
		  	  asmWriter->writeReturn();
		          break;
		  case c::C_CALL:
		  	  asmWriter->writeCall(vmParser->getArg1(), vmParser->getArg2());
		  	  break;

			
	  }
  }
}

int main (int argc, char *argv[])
{
  if (argc < 2) { printf("Must pass .vm file!\n"); return -1; }
  std::string arg1 = argv[1];
  int argLen = arg1.length();
  std::string suffix = arg1.substr(argLen-3);
  std::string filePrefix = std::regex_replace(arg1, std::regex(".vm"), "");
  Writer* asmWriter = new Writer(filePrefix);
  printf ("Binary file will output as: %s.asm\n", filePrefix.c_str());
  if (fs::status(argv[1]).type() == fs::file_type::directory) {
	printf ("Directory...\n");
	for (const auto & file: fs::directory_iterator(arg1)) {
		std::string filepath = file.path();
		int fileLen = filepath.length();
		suffix = filepath.substr(fileLen-3);
		if (fs::status(file).type() == fs::file_type::regular && suffix == ".vm") {
			asmWriter->setFileName(filepath);
			processFile(filepath, asmWriter);
		}
	}
  } else if (fs::status(argv[1]).type() == fs::file_type::regular && suffix == ".vm") {
	printf ("File...\n");
	asmWriter->setFileName(arg1);
	processFile(arg1, asmWriter);
  } else {
  	printf ("Invalid Filename!\n");
	return -1;
  }
  asmWriter->close();
  return 0;
}
