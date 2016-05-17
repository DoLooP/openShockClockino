#include "Arduino.h"
#include "ITOA.h"
// #define ASSERT_ENABLED
#include "myAssert.h"

#define WRITE(A,K) {write('0'+A); S -= (A)*K;}
#define DUO(A,K) {if (S > (A)*K) WRITE(A+1,K) else WRITE(A,K)}
#define QUAD(A,K) {if (S > (A+1)*K) DUO(A+2,K) else DUO(A,K)}
#define OCT(A,K) {if (S > (A+3)*K) QUAD(A+4,K) else QUAD(A,K)}
#define DECA(K) {if (S >= K) {if (S>8*K) WRITE(9,K) else OCT(1,K)}}
#define SIGN(A) if (A < 0) { write('-'); A = -A; }

#define DEBUG_ITOA

void ITOA::itoa(short S)
{
#ifdef DEBUG_ITOA
	Serial.print("itoa(short ");
	Serial.print(S);
	Serial.print(") ...");
	Serial.flush();
#endif
	if (S < 0) { write('-'); S = -S; }
#ifdef DEBUG_ITOA
	Serial.print(S);
	Serial.print(" ... ");
#endif
	DECA(10000)
#ifdef DEBUG_ITOA
	Serial.print(S);
	Serial.print(" ... ");
#endif
	DECA(1000)
#ifdef DEBUG_ITOA
		Serial.print(S);
	Serial.print(" ... ");
#endif
	DECA(100)
#ifdef DEBUG_ITOA
		Serial.print(S);
	Serial.print(" ... ");
#endif
	DECA(10)
#ifdef DEBUG_ITOA
		Serial.print(S);
	Serial.print(" ... ");
#endif
	DECA(1)
	assert(S == 0);
#ifdef DEBUG_ITOA
	Serial.print(S);
	Serial.println(" DONE");
	Serial.flush();
#endif

}

void ITOA::itoa(long S)
{
	SIGN(S)
	DECA(1000000000)
	DECA(100000000)
	DECA(10000000)
	DECA(1000000)
	DECA(100000)
	DECA(10000)
	DECA(1000)
	DECA(100)
	DECA(10)
	DECA(1)
	assert(S == 0);
}