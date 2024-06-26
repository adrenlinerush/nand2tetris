#ifndef WRITER_H
#define WRITER_H

#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <global.cpp>

class Writer {

	public:
		Writer(std::string file);

		void writeArithmetic(std::string command);
		void writePushPop(int command, std::string segment, std::string index);
		void close();
		void setFileName(std::string filename);
		void writeLabel(std::string label);
		void writeGoto(std::string label);
		void writeIf(std::string label);
		void writeFunction(std::string functionName, std::string nVars);
		void writeCall(std::string functionName, std::string nVars);
		void writeReturn();

	private:
		std::string vmFilename;
		std::ofstream asmFile;
		std::string prefix;
		std::vector<std::string> subs;
		int addr;
		int iCR;
		const std::string M_RETURN = "@R13";
		const std::string M_COPY = "@R14";
		const std::string M_FRAME = "@R15";

		void initAsm();
		void writeCommand(std::string command);
                void push(std::string index); 
		void writeComparison(std::string cmd);
		void incrementPointer(std::string p);
		void decrementPointer(std::string p);
		void popValue(std::string p);
		void popSegment(std::string segment, std::string index);
		void pushSegment(std::string segment, std::string index);
		void setSegAddr(std::string segment, std::string index);
		void popVar(std::string name);
		void pushVar(std::string name);
		void pushConstant(std::string index);
		void setVReg(std::string value, std::string vreg);
};
#endif
