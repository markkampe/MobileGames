#include "Arduino.h"
#include "VuMeter.h"

/**
 * a class to drive a linear N-segment VU meter display
 *   driven by a range of consecutive Arduino output pins
 */
VuMeter::VuMeter( int firstReg, int numSegs ) {

	regs = firstReg;
	numRegs = numSegs;

	for( int i = 0; i < numRegs; i++ )
		pinMode( regs + i, OUTPUT );
}
	
/**
 * set the VU meter to a specified level
 */
void VuMeter::setLevel( int level, int fullScale ) {

	// figure out how many lights this corresponds to
	int buckets = numRegs+1;	// 0-n lights means n+1 buckets
	int perBucket = (fullScale + (buckets/2)) / buckets;
	int numLights = level/perBucket;

	// set the lights accordingly
	for( int i = 0; i < numRegs; i++ ) {
		digitalWrite( regs + i, (i >= numLights) ? LOW : HIGH );
	}
}
