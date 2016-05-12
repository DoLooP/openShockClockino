#define TCAADDR 0x70

class Multiplexer
{
	int addr;
public:
	Multiplexer(int addr);
	void switchI2C(int chan);
};

extern Multiplexer tca9548a;