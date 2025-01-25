#include <stdio.h>
#define CMPEND					1
#define EXIT					1
#define ERR						-1
#define CMDCOUNT				17
#define BUFFSIZEMACROS			10
#define KEYBYTEMACROS			64
#define NONE					0

typedef unsigned char byte;
typedef char sbyte;

const byte BUFFSIZE = BUFFSIZEMACROS;
const byte KEYBYTE = KEYBYTEMACROS;
const char *KEYWORD[CMDCOUNT] = {
	"LDA",			//01 000	0
	"LDI",			//01 001	1
	"STR",			//01 010	2
	"ADD",			//01 011	3
	"SUB",			//01 100	4
	"AND",			//01 101	5
	"OR",			//01 110	6
	"NOT",			//01 111	7
	"EQUAL",		//10 000	8
	"NOEQUAL",		//10 001	9
	"LESS",			//10 010	10
	"LEQUAL",		//10 011	11
	"ACI",			//10 100	12
	"ACO",			//10 101	13
	"JUMP",			//10 110	14
	"-",			//10 111	15
	"RTR"			//11 RRR	16
};
const byte BYTE[CMDCOUNT] = {
	2,		//LDA				0
	1,		//LDI				1
	2,		//STR				2
	0,		//ADD				3
	0,		//SUB				4
	0,		//AND				5
	0,		//OR				6
	0,		//NOT				7
	2,		//EQUAL				8
	2,		//NOT EQUAL			9
	2,		//LESS				10
	2,		//LESS OR EQUAL		11
	0,		//ACI				12
	0,		//ACO				13
	2,		//JMP				14
	0,		//-					15
	0		//RTR				16
};
const byte REG[CMDCOUNT] = {
	1,		//LDA				0
	1,		//LDI				1
	1,		//STR				2
	1,		//ADD				3
	1,		//SUB				4
	1,		//AND				5
	1,		//OR				6
	0,		//NOT				7
	1,		//EQUAL				8
	1,		//NOT EQUAL			9
	1,		//LESS				10
	1,		//LESS OR EQUAL		11
	1,		//ACI				12
	1,		//ACO				13
	0,		//JMP				14
	0,		//-					15
	2,		//RTR				16
};
byte pow(const byte _BASE, const byte _LVL) {
	byte result = 1	;
	for (byte i = NONE; i < _LVL; i++) {
		result *= _BASE;
	}
	return result;
}
byte getlenstr(const char *_MASSIVE) {
	byte result;
	for (result = NONE; _MASSIVE[result] != '\0'; result++) {}
	return result;
}
void showbuff(const char *_MASSIVE) {
	for (byte i = NONE; i < BUFFSIZE; i++) {
		printf("| %c ", _MASSIVE[i]);
	}
	printf("\n");
}
void clearbuff(char *_MASSIVE) {
	for (byte i = NONE; i < BUFFSIZE; i++) {
		_MASSIVE[i] = '\0';
	}
}
byte keywordindex(const char* _WORDBUFFMASSIVE) {
	for (byte i = NONE; i < CMDCOUNT; i++) {
		byte keylen = getlenstr(KEYWORD[i]);
		byte j = NONE;
		for (j = NONE; j < keylen; j++) {
			if (_WORDBUFFMASSIVE[j] != KEYWORD[i][j]) {
				break;
			}
		}
		if (j == keylen) {
			return (int)i;
		}
	}
	return ERR;
}
byte compile(FILE *basefile, FILE *outputfile) {
	char wordbuff[10] = "";
	char charflow;
	sbyte wordbuffptr = NONE;
	byte currcode = NONE;
	byte reg = NONE;
	byte step = NONE;
	byte bytec = NONE;

	while ((charflow = getc(basefile)) != EOF) {
		if (charflow == ';') {
			showbuff(wordbuff);
			return CMPEND;
		}
		if (step == 0) {
			if (currcode == NONE && (charflow != ' ' && charflow != '\n' && charflow != EOF)) {
				wordbuff[wordbuffptr] = charflow;
				wordbuffptr++;
				showbuff(wordbuff);
				continue;
			}
			if ((charflow == ' ' || charflow == '\n' || charflow == EOF) && currcode == NONE) {
				wordbuff[wordbuffptr] = '\0';
				int keyindex = keywordindex(wordbuff);
				if (keyindex == ERR) { printf("Syntax error.\nBUFFER:\n"); showbuff(wordbuff); return ERR; }
				currcode = KEYBYTE + (byte)(8 * keyindex);
				reg = REG[keyindex];
				bytec = BYTE[keyindex];
				clearbuff(wordbuff);
				if (reg == NONE && bytec == NONE && currcode != NONE) {
					fprintf(outputfile, "%d\n", currcode);
					clearbuff(wordbuff);
					wordbuffptr = NONE;
					reg = NONE;
					bytec = NONE;
					currcode = NONE;
					step = NONE;
					continue; 
				}
				if (reg == NONE && bytec != NONE && currcode != NONE) {
					fprintf(outputfile, "%d ", currcode);
					step++;
					continue;
				}
				continue;
			}
			if (charflow >= '0' && charflow <= '9' && reg > NONE) {
				currcode += (byte)(pow(8, reg - 1) * (charflow - '0'));
				reg--;
				if (reg == NONE && currcode != NONE && bytec != NONE) {
					fprintf(outputfile, "%d ", currcode);
					step++;
					continue;
				}
				continue;
			}
			if (reg == NONE && currcode != NONE && bytec == NONE) {
				fprintf(outputfile, "%d\n", currcode);
				clearbuff(wordbuff);
				wordbuffptr = NONE;
				reg = NONE;
				bytec = NONE;
				currcode = NONE;
				step = NONE;
				continue;
			}
			continue;
		}
		if (step == 1) {
			if (charflow >= '0' && charflow <= '9' && bytec > NONE) {
				fprintf(outputfile, "%c", charflow);
				wordbuffptr = -1;
				continue;
			}
			if ((charflow == ' ' || charflow == '\n' || charflow == EOF) && wordbuffptr == -1) {
				fprintf(outputfile, " ");
				wordbuffptr = NONE;
				bytec--;
				if (bytec == NONE) {
					fprintf(outputfile, "\n");
					clearbuff(wordbuff);
					wordbuffptr = NONE;
					reg = NONE;
					bytec = NONE;
					currcode = NONE;
					step = NONE;
					continue;
				}
				continue;
			}
			continue;
		}
		continue;
	}
	return CMPEND;
}
int main(int argc, char *argv[]) {
	if (!argv[1]) {
		printf("No input file name.\n");
		return EXIT;
	}
	FILE *file, *outputfile;
	fopen_s(&file, argv[1], "r");
	fopen_s(&outputfile, "o.byte", "w");
	if (file == NULL || outputfile == NULL) {
		if (file) fclose(file);
		if (outputfile) fclose(outputfile);
		printf("File open error.\n");
		return EXIT;
	}
	byte compilestatus = compile(file, outputfile);
	fclose(file);
	fclose(outputfile);
	if(compilestatus != ERR)
		printf("File compiled.\n");
	return (int)compilestatus;
}