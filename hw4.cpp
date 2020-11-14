//Shaun McCormick
//CSCI230-001
//This program encodes and decodes messages using file codes.dat 

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

enum CypherT{COMBINE = 1, ALTCOMBINE, SHIFT};

string StripKey(string word);
bool OpenFile(ifstream & inFile);
void Execute(ifstream & inFile, string command);
string DoShift(int shift, string command, string message);
string DoCombine(string key, bool isCombine, string command, string message);
char Shift(char leter, int shift, string command);       
char CombineCode(char messageLetter, char keyLetter, bool isCombine, size_t j, string command);  
void PrintCommand(string command, string codeType, int lines);
void CypherSwitch(ifstream & inFile, string command, string newMessage, string key, int shift, string codeType);
CypherT ConvertCodeTypeToInteger(string codeType);
char ReturnToRange(char letter);
void PrintMessage(string newMessage);
string ToUpper(string word);
string FixCapitalization(string message, string newMessage);

int main(){
    bool validFile;
    string command;
    ifstream inFile;
    //Open File
    validFile = OpenFile(inFile);
    //if file opens
    if(validFile){
        inFile >> command;
        while (inFile){
            Execute(inFile, command);
            inFile >> command;
        }
    }
    inFile.close();

    return 0; 
}
//Opens file, returns bool based if file opens or fails to open
bool OpenFile(ifstream & inFile){
    bool truthValue;

    inFile.open("codes.dat");

    if (not inFile){
        truthValue = false;
    }
    else {
        truthValue = true;
    }

    return truthValue;
}
//takes in variables and executes high level code. print command prints everything but the encoded/decoded message, cypherswitch handles the lower level execution.
void Execute(ifstream & inFile, string command){
    string codeType, key, newMessage;
    int shift = 0, lines;
    
    inFile >> codeType; 
    if(codeType == "shift"){
        inFile >> shift;
    }
    else{
        inFile >> key;
    }
    inFile >> lines;
    inFile.ignore();
    PrintCommand(command, codeType, lines);

    while(lines != 0){
        CypherSwitch(inFile, command, newMessage, key, shift, codeType);
        lines--;
    }

    return;
}

//makes strings uppercase and removes all characters non-alphabetic
string StripKey(string key){
    size_t i;

    for(i=0; i<key.size(); i++){
        if(!isalpha(key[i])){
            key.erase(i--, 1);
        }
        else{
            key = ToUpper(key);
        }
    }

    return key;
}
//makes strings uppercase, made this its own function due to needing it in multiple places in the program.
string ToUpper(string word){
    size_t i;
    char letter;

    for(i=0; i<word.size(); i++){
        letter = word[i];
        letter = static_cast<char>(toupper(letter));
        word[i] = letter;
    }

    return word;
}
//a switch that determines the command and calls the apropriate lower level functions, i dont think im using enums the best way I can.
void CypherSwitch(ifstream & inFile, string command, string newMessage, string key, int shift, string codeType){
    string message, linesString;
    CypherT cypher;
    cypher = ConvertCodeTypeToInteger(codeType);
    key = StripKey(key);
    getline(inFile, message);
    switch(cypher){
        case COMBINE:
            newMessage = DoCombine(key, true, command, message);
            break;
        case ALTCOMBINE:
            newMessage = DoCombine(key, false, command, message);
            break;
        case SHIFT:
            newMessage = DoShift(shift, command, message);
            break;
    }
    newMessage = FixCapitalization(message, newMessage);
    PrintMessage(newMessage);

    return;
}
//converts the codeType string to a CypherT and returns it to CypherSwitch
CypherT ConvertCodeTypeToInteger(string codeType){
    CypherT cypher = COMBINE;

    if(codeType == "combine"){
        cypher = COMBINE;
    }
    else if (codeType == "altCombine"){
        cypher = ALTCOMBINE;
    }
    else if (codeType == "shift"){
        cypher = SHIFT;
    }

    return cypher;
}

//turns message uppercase, uses for loop to combine message with key word by calling CombineCode which is the lowest level execution
string DoCombine(string key, bool isCombine, string command, string message){
    string newMessage;
    char messageLetter, keyLetter;
    size_t i, j=0;
    
    message = ToUpper(message);
    for(i=0; i<message.size(); i++){
        if(j == key.size()){
            j = 0;
        }
        messageLetter = message[i];
        keyLetter = key[j];
        if(!isspace(messageLetter)){
            messageLetter = CombineCode(messageLetter, keyLetter, isCombine, j, command);
            j++;
        }
        newMessage += messageLetter;
    }

    return newMessage;
}
//lowest level code, combines one character of message and returns the character. im not sure if this way is more efficient or if itd be better to return the entire line
char CombineCode(char messageLetter, char keyLetter, bool isCombine, size_t j, string command){
    char returnLetter = 'a';
    int shift, oddEven;

    if(isalpha(returnLetter)){
        oddEven = j % 2;
        shift = static_cast<int>(keyLetter) - 'A';
        if((isCombine == false) && (oddEven != 0)){
            shift *= -1;
        }
        if(command == "decode"){
            shift *= -1;
        }
        returnLetter = static_cast<char>(messageLetter + shift);
        while(returnLetter > 'Z' || returnLetter < 'A'){
            returnLetter = ReturnToRange(returnLetter);
        }
    }
    
    return returnLetter;
}
//calls function to make message uppercase, takes each character of the message and the shift value and sends them to shift, which does the lowest level calculation.
string DoShift(int shift, string command, string message){
    string newMessage;
    size_t i;
    char messageChar;

    message = ToUpper(message);
    for (i=0; i<message.size(); i++){
        messageChar = message[i];
        if (!isspace(messageChar)){
            messageChar = Shift(messageChar, shift, command);
        }
        newMessage = newMessage + messageChar;
    }

    return newMessage;
}
//lowest level of code, adds shift to letter and checks to see if in range, then returns character
char Shift(char letter, int shift, string command){
    if(command == "decode"){
        shift *= -1;
    }
    
    letter = static_cast<char>(letter + shift);
    
    while(letter > 'Z' || letter < 'A'){
        letter = ReturnToRange(letter);
    }

    return letter;
}
//prints the command, lines and cypher. does not print encoded/decoded line
void PrintCommand(string command, string codeType, int lines){
    if(command == "encode"){
        cout << "Encoding ";
    }
    else if(command == "decode"){
        cout << "Decoding ";
    }
    cout << lines << " lines with " << codeType << "." << endl;

    return;
}
//PUT FUNCTION TO RETURN LETTERS TO RANGE
char ReturnToRange(char letter){
    int dif = 0;
    
    if(letter > 'Z'){
        dif = static_cast<int>(letter) - '['; //[ = 91 or 1 over Z
        letter = 'A';
    }
    else if (letter < 'A'){
        dif = static_cast<int>(letter) - '@'; //@ = 64 or 1 below A
        letter = 'Z';
    }

    letter = static_cast<char>(letter + dif); //these are both chars why is it yelling at me :(

    return letter;
}
//prints the message, i just made this function for readability. it would be less lines to just write the actual thing it does in cypherswitch
void PrintMessage(string newMessage){
    cout << "\t" << newMessage << endl;

    return;
}
//takes original message and compares it to the new message, and changes the capitalization of the new message to match taht of the original message and returns the new message.
string FixCapitalization(string message, string newMessage){
    size_t i;
    char messageLetter, newLetter;

    for(i=0; i<message.size(); i++){
        messageLetter = message[i];
        newLetter = newMessage[i];
        if(islower(messageLetter)){
            newLetter = static_cast<char>(tolower(newLetter));
            newMessage[i] = newLetter;
        }
    }

    return newMessage;
}