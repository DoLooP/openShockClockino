#define TCAADDR 0x70
#define TCA_INVALID (~0)

class Multiplexer
{
	int addr;
	int lastChannel = TCA_INVALID;
public:
	Multiplexer(int addr);
	void switchI2C(int chan);
};

extern Multiplexer tca9548a;