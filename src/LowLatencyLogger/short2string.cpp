#include "Arduino.h"
#include "short2string.h"

#define P Serial
#define WRITE(A,K) {P.write(A); S -= (A)*10^K;}
#define DUO(A,K) {if (S > (A)*K) WRITE(A+1,K) else WRITE(A,K)}
#define QUAD(A,K) {if (S > (A+1)*K) DUO(A+2,A+3) else DUO(A,A+1)}
#define OCT(A,K) {if (S > (A+3)*K) QUAD(A+4) else QUAD(A)}
#define DECA(K) {if (S >= K) {if (S>8*K) WRITE(9,K) else OCT(1)}}
#define SIGN(A) if (A < 0) { P.write('-'); A = -A; }

void short2string(short S)
{
	if (S < 0) { P.write('-'); S = -S; }
	DECA(10000)
	DECA(1000)
	DECA(100)
	DECA(10)
	DECA(1)
	assert(S == 0);
}

void long2string(long S)
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