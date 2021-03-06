#include "Arduino.h"
#include <SdFat.h>
#include "writeBuffer.h"
#include "myAssert.h"

//#define DEBUG_WRITEBUFFER
#ifdef DEBUG_WRITEBUFFER
#define DEBUG_WRITEBUFFER_FLUSH
#define DEBUG_WRITEBUFFER_RESET
#define DEBUG_WRITEBUFFER_WRITECSTR
#define DEBUG_WRITEBUFFER_WRITECHAR
#endif

WriteBuffer::WriteBuffer(uint8_t *buffer, size_t size, FlushFonctionPtr fp) : str(buffer), strSize(size), flushCallBack(fp)
{
	reset();
}

void WriteBuffer::internalsToSerial()
{
	Serial.print((unsigned long)str, HEX);
	Serial.write(',');
	Serial.print(p - str);
}

void WriteBuffer::flush()
{
	assert(p > str);
	assert(p - str <= strSize);
#ifdef DEBUG_WRITEBUFFER_FLUSH
	Serial.println();
	Serial.print(F("WriteBuffer::flush():"));
	internalsToSerial();
	Serial.println(F("["));
	Serial.write(str, p - str);
	Serial.println();
	Serial.println(F("]"));
	delay(250);
	Serial.flush();
#endif
	totalWriten += p - str;
	flushCallBack();
	reset();
#ifdef DEBUG_WRITEBUFFER_FLUSH
	Serial.println(F("WriteBuffer::flush() DONE")); Serial.flush();
#endif
	return;
}

size_t WriteBuffer::reset()
{
#ifdef DEBUG_WRITEBUFFER_RESET
	Serial.print(F("WriteBuffer::reset() "));
#endif
	size_t s = p - str;
	p = str;
#ifdef DEBUG_WRITEBUFFER_RESET
	internalsToSerial();
	Serial.println(F(" DONE"));
#endif 
	return s;
}

size_t WriteBuffer::write(const uint8_t *buffer, size_t l)
{
#ifdef DEBUG_WRITEBUFFER_WRITECSTR
	Serial.print(F("WriteBuffer::writeCSTR("));
	Serial.print((unsigned long)buffer, HEX);
	Serial.print(F(","));
	Serial.print(l);
	Serial.print(F(") "));
	internalsToSerial();
	Serial.print(F(" ..."));
	Serial.flush();
#endif
	int sizeUsed = p - str;
	while (sizeUsed + l > strSize)
	{
		int sizeLeft = strSize - sizeUsed;
		memcpy(p, buffer, sizeLeft);
		p += sizeLeft;
		flush();
		buffer += sizeLeft;
		l -= sizeLeft;
		sizeUsed = 0;
	}
	assert(l<strSize);
	memcpy(p, buffer, l);
	p += l;
#ifdef DEBUG_WRITEBUFFER_WRITECSTR
	Serial.println(F(" DONE"));
	Serial.flush(); 
#endif
	return p - str;
}

size_t WriteBuffer::write(uint8_t c)
{
#ifdef DEBUG_WRITEBUFFER_WRITECHAR
	Serial.print(F("WriteBuffer::writeCHAR('"));
	Serial.write(c);
	Serial.print(F("') "));
	internalsToSerial();
	Serial.print(F(" ..."));
	Serial.flush();
#endif
	if (p - str >= strSize)
		flush();
	*p = c;
	p++;
#ifdef DEBUG_WRITEBUFFER_WRITECHAR
	Serial.println(F(" DONE"));
	Serial.flush();
#endif
	return p - str;

}