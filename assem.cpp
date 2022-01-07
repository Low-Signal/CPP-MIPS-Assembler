/******************************************************
*   Name: Daimeun Praytor                             *
*   MIPS assembly language                            *
*   Compile: "make"                                   *
*   Run: assem < fileName.asm                         *
*                                                     *
*   Assembles a subset of the MIPS instruction set    *
*                                                     *
*                                                     *
******************************************************/

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Returns the decimal value of the register given.
int registerTable(string registerStr, int lineNum);

// Checks if a string contains only numbers.
bool isNumber(string numString);

// Structure to store the encoded instructions
typedef struct{
    union {
        struct {
            unsigned funct: 6;
            unsigned shamt: 5;
            unsigned rd: 5;
            unsigned rt: 5;
            unsigned rs: 5;
            unsigned opcode: 6;
        } rformat;
        struct {
            unsigned immed: 16;
            unsigned rt: 5;
            unsigned rs: 5;
            unsigned opcode: 6;
        } iformat;
        struct {
            unsigned address: 26;
            unsigned opcode: 6;
        } jformat;
        unsigned int encodedValue;
    } u;

    char format;
} mips_template;


int main() {

    // Used for parsing the file
    char line[80] = {0};
    char oper[80];
    char rd[5], rs[5], rt[5];
    char directiveCheck[10];
    char labelCheck[10];
    char semicolon[2];

    // Variables for conversions to c++ strings
    string operString;
    string rdString;
    string rsString;
    string rtString;

    // Tracks the running tally of instructions and words
    int lineNumber = 0;
    int numberWords = 0;

    // Stores the labels and line numbers
    struct labels{
        string name;
        int labelLine = 0;
    };

    // Holds an vector of label structs
    vector<labels> labelList;

    // Holds the data segment in a vector.
    vector<vector<string>> dataVector;

    // Holds the resolved symbols
    vector<vector<string>> symbolVector;

    // Variables for tracking types of symbols found.
    bool inText = false;
    bool inData = false;
    bool isLabel = false;

    while (fgets(line, 80, stdin)) {

        // Stores possible directives without leading whitespace.
        if (sscanf(line, " .%s ", directiveCheck)==1){

            // Checks if the line is a .text directive
            if(strcmp(directiveCheck, "text") == 0){
                inText = true;
                inData = false;

                continue;
            }
            // Checks if the line is a .data directive
            else if(strcmp(directiveCheck, "data") == 0){
                inData = true;
                inText = false;

                continue;
            }
        }

        // Parses the data segment
        if(inData) {
            if (sscanf(line, "%[^:]%s", labelCheck, semicolon) == 2){

                // Creates a new "line" variable without the label
                char delim[] = ":";
                char *ptr = strtok(line, delim);
                ptr = strtok(NULL, delim);

                strcpy(line, ptr);

                char dataType[10];

                if(sscanf(line, " .%s ", dataType ) == 1){

                    string lineString = line;

                    // Gets just the number string
                    lineString = lineString.substr(lineString.find_first_of(" \t")+1);
                    lineString = lineString.substr(lineString.find_first_of(" \t")+1);

                    // Pushes each .word data slot to a vector tied to the variable name
                    if(strcmp(dataType, "word") == 0){
                        stringstream ss(lineString);

                        for(int i; ss >> i;) {
                            dataVector.push_back({{labelCheck, to_string(i)}});

                            if(ss.peek() == ',')
                                ss.ignore();
                            
                            numberWords++;
                        }
                    }
                    else if(strcmp(dataType, "space")== 0){

                        // Converts the number of bits of space to a int
                        int spaceBits = stoi(lineString);

                        // Adds blank space to the dataVector
                        for(int i = 0; i < spaceBits; i++){
                            dataVector.push_back({{labelCheck, to_string(0)}});
                            numberWords++;
                        }
                    }
                }
            }
        }
        
        // Parses the text segment
        if(inText){

            // Stores the first string in a line to check if its a label
            if (sscanf(line, "%[^:]%s", labelCheck, semicolon)==2){
                // Creates a new label structure and pushes it to the label vector.
                labels tempLabel;
                tempLabel.labelLine = lineNumber + 1;
                tempLabel.name = labelCheck;

                labelList.push_back(tempLabel);

                // Creates a new "line" variable without the label
                char delim[] = ":";
                char *ptr = strtok(line, delim);
                ptr = strtok(NULL, delim);

                strcpy(line, ptr);
                
            }

            // Sccanf functions to parse the input line for different instruction formats.
            if (sscanf(line, "%s $%[^,],$%[^,],$%s", oper, rd, rs, rt) == 4) {
                operString = oper;
                rdString = rd;
                rsString = rs;
                rtString = rt;

                vector<string> temp = {operString, rdString, rsString, rtString};
                symbolVector.push_back(temp);

                lineNumber++;   
            }
            else if(sscanf(line, "%s $%[^,],$%[^,],%s", oper, rd, rs, rt) == 4) {
                operString = oper;
                rdString = rd;
                rsString = rs;
                rtString = rt;

                vector<string> temp = {operString, rdString, rsString, rtString};
                symbolVector.push_back(temp);
                lineNumber++;
            }
            else if (sscanf(line, "%s $%[^,],%[^(]($%[^)]", oper, rd, rs, rt) == 4) {
                operString = oper;
                rdString = rd;
                rsString = rs;
                rtString = rt;

                vector<string> temp = {operString, rdString, rsString, rtString};
                symbolVector.push_back(temp);

                lineNumber++;
            }
            else if (sscanf(line, "%s $%[^,],$%s", oper, rd, rs) == 3) {
                
                operString = oper;
                rdString = rd;
                rsString = rs;

                vector<string> temp = {operString, rdString, rsString};
                symbolVector.push_back(temp);

                lineNumber++;
            }
            else if (sscanf(line, "%s $%s", oper, rd) == 2) {
                
                operString = oper;
                rdString = rd;

                vector<string> temp = {operString, rdString};
                symbolVector.push_back(temp);

                lineNumber++;
            }
            else if (sscanf(line, "%s %s", oper, rd) == 2) {
                if(strcmp(oper, "j") == 0){

                    operString = oper;
                    rdString = rd;

                    vector<string> temp = {operString, rdString};
                    symbolVector.push_back(temp);

                    lineNumber++;  
                } 
            }
            else if(sscanf(line, " %s ", oper) == 1){
                if(strcmp(oper, "syscall") == 0){

                    operString = oper;

                    vector<string> temp = {operString};
                    symbolVector.push_back(temp);

                    lineNumber++;
                }
                else {
                    printf("input line: %s\n", line);
                    printf("you need to add sscanf format\n");
                }
            }
            else {
                printf("input line: %s\n", line);
                printf("you need to add sscanf format\n");
            }
        }

    }

    // Outputs the number of instructions and the number of words stored in the data segment.
    cout << "Number of instructions: " << lineNumber << " Number of words: " << numberWords << endl;

    for(int i = 0; i < symbolVector.size(); i++){
        mips_template tempEncoding;

        // Encodes all instructions
        if(symbolVector[i][0] == "addiu"){
            tempEncoding.u.iformat.immed = stoi(symbolVector[i][3]);
            tempEncoding.u.iformat.rt = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.iformat.rs = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.iformat.opcode = 9;

        }
        else if(symbolVector[i][0] == "addu"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][3], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 33;

        }
        else if(symbolVector[i][0] == "and"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][3], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 36;

            
        }
        else if(symbolVector[i][0] == "beq" || symbolVector[i][0] == "bne"){
            int currentLabelLine;
            int destinationLine;

            // Gets the line number of the destination label.
            for(int j = 0; j < labelList.size(); j++){
                if(labelList[j].name == symbolVector[i][3]){
                    currentLabelLine = labelList[j].labelLine;
                    break;
                }
            }

            // Label - current line.
            destinationLine = (currentLabelLine - (i+1));

            tempEncoding.u.iformat.immed = destinationLine;
            tempEncoding.u.iformat.rt = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.iformat.rs = registerTable(symbolVector[i][1], i+2);

            // Adds opcode based on instruction type
            if(symbolVector[i][0] == "beq"){
                tempEncoding.u.iformat.opcode = 4;
            }
            else{
                tempEncoding.u.iformat.opcode = 5;
            }
        }
        else if(symbolVector[i][0] == "bne"){
            int currentLabelLine;
            int destinationLine;

            // Gets the line number of the destination label.
            for(int j = 0; j < labelList.size(); j++){
                if(labelList[j].name == symbolVector[i][3]){
                    currentLabelLine = labelList[j].labelLine;
                    break;
                }
            }

            // Label - current line.
            destinationLine = (currentLabelLine - (i+1));

            tempEncoding.u.iformat.immed = destinationLine;
            tempEncoding.u.iformat.rt = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.iformat.rs = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.iformat.opcode = 5;

        }
        else if(symbolVector[i][0] == "div"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = 0;
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 26;

        }
        else if(symbolVector[i][0] == "j"){
            int tempLabelLine;
            bool foundLabel = false;

            /* Seachers for the label in the label list 
            and sets the instruction number it was found on.*/
            for(int j = 0; j < labelList.size(); j++){
                if(labelList[j].name == symbolVector[i][1]){
                    tempLabelLine = labelList[j].labelLine;
                    foundLabel = true;
                    break;
                }
            }

            // Checks if the label is valid.
            if(!foundLabel){
                cout << "Invalid Instruction " << symbolVector[i][0] <<  " at line: " << i+2 << endl;
                return 0;
            }

            tempEncoding.u.jformat.address = tempLabelLine - 1;
            tempEncoding.u.jformat.opcode = 2;
        }
        else if(symbolVector[i][0] == "mfhi"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = 0;
            tempEncoding.u.rformat.rt = 0;
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 16;

        }
        else if(symbolVector[i][0] == "mflo"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = 0;
            tempEncoding.u.rformat.rt = 0;
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 18;

        }
        else if(symbolVector[i][0] == "mult"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = 0;
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 24;

        }
        else if(symbolVector[i][0] == "or"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][3], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 37;

        }
        else if(symbolVector[i][0] == "slt"){
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][3], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 42;

        }
        else if(symbolVector[i][0] == "subu") {
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.rformat.rs = registerTable(symbolVector[i][2], i+2);
            tempEncoding.u.rformat.rt = registerTable(symbolVector[i][3], i+2);
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 35;

        }
        else if(symbolVector[i][0] == "sw" || symbolVector[i][0] == "lw") {
            int lineAddress;
            bool foundLabel = false;

            // If the offset is a decimal number.
            if(isNumber(symbolVector[i][2])) {
                lineAddress = stoi(symbolVector[i][2]);
                tempEncoding.u.iformat.immed = lineAddress;
            }
            // If the offset is a label string;
            else{
                for(int j = 0; j < dataVector.size(); j++) {
                    if(dataVector[j][0] == symbolVector[i][2]){
                        tempEncoding.u.iformat.immed = j;
                        foundLabel = true;
                        break;
                    }
                }

                // Error for unresolved label.
                if(!foundLabel){
                    cout << "could not find label " << symbolVector[i][2];
                    return 0;
                }
            }

            // Checks the instruction type to get the opcode
            if(symbolVector[i][0] == "sw"){
                tempEncoding.u.iformat.opcode = 43;
            }
            else{
                tempEncoding.u.iformat.opcode = 35;
            }

            tempEncoding.u.iformat.rt = registerTable(symbolVector[i][1], i+2);
            tempEncoding.u.iformat.rs = registerTable(symbolVector[i][3], i+2);
        }
        else if(symbolVector[i][0] == "syscall") {
            tempEncoding.u.rformat.opcode = 0;
            tempEncoding.u.rformat.rd = 0;
            tempEncoding.u.rformat.rs = 0;
            tempEncoding.u.rformat.rt = 0;
            tempEncoding.u.rformat.shamt = 0;
            tempEncoding.u.rformat.funct = 12;

        }
        // If the instruction is invalid
        else{
            cout << "Invalid Instruction " << symbolVector[i][0] <<  " at line: " << i+2 << endl;
            return 0;
        }

        // Prints the encoded instruction
        printf("%08x\n", tempEncoding.u.encodedValue);
    }

    // Prints out the values stored in the dataVector.
    for(int i = 0; i < dataVector.size(); i++){
        mips_template tempEncoding;

        // Easy to use format for converting decimal to hex
        tempEncoding.u.jformat.opcode = 0;
        tempEncoding.u.jformat.address = stoi(dataVector[i][1]);
        printf("%08x\n", tempEncoding.u.encodedValue);
    }


    return 0;
}

// Holds all registers decimal value
int registerTable(string registerStr, int lineNum){

    if(registerStr == "zero"){
        return 0;
    }
    else if(registerStr == "at"){
        return 1;
    }
    else if(registerStr == "v0"){
        return 2;
    }
    else if(registerStr == "v1"){
        return 3;
    }
    else if(registerStr == "a0"){
        return 4;
    }
    else if(registerStr == "a1"){
        return 5;
    }
    else if(registerStr == "a2"){
        return 6;
    }
    else if(registerStr == "a3"){
        return 7;
    }
    else if(registerStr == "t0"){
        return 8;
    }
    else if(registerStr == "t1"){
        return 9;
    }
    else if(registerStr == "t2"){
        return 10;
    }
    else if(registerStr == "t3"){
        return 11;
    }
    else if(registerStr == "t4"){
        return 12;
    }
    else if(registerStr == "t5"){
        return 13;
    }
    else if(registerStr == "t6"){
        return 14;
    }
    else if(registerStr == "t7"){
        return 15;
    }
    else if(registerStr == "s0"){
        return 16;
    }
    else if(registerStr == "s1"){
        return 17;
    }
    else if(registerStr == "s2"){
        return 18;
    }
    else if(registerStr == "s3"){
        return 19;
    }
    else if(registerStr == "s4"){
        return 20;
    }
    else if(registerStr == "s5"){
        return 21;
    }
    else if(registerStr == "s6"){
        return 22;
    }
    else if(registerStr == "s7"){
        return 23;
    }
    else if(registerStr == "t8"){
        return 24;
    }
    else if(registerStr == "t9"){
        return 25;
    }
    else if(registerStr == "k0"){
        return 26;
    }
    else if(registerStr == "k1"){
        return 27;
    }
    else if(registerStr == "gp"){
        return 28;
    }
    else if(registerStr == "sp"){
        return 29;
    }
    else if(registerStr == "fp"){
        return 30;
    }
    else if(registerStr == "ra"){
        return 31;
    }
    else{
        cout << "Invalid register at line: " << lineNum << endl;
        exit(EXIT_FAILURE);
    }
}

// Checks if a string is a number
bool isNumber(string numString) {
    for(int i = 0; i < numString.length(); i++) {
        if(isdigit(numString[i]) == false)
            return false;
    }
    return true;
}