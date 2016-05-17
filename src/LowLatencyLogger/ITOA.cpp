#include "Arduino.h"
#include "ITOA.h"

//#define DEBUG_ITOA
#ifdef  DEBUG_ITOA
#define DEBUG_ITOA_OUTPUT Serial.print(F(" ... "); Serial.print(S);
#else
#define DEBUG_ITOA_OUTPUT ;
#endif

#define WRITE(A,K) {write('0'+A); S -= (A)*K; DEBUG_ITOA_OUTPUT}
#define DUO(A,K) {if (S >= (A+1)*K) WRITE(A+1,K) else WRITE(A,K)}
#define QUAD(A,K) {if (S >= (A+2)*K) DUO(A+2,K) else DUO(A,K)}
#define OCT(A,K) {if (S >= (A+4)*K) QUAD(A+4,K) else QUAD(A,K)}
#define DECA(K) {if (S >= 8*K) DUO(8,K) else OCT(0,K)}
#define SIGN(A) if (A < 0) { write('-'); A = -A; DEBUG_ITOA_OUTPUT}

void ITOA::itoa(short S)
{
#ifdef DEBUG_ITOA
	Serial.print(F("itoa(short ");
	Serial.print(S);
	Serial.print(F(")");
	Serial.flush();
#endif
	SIGN(S);
	 if (S < 100) { if (S < 10) goto size1; else goto size2; }
	 if (S < 10000) { if (S < 1000) goto size3; else goto size4; }
	DECA(10000);
size4:
	DECA(1000);
size3:
	DECA(100);
size2:
	DECA(10);
size1:
	DECA(1);
#ifdef DEBUG_ITOA
	Serial.println(F(" DONE");
	Serial.flush();
#endif
}

void ITOA::itoa(long S)
{
#ifdef DEBUG_ITOA
	Serial.print(F("itoa(long ");
	Serial.print(S);
	Serial.print(F(")");
	Serial.flush();
#endif
	SIGN(S);
	if (S < 100000)
	{
		if (S < 100) { if (S < 10) goto size1; else goto size2; }
		if (S < 10000) { if (S < 1000) goto size3; else goto size4; }
		goto size5;
	} else {
		if (S < 10000000) { if (S < 1000000) goto size6; else goto size7; }
		if (S < 1000000000) { if (S < 100000000) goto size8; else goto size9; }
	}
	DECA(1000000000);
size9:
	DECA(100000000);
size8:
	DECA(10000000);
size7:
	DECA(1000000);
size6:
	DECA(100000);
size5:
	DECA(10000);
size4:
	DECA(1000);
size3:
	DECA(100);
size2:
	DECA(10);
size1:
	DECA(1);
#ifdef DEBUG_ITOA
	Serial.println(F(" DONE");
	Serial.flush();
#endif
}