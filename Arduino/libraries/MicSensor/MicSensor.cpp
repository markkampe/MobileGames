#include "arduino.h"
#include "MicSensor.h"

// CALIBRATION QUANTIES
//	tried dynamic calibration of neutral by tracking
//	and averaging minimum and maximum raw values, but
//	found that noise, sampling error, and hysteresis 
//	in the averaging made that value much worse than 
//	this simple hard-coded constant
static const int MICNEUTRAL = MicSensor::FULLSCALE/2;	// assumed neutral value
static const int AMBIENT    = MICNEUTRAL/6;		// initial assumed ambient noise level

/*
 * weighted average of two numbers
 *
 * We do signal smoothing by taking the wighted average
 * of the current value, and the accumulated long term
 * average.  
 *
 * Equal weights for past and future yields a very moderate
 * smoothing.  Giving the past a much higher weight provides
 * a more stable long-term average.
 */
static const int EVEN = 1;
static const int SLOW = 30;  // slow averaging for ambient noise level

/**
 * weighted (geometric) average of two values
 *
 * @param v1	value #1
 * @param w1	weight of first value
 * @param v2	value #2
 * @param w2	weight of second value
 */
static int geometric( int v1, int w1, int v2, int w2 ) {

	// these products may not fit in a short
	long x1 = (long) v1 * (long) w1;
	long x2 = (long) v2 * (long) w2;

	// round result by adding denominator/2 to numerator
	// otherwise, round-down pulls long term averages to zero
	long r = (x1 + x2 + (w1+w2+1)/2) / (long) (w1 + w2);
	return (int) r;
}

void MicSensor::newInterval( unsigned long now ) {

	// compute the recent waves per second
	if (now == 0) {
		now = millis();
	}
	if (recentTime != 0 && now > recentTime) {
		long wps = (long) wavesCounted;
		wps *= 1000;
		wps /= (now - recentTime);
		recentHertz = wps;
	}
	wavesCounted = 0;

	// reset min and max volumes
	if (recentMinVolume < FULLSCALE)
		prevMinVolume = recentMinVolume;
	recentMinVolume = FULLSCALE;
	if (recentMaxVolume > 0)
		prevMaxVolume = recentMaxVolume;
	recentMaxVolume = 0;

	// note the starting time for the next interval
	recentTime = now;
}

/*
 * initialize calibration and sample values
 */
MicSensor::MicSensor( int port ) {
	micPort = port;    // note our input port

	sampleInterval = 1000;	// one second samples

	// semi-arbitrary initial values
	prevVal = MICNEUTRAL;
	currentWaveHeight = AMBIENT;
	averageWaveHeight = AMBIENT;
	averageBackground = AMBIENT;
	recentMinVolume = AMBIENT;
	recentMaxVolume = AMBIENT;
	rising = false;	
	louder = false;
	wavesCounted = 0;
	recentTime = 0;

	// start a new sampling interval
	newInterval( 0 );
}
  
/**
 * note the amplitude of a newly completed wave
 *
 * @param value of the raw signal
 *
 * This is the only interesting routine in the class.
 * It is called at each reversal to note the amplitude
 * of the last wave and infer information about the 
 * amplitude envelope:
 *  (1) convert raw value to an amplitude vs neutral
 *  (2) long average minima to infer ambient noise level
 *  (3) compute a smoothed maximum amplitude value
 *  (4) maintain max/min amplitudes for this interval
 */
void MicSensor::markWaveHeight (int value) {
  
        // turn the absolute signal into a deviation from neutral
        int height = (value > MICNEUTRAL) ? value - MICNEUTRAL : MICNEUTRAL - value;
        
	// slow-average envelope minima and call that the ambient noise level
        if (!louder && height > currentWaveHeight) { 
                averageBackground = geometric( currentWaveHeight, EVEN, 
					averageBackground, SLOW );
        }

        // note the new volume, and do the short-term smoothing
        louder = height > currentWaveHeight;
	currentWaveHeight = height;
	averageWaveHeight = geometric(currentWaveHeight, EVEN, 
					averageWaveHeight, EVEN);
        
	// see if we have a new maximum/minimum interval value
	if (averageWaveHeight < recentMinVolume) {
		recentMinVolume = averageWaveHeight;
	} 
	if (averageWaveHeight > recentMaxVolume) {
		recentMaxVolume = averageWaveHeight;
	}
}

/**
 * called on each sample to identify sinusoid peaks
 *
 * @param current (raw) value
 * @param previous (raw) value
 * @param whether we were last rising or falling
 *
 * @return whether or not this represents a sinusoid reversal
 */
bool MicSensor::detectReversal(int cur, int prev, bool wasRising) {
	if (wasRising && prev > MICNEUTRAL && cur < prev) { // passed a maximum
		wavesCounted++;
		return true;
        } else if (prev < MICNEUTRAL && !rising && cur > prev) {  // reached a minimum
		return true;
        } 
        
        return false;
}
  
/*
 * called by the client to take and process a sample
 *
 * we compute smoothed volume levels and wavelengths, and
 * continuously re-calibrate the ambient level, but this
 * method also returns the raw sample to the caller 
 * (just in case he knows what to do with it)
 */
int MicSensor::sample() {
    	int val = analogRead( micPort );

	// see if we have just passed a sinusoid peak
	if (detectReversal(val, prevVal, rising)) {
		// see if we should reset the per-interval statistics
		unsigned long now = millis();
		if (now < recentTime || now > recentTime + sampleInterval) {
			newInterval( now );
		}

		// note the progress of the envelope
		markWaveHeight(prevVal);
        }

	rising = val > prevVal;
	prevVal = val;
        return( val );
}
