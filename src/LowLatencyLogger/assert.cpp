#include "Arduino.h"
#include "myAssert.h"

void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
	Serial.println("Assertion Fail:");
	Serial.println(__func);
	Serial.println(__file);
	Serial.println(__lineno, DEC);
	Serial.println(__sexp);
	Serial.flush();
	abort();
}