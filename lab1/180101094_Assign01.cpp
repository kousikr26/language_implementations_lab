
/* Run Instructions
g++ 180101094_Assign01.cpp
./a.out
 
*/
#include <bits/stdc++.h>
using namespace std;

#define DELIMITER '^'

//Utility Function delcarations
string intToHex(int x);
int hexToInt(string x);
void split(string const &input,bool intermediate=false);
void strip(string &x);
string pad(string x, int len = 6, char y = '0', bool right = false);
void read_optab();

//Global variables
string inp, label, opcode, operand, record, recordAddress, lineAddress;

//Hashtables for optable and symtable
unordered_map<string, string> optab, symtab;




int main(){
    
    ifstream input; 
	ofstream output;
	input.open("input.asm");
	output.open("intermediate.asm");
	int currentAddress = 0,progLength = 0;
	string progName,startAddress; //progName is name of the program and startAddress is the starting address of first instruction
	read_optab(); // Read optable from file
	getline(input,inp); // Get the first line from input file
	split(inp); // Splits the line into label opcode operand
	if(opcode == "START"){
		currentAddress = hexToInt(operand);
		startAddress = operand;
		progName = label;
		output << left<<setw(6) << intToHex(currentAddress) << setw(10) << label << setw(10) << opcode << setw(20) << operand << endl;
	}
	else cout<<"START statement not found"<<endl;

    while(getline(input,inp)){
        // Loop over every line in the file
		if (inp.size()>0){
			
			split(inp);
			output << left<< setw(6) << intToHex(currentAddress) << setw(10) << label << setw(10) << opcode << setw(20) << operand << endl;
			if (label != ""){
				if(symtab.count(label)) cout<<"Duplicate label"<<endl;
				else symtab[label] = intToHex(currentAddress);
			}
			// Check what opcode is and increment the currentAddress accordingly
			if(optab.count(opcode)) currentAddress+=3;
			
			else if (opcode == "WORD") currentAddress+=3;
			else if (opcode == "RESW") currentAddress +=(3*stoi(operand));
			else if (opcode == "RESB") currentAddress+=stoi(operand);
			else if (opcode == "BYTE"){
				if(operand[0] == 'X')
					currentAddress+=(operand.length()-3)/2;
				else if (operand[0] == 'C')
					currentAddress +=operand.length()-3;
			}
			else if(opcode =="START") cout << "Illegal START statement" << endl;
			else if(opcode == "END") break;
			else cout << "Illegal opcode " << opcode << " " << endl;
			
		}	
	}
	progLength = currentAddress-hexToInt(startAddress);	
    input.close();
	output.close();
	cout << "Pass 1 complete" << endl;

	// Pass 2
	input.open("intermediate.asm");
	output.open("output.txt");
	int recordLength=0;
	while (getline(input, inp)) // Read the intermediate file
	{
		split(inp,true);

		if(recordAddress=="") recordAddress=lineAddress;
		if(opcode == "START"){
			// Output the header record to output file
			output<<"H"<<"^"<<pad(label,6,' ',true)<<"^"<<pad(startAddress)<<"^"<<pad(intToHex(progLength))<<endl;
		}
		else if(opcode == "RESW" || opcode =="RESB"){
			// Incase of RESW or RESB a new record needs to be started
			if(recordLength>0){
				output << "T^" + pad(recordAddress) + "^" + pad(intToHex(recordLength), 2)  + record << endl;
				record = "";
				recordLength = 0;
				continue;
			}
			recordAddress = "";
		}
		else{
			// Handle the case of WORD and BYTE instructions
			if(opcode == "WORD"){
				record+= DELIMITER + pad(intToHex(stoi(operand)));
				recordLength+=3;
			}
			else if (opcode =="BYTE"){
				if(operand[0]=='C'){
					record+=DELIMITER;
					for(int i=2;i<operand.length()-1;i++){
						record+=intToHex((int)operand[i]);
						recordLength+=1;
					}
					
				}
				else if(operand[0]=='X'){
					record+= DELIMITER + operand.substr(2,operand.length() - 3);
					recordLength+=(operand.length()-3)/2;
					
				}
			}
			else{
				// Handle various cases of opcode and operand
				if (optab.count(opcode) && symtab.count(operand))
				{
					record += (DELIMITER + pad(optab[opcode], 2) + pad(symtab[operand], 4));
					recordLength+=3;
				}
				else if(opcode =="RSUB"){

					record += DELIMITER + pad(optab[opcode], 2) + pad("0", 4);
					recordLength += 3;
				}
				else if (operand.substr(operand.length() - 2, 2) == ",X") 
				{
					operand = operand.substr(0, operand.length() - 2);
					int shifted_address = 32768+hexToInt(symtab[operand]);
					record += DELIMITER + pad(optab[opcode], 2) + pad(intToHex(shifted_address), 4);
					recordLength += 3;
				}
				else if(opcode == "END"){
					if (recordLength > 0)
					{
						output << "T^" + pad(recordAddress) + "^" + pad(intToHex(recordLength), 2)  + record << endl;
						record = "";
						recordLength = 0;
					}
					recordAddress = "";
					output<<"E^"<<pad(startAddress);
				}
				else cout<<"Opcode or operand not found "<<"opcode : "<<opcode<<" operand : "<<operand<<endl;
			}
			if(recordLength>27){ // If recordLength has exceeded 27 a new record has to be started
				output<<"T^"+pad(recordAddress)+"^"+pad(intToHex(recordLength),2)+record<<endl;
				record = "";
				recordAddress="";
				recordLength=0;
			}
		}
	}
	cout<<"Pass 2 complete"<<endl;
	
}
string intToHex(int x){
	//Takes an integer and return Hex string corresponding to it
	stringstream hexStream;
	hexStream<<hex<<x;
	string res(hexStream.str());
	for (auto &c : res)
		c = toupper(c);
	return res;
}
int hexToInt(string x){
	// Takes a hex string and returns corresponding decimal integer
	int res;
	stringstream intStream;
	intStream << x;
	intStream >> hex >> res; 

	return res;
}
void strip(string &x){
	// Strips all the whitespace from a string
	x.erase(remove(x.begin(), x.end(), ' '), x.end());
}
void split(string const &input, bool intermediate ){
	// intermediate is true if the line is read from intermediate file and false if read from input.asm
	// Split the string based on whitespace and store it in lineAddress label opcode and operand
	istringstream buffer(input);
	vector<string> ret((istream_iterator<string>(buffer)), istream_iterator<string>());
	for (int i = 0; i < ret.size(); i++)
	{
		strip(ret[i]);
	}
	if (intermediate)
	{
		if (ret.size() == 4)
			lineAddress = ret[0], label = ret[1], opcode = ret[2], operand = ret[3];
		else if (ret.size() == 3)
			lineAddress = ret[0], label = "", opcode = ret[1], operand = ret[2];
		else if (ret.size() == 2)
			lineAddress = ret[0], label = "", opcode = ret[1], operand = "";
		else
			cout << "Error in line " << inp << endl;
	}
	else
	{

		if (ret.size() == 3)
			label = ret[0], opcode = ret[1], operand = ret[2];

		else if (ret.size() == 2)
			label = "", opcode = ret[0], operand = ret[1];
		else
			label = "", opcode = ret[0], operand = "";
	}
}
string pad(string x, int len , char y, bool right ){
	// Pad a string to length len with char y on the right if right = true
	if (right)
		return x + string(len - x.length(), y);
	return string(len - x.length(), y) + x;
}

void read_optab(){
	// Read the optab file to optable
	ifstream optabfile("optab.txt");
	string optab_buffer;
	while (getline(optabfile, optab_buffer))
	{
		split(optab_buffer);
		optab[opcode] = intToHex(stoi(operand));
	}
	optabfile.close();
}