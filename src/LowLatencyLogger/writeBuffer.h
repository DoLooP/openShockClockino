#pragma once
class WriteBuffer : public Print
{
	void internalsToSerial();
public:
	size_t totalWriten = 0;
	typedef void (*FlushFonctionPtr)();
	FlushFonctionPtr flushCallBack;
	uint8_t * const str;
	const size_t strSize;
	uint8_t *p;

	WriteBuffer(uint8_t *buffer, size_t len, FlushFonctionPtr fp);
	void flush();
	size_t reset();
	virtual size_t write(const uint8_t *buffer, size_t l);
	virtual size_t write(uint8_t c);
};