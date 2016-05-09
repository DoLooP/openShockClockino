#include "Arduino.h"
#include <SdFat.h>
#include "writeBuffer.h"
#include "assert.h"

//#define DEBUG_WRITEBUFFER
#ifdef DEBUG_WRITEBUFFER
#define DEBUG_WRITEBUFFER_FLUSH
#define DEBUG_WRITEBUFFER_WRITE
#define DEBUG_WRITEBUFFER_RESET
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
	Serial.print("WriteBuffer::flush():");
	internalsToSerial();
	Serial.println("[");
	Serial.write(str, p - str);
	Serial.println();
	Serial.println("]");
	delay(250);
	Serial.flush();
#endif
	totalWriten += p - str;
	flushCallBack();
	reset();
#ifdef DEBUG_WRITEBUFFER_FLUSH
	Serial.println("WriteBuffer::flush() DONE"); Serial.flush();
#endif
	return;
}

size_t WriteBuffer::reset()
{
#ifdef DEBUG_WRITEBUFFER_RESET
	Serial.print("WriteBuffer::reset() ");
#endif
	size_t s = p - str;
	p = str;
#ifdef DEBUG_WRITEBUFFER_RESET
	internalsToSerial();
	Serial.println(" DONE");
#endif 
	return s;
}

size_t WriteBuffer::write(const uint8_t *buffer, size_t l)
{
#ifdef DEBUG_WRITEBUFFER_WRITE
	Serial.print("WriteBuffer::write(");
	Serial.print((unsigned long)buffer, HEX);
	Serial.print(",");
	Serial.print(l);
	Serial.print(") ");
	internalsToSerial();
	Serial.println("...");
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
#ifdef DEBUG_WRITEBUFFER_WRITE
	Serial.println("WriteBuffer::write(...) DONE");
#endif
	return p - str;
}

size_t WriteBuffer::write(uint8_t c)
{
#ifdef DEBUG_WRITEBUFFER_WRITE
	Serial.print("WriteBuffer::write('");
	Serial.print(c);
	Serial.print(") ");
	internalsToSerial();
	Serial.println(" ...");
#endif
	if (p - str >= strSize)
		flush();
	*p = c;
	p++;
	return p - str;
#ifdef DEBUG_WRITEBUFFER_WRITE
	Serial.println("WriteBuffer::write(...) DONE");
#endif
}