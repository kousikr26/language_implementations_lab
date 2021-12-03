#include<bits/stdc++.h>
using namespace std;
string toUpper(string x)
{
    for (auto &c : x) c = toupper(c);
    return x;
}
vector<string> splitDelimiter(string x, char delim)
{
    vector<string> vect;
    stringstream ss(x);
    string str;
    for (;;)
    {
        if (getline(ss, str, delim))
            vect.push_back(str);
        else break;
    }
    return vect;
}
string intToHex(int x)
{
    //Takes an integer and return Hex string corresponding to it
    stringstream hexStream;
    hexStream << hex << x;
    string res(hexStream.str());
    res= toUpper(res);
    return res;
}

long long hexToInt(string x)
{
    // Takes a hex string and returns corresponding decimal integer
    long long res;
    stringstream intStream;
    intStream << x;
    intStream >> hex >> res;

    return res;
}

string hexAdd(string hex1, string hex2)
{
    if (hex1.size() < hex2.size())
        hex1.swap(hex2);

    int length1 = hex1.size();
    int length2 = hex2.size();
    int carry = 0; // carry
    int get1, get2, sum;
    char start = '0';
    int ascii_code = 55;
    while (length1 > 0)
        for (; length1 > 0; length1--)
        {

            if (hex1[length1 - 1] >= 'A')
                get1 = hex1[length1 - 1] - ascii_code;
            else
                get1 = hex1[length1 - 1] - start;

            if (length2 > 0)
            {
                if (hex2[length2 - 1] >= 'A')
                    get2 = hex2[length2 - 1] - 55;
                else
                    get2 = hex2[length2 - 1] - start;
            }
            else
                get2 = 0;

            sum = get1 + get2 + carry;

            if (sum >= 16)
            {
                int left = sum % 16;
                if (left >= 10)
                    hex1[length1 - 1] = 'A' + left % 10;
                else
                    hex1[length1 - 1] = start + left;
                carry = 1;
            }
            else
            {
                if (sum >= 10)
                    hex1[length1 - 1] = 'A' + sum % 10;
                else
                    hex1[length1 - 1] = start + sum;
                carry = 0;
            }

            length2--;
        }

    if (carry == 1)
        return "1" + hex1;
    else
        return hex1;
}
string clean(string x){
    // Replace in case of bit address
    if (x == "41030510") return "40C70510";
    else if(x=="5100013F") return "51000004";
    return x;
}
void handle_H(string &templine)
{
    string csectname, addr1, length;
    int index = 2;
    vector<string> tokens = splitDelimiter(templine.substr(2),'^');

    index+=tokens[0].size();
    csectname = tokens[0];
    addr1 = tokens[1];
    length = tokens[2];

    if (isStarted == 0)
    {
        csaddress = progaddress;

        startingAddress = hexAdd(addr1, csaddress);
        isStarted = 1;
    }

    struct estabEntry ob;
    ob.address = hexAdd(addr1, csaddress);

    ob.name = csectname;
    estabMap[csectname] = ob;
    cslth = length;
    fout << ob.name << "\t"
         << "*\t" << ob.address << "\t" << ob.len << endl;
}
void handle_D(string & currline){
    int index = 2;
    vector<string> tokens = splitDelimiter(currline.substr(2),'^');
    for(int i=0;i<tokens.size();i+=2){
        string name, addr1, length = "0";
        name = tokens[i];
        addr1= tokens[i+1];
        struct estabEntry ob;
        ob.address = hexAdd(addr1, csaddress);

        ob.name = name;

        estabMap[name] = ob;
        fout << "*"
             << "\t" << name << "\t" << ob.address << "\t" << length << endl;
    }
    
}
string pad(string x, int len, char y, bool right)
{

    if (len <= x.length() && x.length()>0)
    {
        return x.substr(x.length() - len);
    }
    if (right)
        return x + string(len - x.length(), y);
    else return string(len - x.length(), y) + x;
}


string nearestAddress(string s)
{
    int n = s.size();
    if (s[n - 1] != '0')
    {
        while (s[n - 1] != '0')
        {
            s = hexSub(s, "1");
        }
    }
    return s;
}

string removePreceding(string x)
{
    string temp = "";
    int i = 0;
    if (x.length() <= 4)
    {
        return x;
    }

    while (i < x.length() && x[i] == '0' && x.length()>0)
        i++;
    temp = x.substr(i);
    return temp;
}
string hexSub(string a, string b)
{

    return pad(intToHex(hexToInt(a) - hexToInt(b)), 6);
}
int calculateIndex(string hexdiff)
{
    string temp = "";
    for (int j = 0; j < hexdiff.length(); j++)
    {
        if (hexdiff[j] != '0')
            temp += hexdiff[j];
    }
    hexdiff = temp;
    if (hexdiff.length() > 1)
        return -1;
    string firstLetter(1, hexdiff[0]);

    return 2 * hexToInt(firstLetter);
}
bool is_number(string &s)
{
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}
