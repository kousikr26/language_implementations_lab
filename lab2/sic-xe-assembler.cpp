/* SIC-XE Assembler */

/* Run Instructions
g++ sic-xe-assembler.cpp
./a.out
 
*/
#include <bits/stdc++.h>
using namespace std;

#define DELIMITER '^'

//Utility Function delcarations
string intToHex(int x);
int hexToInt(string x);
string hexToBin(string x) ;
string binToHex(string x) ;
void split(string const &input, bool intermediate = false);
vector<string> splitDelimiter(string x,char delim =',');
void strip(string &x);
string pad(string x, int len = 6, char y = '0', bool right = false);
void read_optab();
string get_initial(string opcode, int n, int i, int x, int p, int b, int e, int len = 8);
bool is_number(string &s);
string toUpper(string x);
void add_registers();
//Global variables
string inp,
	label, opcode, operand, record, recordAddress, lineAddress;

//Hashtables for optable and symtable
unordered_map<string, string> optab, symtab,littab,templittab;
unordered_map<string,int> sectionLengths;
unordered_set<string> type2_instructions = {"COMPR","CLEAR","TIXR","ADDR"};
vector<string> ext_refs, ext_defs;
vector<string> mod_recs;
/*
Extra opcodes in SIC/XE
LDL,LDB,LDT,STL,COMPR(2),CLEAR(2),J,TIX,TIXR(2),STCH
+JSUB
handle type 1 instructions
load register names symtab
base relative addressing
evaluate expressions buffend - buffer *****

*/


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
			if(opcode[0] == '.') continue;
			
			else if (opcode=="CSECT"){
				sectionLengths.insert({progName,currentAddress - hexToInt(startAddress)});
				progName = label;
				currentAddress = 0;
				output << left << setw(6) << pad(intToHex(currentAddress), 4) << setw(10) << label << setw(10) << opcode << setw(20) << "" << endl;
				continue;
			}
			if(operand[0]=='='){
				if(templittab.count(operand.substr(1))) ;
				else templittab[operand.substr(1)] = "";
			}
			if (opcode == "LTORG" || opcode =="END")
			{
				
				
				for (auto elem : templittab)
				{
					littab[elem.first] = intToHex(currentAddress);
					output << left << setw(6) << pad(intToHex(currentAddress), 4) << setw(10) << "" << setw(10) << "*" << setw(20) << elem.first << endl;
					if (elem.first[0] == 'C')
					{
						int len = elem.first.length() - 3;
						currentAddress += len;
					}
					else if (elem.first[0] == 'X')
					{
						int len = elem.first.length() - 3;
						currentAddress += len / 2;
					}
				}
				templittab.clear();
				if(opcode=="LTORG") continue;
				else{
					;
				}
			}
			else if (opcode == "EQU")
			{
				if (operand == "*"){
					operand = intToHex(currentAddress);
					symtab[label] =operand; 
				}
					
				else
				{
					if (is_number(operand))
					{
						symtab[label] = operand;
					}
					
				}
				continue;
			}
			output << left<< setw(6) << pad(intToHex(currentAddress),4) << setw(10) << label << setw(10) << opcode << setw(20) << operand << endl;
			if (label != ""){
				if(symtab.count(label)) cout<<"Duplicate label"<<endl;
				else symtab[label] = intToHex(currentAddress);
			}
			// Check what opcode is and increment the currentAddress accordingly
			
			if(opcode=="COMPR" || opcode=="CLEAR" || opcode =="TIXR"){
				currentAddress+=2;
			}
			else if(optab.count(opcode)) currentAddress+=3;
			
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
			else if(opcode == "END") {
				;
			}
			else if(opcode[0]=='+'){
				currentAddress+=4;
			}
			else if (opcode == "EXTREF" || opcode =="EXTDEF"){
				;
			}
			

			else cout << "Illegal opcode " << opcode << " " << endl;
			
		}	
	}
	sectionLengths.insert({progName,currentAddress - hexToInt(startAddress)});
	input.close();
	output.close();
	cout << "Pass 1 complete" << endl;
	
	add_registers();
	// Pass 2

	bool firstSection = true;
	input.open("intermediate.asm");
	output.open("output.txt");
	int recordLength=0;
	int programCounter = 0;
	while (getline(input, inp)) // Read the intermediate file
	{
		split(inp,true);
		
		if(recordAddress=="") recordAddress=lineAddress;
		if(opcode == "START"){
			// Output the header record to output file
			output<<"H"<<"^"<<pad(label,6,' ',true)<<"^"<<pad(startAddress)<<"^"<<pad(intToHex(sectionLengths[label]))<<endl;
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
			if(opcode == "RESW") programCounter+=3*stoi(operand);
			else
				programCounter += stoi(operand);
		}
		else if (opcode == "EXTREF")
		{

			ext_refs.clear();
			ext_refs = splitDelimiter(operand);
			output << "R";
			for(auto elem:ext_refs){
				output<<"^"<<pad(elem,6,' ',true);
			}
			output<<endl;
		}
		else if (opcode == "EXTDEF"){
			ext_defs.clear();
			ext_defs = splitDelimiter(operand);
			output << "D";
			for (auto elem : ext_defs)
			{
				output << "^"<<pad(elem, 6, ' ', true) << "^"<<pad(symtab[elem],6);
			}
			output << endl;
		}
		else{
			// Handle the case of WORD and BYTE instructions
			if(opcode == "WORD"){
				if(is_number(operand)) record+= DELIMITER + pad(intToHex(stoi(operand)));
				else{
					record += DELIMITER + pad("0"); 
					// put in function
					if (operand.find('-') != std::string::npos)
					{
						vector<string> operands = splitDelimiter(operand,'-');
						mod_recs.push_back(pad(intToHex(programCounter+1),6) + "^06" + "^+" + operands[0]);
						mod_recs.push_back(pad(intToHex(programCounter+1),6) + "^06" + "^-" + operands[1]);
					}

					else if (operand.find('+') != std::string::npos)
					{
						vector<string> operands = splitDelimiter(operand,'+');
						mod_recs.push_back(pad(intToHex(programCounter+1),6) + "^06" + "^+" + operands[0]);
						mod_recs.push_back(pad(intToHex(programCounter+1),6) + "^06" + "^+" + operands[1]);
					}
					else{
						cout<<"Unsupported symbol foud in operand : "<<operand<<endl;
					}
						
				}
					
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
				int type = 3, indirect = 1, immediate = 1, index = 0, base = 0, pc_relative = 1,extended = 0,literal= 0;
				int padding = type;
				if (operand.length() > 2 && operand.substr(operand.length() - 2, 2) == ",X") //indexed addressing
				{
					index = 1;
					
					operand = operand.substr(0, operand.length() - 2);
					
				}
				if(opcode[0] == '+'){
					extended = 1;
					pc_relative = 0;
					opcode = opcode.substr(1);
					type = 4;
					padding = 5;
					mod_recs.push_back(pad(intToHex(programCounter+1),6)+"^05"+"^+"+operand);
				}
				
				if(operand[0] == '#'){
					indirect = 0;
					operand = operand.substr(1);
				}
				else if (operand[0]=='@'){
					immediate = 0;
					operand = operand.substr(1);
				}
				else if(operand[0]=='='){
					operand = operand.substr(1);
					literal = 1;
				}
				
				if(type2_instructions.count(opcode)){
					type = 2;
					pc_relative = 0;
					padding = type;
				}
				string target;

				
				if (opcode == "CLEAR" ||opcode == "TIXR"){
					type = 2;
					pc_relative = 0;
					programCounter +=type;
					record += (DELIMITER + optab[opcode] + pad(symtab[operand],2,'0',true));
					recordLength+=2;
					
				}
				else if(opcode == "COMPR"){
					type = 2;
					programCounter+=type;
					record += (DELIMITER + optab[opcode] + symtab[operand.substr(0,1)]+symtab[operand.substr(2)]);
					recordLength += 2;
				}
				else if (optab.count(opcode) && (symtab.count(operand) || find(ext_refs.begin(), ext_refs.end(), operand) != ext_refs.end() || indirect == 0 || literal || index))
				{
					programCounter += type;

					if (find(ext_refs.begin(), ext_refs.end(), operand) != ext_refs.end())
						target = pad("", padding);
					else if (literal)
					{
						
						target = pad(intToHex((hexToInt(littab[operand]) - programCounter)), type);
					}
					else if(index){
						target = pad("0", 5); //Change this
					}
					else if (symtab.count(operand))
					{
						if (hexToInt(symtab[operand]) > programCounter)
							target = pad(intToHex((hexToInt(symtab[operand]) - programCounter)), padding);
						else
						{

							int diff = hexToInt(symtab[operand]) - programCounter;
							target = pad(intToHex(diff), padding);
						}
					}
					else if (indirect == 0)
					{
						if (is_number(operand))
							target = pad(operand, type);
						else
							target = pad(symtab[operand], type);
					}
					
					
					record += (DELIMITER + get_initial(optab[opcode], indirect, immediate, index, base, pc_relative, extended)) + target;
					recordLength += type;
				}
				else if(opcode =="RSUB"){
					programCounter += type;
					pc_relative = 0;
					record += DELIMITER + get_initial(optab[opcode], indirect, immediate, index, base, pc_relative, extended) + pad("0", 4);
					recordLength += 3;
				}
				else if (operand.length()>2 && operand.substr(operand.length() - 2, 2) == ",X")  //indexed addressing
				{
					index = 1;
					programCounter += type;
					operand = operand.substr(0, operand.length() - 2);
					int shifted_address = 32768+hexToInt(symtab[operand]);
					target = pad("0",5); //Change this 
					record += (DELIMITER + get_initial(optab[opcode], indirect, immediate, index, base, pc_relative, extended)) + target;
					recordLength += type;
				}
				else if(opcode == "END"){
					if (recordLength > 0)
					{
						output << "T^" + pad(recordAddress) + "^" + pad(intToHex(recordLength), 2)  + record << endl;
						record = "";
						recordLength = 0;
					}
					for (auto elem : mod_recs)
					{
						output << "M^" + elem << endl;
					}
					mod_recs.clear();
					recordAddress = "";
					output << "E";
					if (firstSection)
						output << "^" << pad(startAddress) << endl
							   << endl;
					else
						output  << endl
							   << endl;
				}
				else if(opcode =="*"){
					if (operand[0] == 'C')
					{
						record += DELIMITER;
						for (int i = 2; i < operand.length() - 1; i++)
						{
							record += intToHex((int)operand[i]);
							recordLength += 1;
							
						}
						programCounter += (operand.length() - 3);
					}
					else if (operand[0] == 'X')
					{
						record += DELIMITER + operand.substr(2, operand.length() - 3);
						recordLength += (operand.length() - 3) / 2;
						programCounter += (operand.length() - 3) / 2;
					}
				}
				else if(operand=="CSECT"){
					
					if (recordLength > 0)
					{
						output << "T^" + pad(recordAddress) + "^" + pad(intToHex(recordLength), 2) + record << endl;
						record = "";
						recordLength = 0;
					}
					for(auto elem:mod_recs){
						output<<"M^"+elem<<endl;
					}
					mod_recs.clear();
					recordAddress = "";
					output << "E" ;
					if(firstSection) output << "^"<<pad(startAddress) << endl<< endl;
					else output <<endl<<endl;
					programCounter = 0;
					firstSection = false;
					output << "H"<< "^" << pad(opcode, 6, ' ', true) << "^" << pad(startAddress) << "^" << pad(intToHex(sectionLengths[opcode])) << endl;
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
	cout<<"Object code stored in output.txt"<<endl;
	
}
string intToHex(int x){
	//Takes an integer and return Hex string corresponding to it
	stringstream hexStream;
	hexStream<<hex<<x;
	string res(hexStream.str());
	
	return toUpper(res);
}
string toUpper(string x){
	for (auto &c : x)
		c = toupper(c);
	return x;
}
int hexToInt(string x){
	// Takes a hex string and returns corresponding decimal integer
	int res;
	stringstream intStream;
	intStream << x;
	intStream >> hex >> res; 

	return res;
}
string hexToBin(string x)
{
	// Takes a hex string and returns corresponding decimal integer
	stringstream ss;
	ss << hex << x;
	unsigned n;
	ss >> n;
	bitset<8> b(n);
	return b.to_string();
}
string binToHex(string x){
	stringstream ss;
	ss << hex << stoll(x, NULL, 2);
	return ss.str();
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

		else if (ret.size() == 2){
			if(ret[1] == "CSECT")
				label = ret[0], opcode = ret[1];

			else label = "", opcode = ret[0], operand = ret[1];

		}
			
		else
			label = "", opcode = ret[0], operand = "";
	}
}
vector<string> splitDelimiter(string x, char delim){
	vector<string> vect;
	stringstream ss(x);
	string str;
	while (getline(ss, str, delim))
	{
		vect.push_back(str);
	}
	return vect;
}
string pad(string x, int len , char y, bool right ){
	// Pad a string to length len with char y on the right if right = true
	if(len <= x.length()) {
		return x.substr(x.length()-len);
	}
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
		optab[opcode] = operand;
	}
	optabfile.close();

}
string get_initial(string opcode, int n , int i, int x, int p, int b, int e, int len){
	string opcodeBin = hexToBin(opcode);

	opcodeBin[len - 2] = (char)(n + '0');
	opcodeBin[len - 1] = (char)(i + '0');
	
	stringstream ss;
	ss << x<<p<<b<<e;
	opcodeBin=toUpper(binToHex(opcodeBin) + binToHex(ss.str()));
	
	return pad(opcodeBin,3);
}
bool is_number(string &s)
{
	return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}
void add_registers(){
	symtab["A"] = "0";
	symtab["X"] = "1";
	symtab["L"] = "2";
	symtab["B"] = "3";
	symtab["S"] = "4";
	symtab["T"] = "5";
	symtab["F"] = "6";
}