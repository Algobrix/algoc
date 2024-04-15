#include <algoC.h>


void application(ALGOC_APP)
{
	//enterNumber() enterString() enterChar()
	writeLine(ALGOC,"enter number");
	int number = enterNumber(ALGOC);
	write(ALGOC,"the number you entered is ");
	writeLine(ALGOC,number);


	writeLine(ALGOC,"enter char");
	char oneChar = enterChar(ALGOC);
	write(ALGOC,"the char you entered is ");
	writeLine(ALGOC,oneChar);


	writeLine(ALGOC,"enter string");
	char *multipleChars = enterString(ALGOC);
	write(ALGOC,"the string you entered is ");
	writeLine(ALGOC,multipleChars);
}
