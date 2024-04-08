#include <writer.h>

Writer::Writer(std::string file){
	prefix = file;
	if (file.find("/") != std::string::npos) {
		int start=file.find_last_of("/");
		prefix = file.substr(start+1);
	}
	addr = 0;
	iCR = 0;
	std::string filename = file+".asm";
	printf("Openning asm file.\n");
	asmFile.open (filename);
	initAsm();
}

void Writer::setFileName(std::string filename) {
	std::vector sFilename = split(filename, "/");
	std::string bFilename = sFilename.back();
	sFilename = split(bFilename, ".");
	vmFilename = sFilename.front();
}

void Writer::writeLabel(std::string label) {
	std::string command = "("+label+")";
	printf("%s\n",command.c_str());
	asmFile << command << "\n";  
}

void Writer::writeGoto(std::string label) {
	writeCommand("@"+label);
	writeCommand("0;JMP");
}

void Writer::writeIf(std::string label) {
	popValue("SP");
	writeCommand("D=M");
	writeCommand("@"+label);
	writeCommand("D;JNE");
}

void Writer::writeFunction(std::string functionName, std::string nVars) {
	writeLabel(functionName);
      
	for (int l = 0; l < stoi(nVars); l++) {
		setSegAddr("LCL", itoa(l));
		writeCommand("A=D");
		writeCommand("M=0");
		incrementPointer("SP");
	}
}

void Writer::writeCall(std::string functionName, std::string nVars) {
	//push return address
	writeCommand("@return"+itoa(iCR));
	writeCommand("D=A");
	writeCommand("@SP");
	writeCommand("A=M");
	writeCommand("M=D");
	incrementPointer("SP");
	
	//store off LCL, ARG, THIS, THAT
	for (int p = 1; p < 5; p++) {
		writeCommand("@" + itoa(p));
		writeCommand("D=M");
		writeCommand("@SP");
		writeCommand("A=M");
		writeCommand("M=D");
		incrementPointer("SP");
	}

	//Positions Arg
	
	//Push SP Value to the Stack
	writeCommand("@SP");
	writeCommand("D=M");
	writeCommand("A=M");
	writeCommand("M=D");
	incrementPointer("SP");
	
	//Subtract 5 and nVars from SP
	writePushPop(c::C_PUSH, "constant", "5");
	writeArithmetic("sub");
	writePushPop(c::C_PUSH, "constant", nVars);
	writeArithmetic("sub");

	//Pop Value to ARG
	popValue("SP");
	writeCommand("D=M");
	writeCommand("@ARG");
	writeCommand("M=D");

	//Set LCL to SP
	writeCommand("@SP");
	writeCommand("D=M");
	writeCommand("@LCL");
	writeCommand("M=D");
	
	//actual function call
	writeGoto(functionName);

	//inject return address label
	writeLabel("return"+itoa(iCR));
	iCR++;

}

void Writer::writeReturn() {
	
	//save Return Address

	writeCommand("@5");
	writeCommand("D=A");
	writeCommand("@LCL");
	writeCommand("D=M-D");
	writeCommand("A=D");
	writeCommand("D=M");
	writeCommand(M_RETURN);
	writeCommand("M=D");
	
	writePushPop(c::C_POP, "argument", "0");

	writeCommand("@ARG");
	writeCommand("D=M+1");
	writeCommand("@SP");
	writeCommand("M=D");

	writeCommand("@LCL");
	writeCommand("D=M");

	for (int p = 4; p > 0; p--) {
		writeCommand(M_FRAME);
		if (p == 4) { writeCommand("M=D-1"); }
		else { writeCommand("M=M-1"); }

		writeCommand("A=M");
		writeCommand("D=M");
		writeCommand("@" + itoa(p));
		writeCommand("M=D");
	}
	//get return addr
	writeCommand(M_RETURN);
	writeCommand("A=M");

	//return
	writeCommand("0;JMP");
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
	writeCommand("@BRET"+itoa(iCR));
	writeCommand("D=A"); 
	writeCommand(M_RETURN);
	writeCommand("M=D"); 

	//Pop last 2 values and compare
	popValue("SP");
	writeCommand("D=M"); 
	popValue("SP"); 
	writeCommand("D=M-D"); 
	writeCommand("@SETTRUE"); 
	if (cmd == "gt") { writeCommand("D;JGT"); } 
	if (cmd == "lt") { writeCommand("D;JLT"); }
	if (cmd == "eq") { writeCommand("D;JEQ"); }
	writeCommand("@SETFALSE"); 
	writeCommand("0;JMP");  
	
	writeLabel("BRET"+itoa(iCR));
	iCR++;
	incrementPointer("SP"); //<-- where I want to return
}

void Writer::incrementPointer(std::string p) {
	writeCommand("@"+p);
	writeCommand("M=M+1");
}

void Writer::decrementPointer(::std::string p) {
	writeCommand("@"+p);
	writeCommand("M=M-1");
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
		if (segment == "static") { pushVar(prefix+vmFilename+"."+index); }
		if (segment == "pointer") { pushVar("R"+itoa(3+stoi(index))); }
	} else if (command == c::C_POP) {
		if (segment == "local") { popSegment("LCL", index); }
		if (segment == "argument") { popSegment("ARG", index); }
		if (segment == "this") { popSegment("THIS", index); }
		if (segment == "that") { popSegment("THAT", index); }
		if (segment == "temp") { popVar("R"+itoa(5+stoi(index))); }
		if (segment == "static") { popVar(prefix+vmFilename+"."+index); }
		if (segment == "pointer") { popVar("R"+itoa(3+stoi(index))); }
	}
}

void Writer::close(){
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
	
	//Close the file
	printf("Closing asm file.\n");
	asmFile.close();
}

void Writer::initAsm(){
	//Initialize Stack Pointer and Base Adress of segments
	setVReg("256", "SP");
	setVReg("0", "LCL");
	setVReg("0", "ARG");
	setVReg("0", "THIS");
	setVReg("0", "THAT");
	writeCall("Sys.init", "0");
}

void Writer::setSegAddr(std::string segment, std::string index) {
	writeCommand("@" + index);
	writeCommand("D=A");
	writeCommand("@" + segment);
	writeCommand("D=D+M");
}

void Writer::popSegment(std::string segment, std::string index) {
	setSegAddr(segment, index);
	writeCommand(M_COPY);
	writeCommand("M=D");
	popValue("SP");
	writeCommand("D=M");
	writeCommand(M_COPY);
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
	printf("%i: %s\n", addr, command.c_str());
	asmFile << command << "\n";  
	addr++;
}
