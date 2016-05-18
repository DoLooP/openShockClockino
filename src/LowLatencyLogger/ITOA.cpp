#include "Arduino.h"
#include "ITOA.h"

//#define DEBUG_ITOA
#ifdef  DEBUG_ITOA
#define DEBUG_ITOA_OUTPUT Serial.print(F(" ... ")); Serial.print(S);
#else
#define DEBUG_ITOA_OUTPUT ;
#endif

#define WRITE(A,K) {write('0'+A); S -= (A)*K; DEBUG_ITOA_OUTPUT}
#define DUO(A,K) {if (S >= (A+1)*K) WRITE(A+1,K) else WRITE(A,K)}
#define QUAD(A,K) {if (S >= (A+2)*K) DUO(A+2,K) else DUO(A,K)}
#define OCT(A,K) {if (S >= (A+4)*K) QUAD(A+4,K) else QUAD(A,K)}
#define DECA(K) {if (S >= 8*K) DUO(8,K) else OCT(0,K)}
#define SIGN(A) {if (A < 0) { write('-'); A = -A; DEBUG_ITOA_OUTPUT}}

void ITOA::itoa(long S) { itoa(S, true); }
void ITOA::itoa(unsigned long S) { itoa(S, false); }
void ITOA::itoa(short S) { itoa(S, true); }
void ITOA::itoa(unsigned short S) { itoa(S, false); }

const unsigned long pow10[] = {
	1
	,10
	,100
	,1000	// K
	,10000
	,100000
	,1000000	// M
	,10000000
	,100000000
	,1000000000		// B
};

void ITOA::itoa(long S, bool sign)
{
#ifdef DEBUG_ITOA
	Serial.print(F("itoa("));
	if (sign)
		Serial.print(F("signed "));
	else
		Serial.print(F("unsigned "));
	Serial.print(S);
	Serial.print(F(")"));
	Serial.flush();
#endif
	if (sign)
		SIGN(S);
	int size;
	if (S < 100000)
	{
		if (S < 100) { if (S < 10) size = 1; else size = 2; }
		else if (S < 10000) { if (S < 1000) size = 3; else size = 4; }
		else size = 5;
	}
	else {
		if (S < 10000000) { if (S < 1000000) size = 6; else size = 7; }
		else if (S < 1000000000) { if (S < 100000000) size = 8; else size = 9; }
	}
#ifdef DEBUG_ITOA
	Serial.print(F("size:"));
	Serial.print(size);
	Serial.print(F(" "));
#endif
	while (size--) {
		DECA(pow10[size]);
	}

#ifdef DEBUG_ITOA
	Serial.println(F(" DONE"));
	Serial.flush();
#endif
}