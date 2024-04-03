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

	private:
		std::ofstream asmFile;
		std::string prefix;
		std::vector<std::string> subs;
		int addr;
		const std::string M_RETURN = "@R13";
		const std::string M_SEGP = "@R14";

		void initAsm();
		void writeCommand(std::string command);
                void push(std::string index); 
		void writeComparison(std::string cmd);
		void incrementPointer(std::string p);
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
