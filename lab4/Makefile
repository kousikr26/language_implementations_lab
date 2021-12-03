all: 180101094_lexer.l  180101094_parser.y
	flex 180101094_lexer.l
	yacc -d 180101094_parser.y
	g++ y.tab.c lex.yy.c -o parser

clean:
	rm -f parser lex.yy.c y.tab.c y.tab.h tokenizer

