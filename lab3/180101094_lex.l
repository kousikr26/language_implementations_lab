%{
// Token coding scheme
#define PROGRAM 1
#define VAR 2
#define BEGINPROGRAM 3
#define END 4
#define EOFCODE 5
#define INTEGER 6
#define FOR 7
#define READ 8
#define WRITE 9
#define TO 10
#define DO 11
#define SEMI 12
#define COLON 13
#define COMMA 14
#define COLEQ 15
#define PLUS 16
#define MINUS 17
#define TIMES 18
#define DIV 19
#define LPAREN 20
#define RPAREN 21
#define ID 22
#define INT 23


#include <bits/stdc++.h>
using namespace std;

// Lex variables
int yylval;

unordered_map <int,string> symbol_table;
int tableIndex = 0, yyline = 1, yycolumn = 1;


int install_id(); // Install ID
int install_num(); // Install num

%}
%option noyywrap

delim   [ \t]
ws      {delim}+
letter [a-zA-Z]
digit  [0-9]
id     {letter}({letter}|{digit})*
number {digit}+



%%

\n						{yyline++; yycolumn = 1;}
[ \t]+					{yycolumn += yyleng;}

(PROGRAM|program)					{yycolumn += yyleng; return PROGRAM;}
(VAR|var)						{yycolumn += yyleng; return VAR;}
(BEGIN|begin)					{yycolumn += yyleng; return BEGINPROGRAM;}
(END|end)\.						{yycolumn += yyleng;return EOFCODE;}
(END|end)						{yycolumn += yyleng;return END;}
(INTEGER|integer)					{yycolumn += yyleng; return INTEGER;}
(FOR|for)						{yycolumn += yyleng; return FOR;}
(READ|read)					{yycolumn += yyleng; return READ;}
(WRITE|write)					{yycolumn += yyleng; return WRITE;}
(TO|to)						{yycolumn += yyleng; return TO;}
(DO|do)					{yycolumn += yyleng; return DO;}
(DIV|div)						{yycolumn += yyleng; return DIV;}

[a-zA-Z]+[0-9]*			{yylval = install_id(); return ID;}
[0-9]+					{yylval = install_num(); yycolumn += yyleng; return INT;}



";"						{yycolumn += yyleng; return SEMI;}
":"						{yycolumn += yyleng; return COLON;}
","						{yycolumn += yyleng; return COMMA;}
"."						{yycolumn += yyleng; return EOFCODE;}
"("						{yycolumn += yyleng; return LPAREN;}
")"						{yycolumn += yyleng; return RPAREN;}

"-"						{yycolumn += yyleng; return MINUS;}
"+"						{yycolumn += yyleng; return PLUS;}
"*"						{yycolumn += yyleng; return TIMES;}
":="					{yycolumn += yyleng; return COLEQ;}
.						{string s(yytext); cout << "Incorrect character \'" + s + "\'" << ", line=" << yyline << " column=" << yycolumn << endl; yycolumn += yyleng;}



%%



int install_id(){ 
	// Inserts into hash table (symbol_table)
	string s(yytext); 
	symbol_table[tableIndex++]=s;
	return tableIndex-1;
}
int install_num(){
	// Inserts into hash table (symbol_table)
	string s(yytext);
	symbol_table[tableIndex++]=s;
	return tableIndex-1;
}

int main()
{	
	int i;
	int prevline = 0;
	while((i=yylex()) != EOFCODE){
		
		// Read line by line and print output in format

		string lineNo = yyline!=prevline ? to_string(yyline) : ""; //Change line number
		prevline=yyline;
		if(i==ID)	cout << setw(4)<< lineNo <<" "<<setw(10) << i << setw(10)<<"^" << symbol_table[yylval] << endl;
		else if (i==INT) 	cout << setw(4)<< lineNo <<" "<< setw(10) << i << setw(10)<<"#" << symbol_table[yylval] << endl; 
		else cout << setw(4)<< lineNo <<" "<< setw(10) << i << endl; 
		
		
	}
	cout << setw(4)<<yyline <<" "<<setw(10) << EOFCODE << endl; // Print EOFCODE
}



