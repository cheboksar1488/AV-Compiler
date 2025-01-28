#include <stdio.h>
#define CMPEND						1
#define EXIT						1
#define ERR							-1
#define CMDCOUNT					17
#define WORDBUFF_SIZE_MACRO			10
#define KEYBYTEMACROS				64
#define NONE						0
#define FLAGS_COUNT_MACRO			1
#define FILENAME_BUFF_SIZE_MACRO	255
#define DEBUG(arg) (printf("%d\n", arg))
#define DEBUG_CODE 0

typedef unsigned char byte;
typedef char sbyte;

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
byte pow(const byte _BASE, const byte _EXP) {
	byte result = 1	;
	for (byte i = NONE; i < _EXP; i++) {
		result *= _BASE;
	}
	return result;
}
byte getlenstr(const char *_MASSIVE) {
	byte result;
	for (result = NONE; _MASSIVE[result] != '\0'; result++) {}
	return result;
}
void showbuff(const char *_MASSIVE, const int BUFFERSIZE) {
	for (byte i = NONE; i < BUFFERSIZE; i++) {
		printf("| %c ", _MASSIVE[i]);
	}
	printf("\n");
}
void clearbuff(char *_BUFFER, const int BUFFERSIZE) {
	for (byte i = NONE; i < BUFFERSIZE; i++) {
		_BUFFER[i] = '\0';
	}
}
sbyte get_index(const char *_wordbuff_massive, const char **_base, const byte _basesize) {
	for (byte i = NONE; i < _basesize; i++) {
		byte baselen = getlenstr(_base[i]);
		byte j = NONE;
		for (j; j < baselen; j++) {
			if (_wordbuff_massive[j] != _base[i][j]) {
				break;
			}
		}
		if (j == baselen) {
			return i;
		}
	}
	return ERR;
}
byte compile(FILE *basefile, FILE *outputfile) {
	char wordbuff[WORDBUFF_SIZE_MACRO] = "";
	char charflow;
	sbyte wordbuffptr = NONE;
	byte currcode = NONE;
	byte reg = NONE;
	byte step = NONE;
	byte bytec = NONE;

	while ((charflow = getc(basefile)) != EOF) {
		if (charflow == ';') {
			showbuff(wordbuff, WORDBUFF_SIZE_MACRO);
			return CMPEND;
		}
		if (step == 0) {
			if (currcode == NONE && (charflow != ' ' && charflow != '\n' && charflow != EOF)) {
				wordbuff[wordbuffptr] = charflow;
#if DEBUG_CODE==1
				showbuff(wordbuff, WORDBUFF_SIZE_MACRO);
#endif
				wordbuffptr++;
				continue;
			}
			if ((charflow == ' ' || charflow == '\n' || charflow == EOF) && currcode == NONE) {
				wordbuff[wordbuffptr] = '\0';
				sbyte keyindex = get_index(wordbuff, KEYWORD, CMDCOUNT);
				if (keyindex == ERR) { printf("Syntax error.\nBUFFER:\n"); showbuff(wordbuff, WORDBUFF_SIZE_MACRO); return EXIT; }
				currcode = KEYBYTE + (byte)(8 * keyindex);
				reg = REG[keyindex];
				bytec = BYTE[keyindex];
				if (reg == NONE && bytec == NONE && currcode != NONE) {
					fprintf(outputfile, "%d\n", currcode);
					clearbuff(wordbuff, WORDBUFF_SIZE_MACRO);
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
				clearbuff(wordbuff, WORDBUFF_SIZE_MACRO);
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
					clearbuff(wordbuff, WORDBUFF_SIZE_MACRO);
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
byte extensionchecker(char *argv, char *extension) {
	byte extlen = getlenstr(extension);
	byte argvlen = getlenstr(argv);
	if (extlen >= argvlen) {
		return 0;
	}
	byte match = 0;
	for (byte i = 0; i < extlen; i++) {
		if (argv[argvlen - extlen + i] != extension[i]) {
			match = 0;
			break;
		}
		match++;
	}
	return match == extlen ? 1:0;
}
void strcopy(char *acceptor, const char *donor) {
	byte i = 0;
	for (i; i < getlenstr(donor); i++) {
		acceptor[i] = donor[i];
	}
	acceptor[i] = '\0';
}
void copyname(char *acceptor, const char *donor) {
	byte i = 0;
	for (i; i < getlenstr(donor); i++) {
		if (donor[i] == '.') {
			break;
		}
		acceptor[i] = donor[i];
	}
	acceptor[i] = '\0';
}
void strcut(char *base, const char *arg) {
	byte baselen = getlenstr(base);
	byte arglen = getlenstr(arg);
	for (byte i = 0; i < arglen; i++) {
		base[baselen + i] = arg[i];
	}
	base[baselen + arglen] = '\0';
}
sbyte cmd_parser(const int argc, const char *argv[], const char **flags, const char **extensions, char *filename_input_buffer, char *filename_output_buffer) {
	if (argc < 2) {
		printf("No input filename.\n");
		return ERR;	
	}
	if (!extensionchecker(argv[1], extensions[0])) {
		strcopy(filename_input_buffer, argv[1]);
		strcut(filename_input_buffer, extensions[0]);
	}
	else {
		strcopy(filename_input_buffer, argv[1]);
	}
	for (byte i = 2; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (get_index(argv[i], flags, FLAGS_COUNT_MACRO))
			{
			case -1:
				printf("Unknow flag.\n");
				continue;
			case 0:
				if ((i+1)>=argc) {
					printf("No input filename output.\n");
					clearbuff(filename_output_buffer, FILENAME_BUFF_SIZE_MACRO);
					copyname(filename_output_buffer, argv[1]);
					strcut(filename_output_buffer, extensions[1]);
					continue;
				}
				if (!extensionchecker(argv[i+1], extensions[1])) {
					clearbuff(filename_output_buffer, FILENAME_BUFF_SIZE_MACRO);
					strcopy(filename_output_buffer, argv[i + 1]);
					strcut(filename_output_buffer, extensions[1]);
				}
				continue;
			default:
				break;
			}
		}
	}
	return EXIT;
}
int main(int argc, char **argv) {
	char filename_input_buffer[FILENAME_BUFF_SIZE_MACRO] = "";
	char filename_output_buffer[FILENAME_BUFF_SIZE_MACRO] = "o.byte";
	const char *extensions[] = {
		".avm",
		".byte"
	};
	const char* flags[FLAGS_COUNT_MACRO] = {
		"-o"
	};
	if (cmd_parser(argc, argv, flags, extensions, filename_input_buffer, filename_output_buffer) == ERR) {
		return EXIT;
	}
	FILE *file, *outputfile;
	fopen_s(&file, filename_input_buffer, "r");
	fopen_s(&outputfile, filename_output_buffer, "w");
	if (file == NULL || outputfile == NULL) {
		if (file) { printf("file"); fclose(file); };
		if (outputfile) { printf("outputfile\n"); fclose(outputfile); };
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
