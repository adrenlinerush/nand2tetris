#include <writer.h>

Writer::Writer(std::string file){
	prefix = file;
	if (file.find("/") != std::string::npos) {
		int start=file.find_last_of("/");
		prefix = file.substr(start+1);
	}
	addr = 0;
	std::string filename = file+".asm";
	printf("Openning asm file.\n");
	asmFile.open (filename);
	initAsm();
}

void Writer::writeArithmetic(std::string command){
	if (command == "eq" || command == "gt" || command == "lt") {
		//Comparison Commands
		writeComparison(command);
		return;
	}

	popValue("SP");
	//Logical Commands
	if (command == "neg") { writeCommand("M=-M"); incrementPointer("SP");  return; }
	if (command == "not") { writeCommand("M=!M"); incrementPointer("SP"); return; }


	//Arithmetic Commands
	writeCommand("D=M");
	popValue("SP");
        if (command == "add") { writeCommand("M=D+M"); incrementPointer("SP"); return; }
	if (command == "sub") { writeCommand("M=M-D"); incrementPointer("SP"); return; }
        if (command == "and") { writeCommand("M=D&M"); incrementPointer("SP"); return; }
	if (command == "or") { writeCommand("M=D|M"); incrementPointer("SP"); return; }

}

void Writer::writeComparison(std::string cmd) {
	//Set return address
	int returnAddr = addr+16; //current address + number of commands
	writeCommand("@"+itoa(returnAddr)); //2
	writeCommand("D=A"); //3
	writeCommand(M_RETURN); //4
	writeCommand("M=D"); //5

	//Pop last 2 values and compare
	popValue("SP"); //+3 Commands 6..7..8
	writeCommand("D=M"); //9
	popValue("SP"); //+3 Commands 10..11..12
	writeCommand("D=M-D"); //12
	writeCommand("@SETTRUE"); //13
	if (cmd == "gt") { writeCommand("D;JGT"); } //14
	if (cmd == "lt") { writeCommand("D;JLT"); }
	if (cmd == "eq") { writeCommand("D;JEQ"); }
	writeCommand("@SETFALSE"); //15
	writeCommand("0;JMP");  //16
	incrementPointer("SP"); //<-- where I want to return
}

void Writer::incrementPointer(std::string p) {
	writeCommand("@"+p);
	writeCommand("M=M+1");
}

void Writer::popValue(std::string p) {
	writeCommand("@"+p);
	writeCommand("M=M-1");
	writeCommand("A=M");
}


void Writer::writePushPop(int command, std::string segment, std::string index){
	printf("Command: %i\nSegment: %s\nIndex: %s\nROM Addr: %i\n",command, segment.c_str(), index.c_str(), addr);
	if (command == c::C_PUSH) {
		if (segment == "constant") { pushConstant(index); }
		if (segment == "local") { pushSegment("LCL", index); }
		if (segment == "argument") { pushSegment("ARG", index); }
		if (segment == "this") { pushSegment("THIS", index); }
		if (segment == "that") { pushSegment("THAT", index); }
		if (segment == "temp") { pushVar("R"+itoa(5+stoi(index))); }
		if (segment == "static") { pushVar(prefix+"."+index); }
		if (segment == "pointer") { pushVar("R"+itoa(3+stoi(index))); }
	} else if (command == c::C_POP) {
		if (segment == "local") { popSegment("LCL", index); }
		if (segment == "argument") { popSegment("ARG", index); }
		if (segment == "this") { popSegment("THIS", index); }
		if (segment == "that") { popSegment("THAT", index); }
		if (segment == "temp") { popVar("R"+itoa(5+stoi(index))); }
		if (segment == "static") { popVar(prefix+"."+index); }
		if (segment == "pointer") { popVar("R"+itoa(3+stoi(index))); }
	}
}

void Writer::close(){
	writeCommand("@END");
	writeCommand("0;JMP");

	//Write out subs
	

	//Initialize True Value 
	writeCommand("(SETTRUE)");
	writeCommand("@SP");
	writeCommand("A=M");
	writeCommand("M=-1");
	writeCommand(M_RETURN);
	writeCommand("A=M");
	writeCommand("0;JMP");

	//Initialize False Value
	writeCommand("(SETFALSE)");
	writeCommand("@SP");
	writeCommand("A=M");
	writeCommand("M=0");
	writeCommand(M_RETURN);
	writeCommand("A=M");
	writeCommand("0;JMP");
	
	//Good practice to end with infinite loop
	writeCommand("(END)");
	writeCommand("@END");
	writeCommand("0;JMP");

	//Close the file
	printf("Closing asm file.\n");
	asmFile.close();
}

void Writer::initAsm(){
	//Initialize Stack Pointer and Base Adress of segments
	setVReg("256", "SP");
	setVReg("300", "LCL");
	setVReg("400", "ARG");
	setVReg("3000", "THIS");
	setVReg("3010", "THAT");
}

void Writer::setSegAddr(std::string segment, std::string index) {
	writeCommand("@" + index);
	writeCommand("D=A");
	writeCommand("@" + segment);
	writeCommand("D=D+M");
}

void Writer::popSegment(std::string segment, std::string index) {
	setSegAddr(segment, index);
	writeCommand(M_SEGP);
	writeCommand("M=D");
	popValue("SP");
	writeCommand("D=M");
	writeCommand(M_SEGP);
	writeCommand("A=M");
	writeCommand("M=D");
}

void Writer::pushSegment(std::string segment, std::string index) { 
	setSegAddr(segment, index);
	writeCommand("A=D");
	writeCommand("D=M");
	writeCommand("@SP");
	writeCommand("A=M");
	writeCommand("M=D");
	incrementPointer("SP");
}

void Writer::setVReg(std::string value, std::string vreg){
	writeCommand("@" + value);
	writeCommand("D=A");
	writeCommand("@" + vreg);
	writeCommand("M=D");
}

void Writer::pushConstant(std::string index){
	writeCommand("@"+index);
	writeCommand("D=A");
	writeCommand("@SP");
	writeCommand("A=M");
	writeCommand("M=D");
	incrementPointer("SP");
}

void Writer::popVar(std::string name){
	popValue("SP");
	writeCommand("D=M");
	writeCommand("@"+name);
	writeCommand("M=D");
}

void Writer::pushVar(std::string name) {
	writeCommand("@"+name);
	writeCommand("D=M");
	writeCommand("@SP");
	writeCommand("A=M");
	writeCommand("M=D");
	incrementPointer("SP");
}

void Writer::writeCommand(std::string command)
{
	printf("%s\n",command.c_str());
	asmFile << command << "\n";  
	addr++;
}
