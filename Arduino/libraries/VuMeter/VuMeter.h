/**
 * a class to drive a linear N-segment VU meter display
 */
class VuMeter {

    public:
	VuMeter( int firstReg, int numSegs );
	
	void setLevel( int level, int fullScale );

    private:
	int regs;
	int numRegs;
};
