#include <bits/stdc++.h>

using namespace std;
const int BYTE = 8;
const char TAB = '\t';
string Input_file;

struct estabEntry
{
	string name,address,len;
};

struct finalval
{
	string val;
	int isPresent = 0;
	int no_half_bytes;
};

map<string, finalval> final_val;
string hexAdd(string hex1, string hex2);
void handle_H(string &templine);
string pad(string x, int len, char y = '0', bool right = false);
string hexSub(string a, string b);
string nearestAddress(string s);
string removePreceding(string x);
int calculateIndex(string hexdiff);
string toUpper(string x);
string intToHex(int x);
long long hexToInt(string x);



ifstream fin, fin2;
ofstream fout;
string startingAddress, progaddress, csaddress, cslth, execaddr;
map<string, estabEntry> estabMap;
int isStarted = 0;



map<string, string> loader_primary;
map<int, string> helpermap;
static int counter_addr = 0;


string findOperand(int offset, int dataLength, string addressLabel)
{
	int tempOffset = 1;
	if (dataLength % 2 == tempOffset)
		offset += tempOffset;
	string operand = "";
	int start = offset, counter = 0;
	for(;;)
	{
		counter++;
		if (counter == dataLength + tempOffset)
			break;
		operand += loader_primary[addressLabel][start];
		start++;
		if (start >= BYTE*4*tempOffset)
			break;
	}
	if (!(counter == dataLength + tempOffset))
	{
		if(dataLength == counter);

		addressLabel = hexAdd(addressLabel, "10");
		start = 0;

		for(;;){
			counter++;
			if (counter - tempOffset== dataLength + tempOffset)
				break;

			operand += loader_primary[addressLabel][start];
			start++;
			if(start>=BYTE*4*tempOffset) break;
		}
	}
	return operand;
}

void updateloaderPrimary(string label_addr, int dataLength, string newval, int offset)
{
	int start = offset;
	string temp = "";
	int j = 0;
	int secOffset = j;
	
	while (start+secOffset < BYTE*4)
	{
		if (j == dataLength)
			break;
		loader_primary[label_addr][start+secOffset] = newval[j];
		
		temp += loader_primary[label_addr][start+secOffset];
		j++;
		start++;
	}
	if (j != dataLength)
	{
		label_addr = hexAdd(label_addr, "10");
		start = 0;
		for (;start + secOffset < BYTE*4;start++)
		{
			if (j == dataLength && secOffset==0)
				break;
			loader_primary[label_addr][start+secOffset] = newval[j];
			
			temp += loader_primary[label_addr][start];
			j+=1;
			
		}
	}
}

#include "loader_utilities.cpp"
int main()
{

	progaddress = "4000"; // default
	Input_file = "loader_input.txt";
	cout << "Enter relocation start address :  ";
	cin >> progaddress;
	string address, currline; // can be in hex

	fin.open(Input_file);
	fout.open("loader_intermediate.txt");
	int lineCount =0;
	while (getline(fin, currline))
	{
		
		lineCount++;
		switch (currline[0])
		{
		case 'H' :
			handle_H(currline);
			break;
		case 'D':
			handle_D(currline);
			break;
		case 'T':
			while (getline(fin, currline))
			{
				if (currline[0] == 'E' && currline.length() > 0)
				{
					csaddress = hexAdd(csaddress, cslth);
					break;
				}
			}
			break;
		default:
			break;
		}
		
		

	}
	fin.close();
	fout.close();

	cout << "loader_interm.txt file has been generated\n\n";
	vector<string> modifiedAddresses;
	string  csectname, csaddr;
	currline="";
	string s = "........";

	csaddress = progaddress;
	execaddr = progaddress;
	currline="";
	csectname="";
	string  beg, end, addr_text_starts, memlocColumn;
	
	int firstsection = 0;

	fin.open(Input_file);
	fout.open("loader_intermediate_2.txt");

	string buffer(BYTE*4, '0');

	while (getline(fin, currline)){

		if (currline[0] == 'H')
		{

			int i;
			i = 2;
			csectname = "";
			while (i < currline.length() && currline[i] != '^')
			{
				
				csectname += currline[i];
				i++;
			}

			if (firstsection != 1)
			{
				csaddress = progaddress;
				execaddr = csaddress;
				firstsection = 1;
				beg = removePreceding(nearestAddress(csaddress));
			}
			else
			{
				csaddress = estabMap[csectname].address;
				beg = removePreceding(nearestAddress(csaddress));
			}
			cslth = "";
			cslth = estabMap[csectname].len;

			if (beg != memlocColumn)
				fout << beg << TAB << s << TAB << s << TAB << s << TAB << s << endl;

			beg = removePreceding(hexAdd(beg, "10"));
		}
		else if (currline[0] == 'T')
		{

			int t = 2;
			addr_text_starts = "";
			vector<string> tokens = splitDelimiter(currline.substr(2),'^');

			addr_text_starts = tokens[0];
			t+=tokens[0].size();
			addr_text_starts = hexAdd(csaddress, addr_text_starts);
			t++;

			memlocColumn = removePreceding(nearestAddress(addr_text_starts));

			end = memlocColumn;

			string s = "........";
			while (beg != end)
			{
				fout << beg << TAB << s << TAB << s << TAB << s << TAB << s << "\n";
				beg = hexAdd(beg, "10");
			}
			fout << memlocColumn << TAB;

			string hexdiff = hexSub(addr_text_starts, memlocColumn);
			int start = calculateIndex(hexdiff);

			while (currline[t] != '^') t+=1;
			t++;

			for (int j = 0; j < start; j++)
			{
				fout << buffer[j];
				if ((j + 1) % BYTE != 1)
					fout << TAB;
			}

			while (t != currline.length())
			{
				int ft = 0, fsi = 0;
				while (start < BYTE*4 && t < currline.length() && currline[t] != '^')
				{
					buffer[start] = currline[t];
					fout << buffer[start];
					if ((start + 1) % BYTE == 0)
					{
						fout << TAB;
					}
					start++;
					t++;
				}
				if (!(t < currline.length()))
					break;

				
				{
					ft = (currline[t] == '^');
					fsi = (start == BYTE * 4);
				}

				if (ft==0 && fsi==1)
				{
					start = 0;
					memlocColumn = removePreceding(hexAdd(memlocColumn, "10"));
					fout << "\n"
						 << memlocColumn << TAB;
					fill(buffer.begin(), buffer.begin() + buffer.size(), '.');
				}
				else if (ft==1 && fsi==0)
				{
					t++;
				}
				else if (ft==1 && fsi==1)
				{
					start = 0;
					memlocColumn = removePreceding(hexAdd(memlocColumn, "10"));
					fout << "\n"
						 << memlocColumn << TAB;
					fill(buffer.begin(), buffer.begin() + buffer.size(), '.');
					t++;
				}
			}
			while (true)
			{
				fout << buffer[start];
				if (start == BYTE-1 || start == 2*BYTE-1 || start == 3*BYTE-1)
					fout << TAB;
				else if (start == 31)
					fout << endl;
				start++;
				if (start >= BYTE*4)
					break;
			}
			fill(buffer.begin(), buffer.begin() + buffer.size(), '0');
			beg = hexAdd(memlocColumn, "10");
		}
		else if (currline[0] == 'M')
			string operand_addr;
	}
	fin.close();
	fout.close();
	fin.open("loader_intermediate_2.txt");
	for(;;)
	{
		if (!getline(fin, currline))
			break;

		int i = 0;
		string saddr, arr;
		
		while (currline[i] != TAB)
		{
			saddr += currline[i];
			i++;
		}
		for (;i < currline.length();i++)
		{
			if (currline[i] == TAB)
				;
			else
			{
				arr += currline[i];
				
			}
		}
		loader_primary[saddr] = arr;
		helpermap[counter_addr] = saddr;
		counter_addr++;
	}
	fin.close();
	fin.open(Input_file);
	fin2.open("loader_interm.txt");

	for(;;)
	{
		if (!getline(fin, currline)) break;
		
		if (currline[0] == 'H')
		{

			int i = 2;
			csectname = "";
			while (currline[i] != '^')
			{
				csectname += currline[i];
				i++;
			}
			csaddr = estabMap[csectname].address;
		}
		else if (currline[0] == 'M')
		{
			int i = 2;
			int j=i;
			string addrToModify = "";
			while (currline[i] != '^' && i>0)
			{
				addrToModify += currline[i];
				i++;
			}
			string temp = addrToModify;
			addrToModify = hexAdd(addrToModify, csaddr);
			i++;
			string label_addr = removePreceding(nearestAddress(addrToModify));
			string offset = hexSub(addrToModify, label_addr);
			string num_half_bytes = "";

			while (currline[i] != '^')
			{
				num_half_bytes += currline[i];
				i++;
			}
			int no_hb = hexToInt(num_half_bytes);
			
			char operator_ = currline[i+1];
			string sym = "";
			i+=2;
			 for(;i < currline.length();i++)
			{
				sym += currline[i+j-2];
				
			}
			int offsetVal = hexToInt(offset) +hexToInt(offset);

			

			string operand = findOperand(offsetVal, no_hb, label_addr);

			final_val[addrToModify].no_half_bytes = (no_hb);

			string ans;
			if (operator_ == '+')
			{

				if (final_val[addrToModify].isPresent ==0)
				
				{
					final_val[addrToModify].val = operand;
					final_val[addrToModify].isPresent = 1;
				}
				
				j++;
				
				if (no_hb % 2 != 0)
					ans = hexAdd(final_val[addrToModify].val, estabMap[sym].address).substr(1);
				else
					ans = hexAdd(final_val[addrToModify].val, estabMap[sym].address);
				final_val[addrToModify].val = ans;
				j-=1;
			}
			else if (operator_ == '-')
			{
				if (final_val[addrToModify].isPresent != 1){
					final_val[addrToModify].val = operand;
					final_val[addrToModify].isPresent = 1;
				}
				ans = hexSub(final_val[addrToModify].val, estabMap[sym].address);
				if (no_hb % 2 !=0)
					ans = ans.substr(1, ans.length() - 1);
				final_val[addrToModify].val = ans;
			}
			else
				cout << "Invalid operator\n";
			modifiedAddresses.push_back(addrToModify);
		}
		else if (currline[0] == 'E')
		{
			int sec=1;
			// place correct values in map
			for (int i = 0; i < modifiedAddresses.size(); i++)
			{
				string label_addr = removePreceding(nearestAddress(modifiedAddresses[i]));
				string offset = hexSub(modifiedAddresses[i], label_addr);
				int offsetVal = hexToInt(offset) * 2*sec;
				string finaldata = final_val[modifiedAddresses[i]].val;
				int no_half_bytes = final_val[modifiedAddresses[i]].no_half_bytes;
				string temp = "";
				if (finaldata.length() > no_half_bytes && sec>0)
				{
					temp = finaldata;
					reverse(temp.begin(), temp.end());
					temp = temp.substr(0, no_half_bytes);
					reverse(temp.begin(), temp.end());
					finaldata = temp;
				}

				if (finaldata.length() % 2 !=sec-1)
					offsetVal += sec;
				updateloaderPrimary(label_addr, finaldata.length(), finaldata, offsetVal);
			}
			modifiedAddresses.clear();
		}
	}
	fin.close();
	fin2.close();
	fout.close();

	fout.open("LOADER_OUTPUT_FINAL.txt");
	for (int t = 0; t < helpermap.size(); t++)
	{
		string templine = loader_primary[helpermap[t]];
		fout << helpermap[t] << TAB << clean(templine.substr(0, BYTE)) << TAB << clean(templine.substr(BYTE, BYTE)) << TAB << clean(templine.substr(BYTE*2, BYTE)) << TAB << clean(templine.substr(BYTE*3, BYTE)) << "\n";
	}
	fout.close();
	cout << "\n Output file has been generated\n";
}
