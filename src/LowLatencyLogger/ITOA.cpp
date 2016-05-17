#include "Arduino.h"
#include "ITOA.h"

//#define DEBUG_ITOA
#ifdef  DEBUG_ITOA
#define DEBUG_ITOA_OUTPUT Serial.print(" ... "); Serial.print(S);
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
	Serial.print("itoa(short ");
	Serial.print(S);
	Serial.print(")");
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
	Serial.println(" DONE");
	Serial.flush();
#endif
}

void ITOA::itoa(long S)
{
	SIGN(S);
	DECA(1000000000);
	DECA(100000000);
	DECA(10000000);
	DECA(1000000);
	DECA(100000);
	DECA(10000);
	DECA(1000);
	DECA(100);
	DECA(10);
	DECA(1);
}