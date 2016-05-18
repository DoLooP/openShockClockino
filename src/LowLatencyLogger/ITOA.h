class ITOA
{
	void itoa(long S, bool sign);
public:
	virtual size_t write(uint8_t c) = 0;	// we use this interface to send decimals to derived class
	void itoa(short S);
	void itoa(unsigned short S);
	void itoa(long S);
	void itoa(unsigned long S);
};