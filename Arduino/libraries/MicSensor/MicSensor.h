/**
 * a class to encapsulate an analog microphone, processing
 * instantaneous values to smoothe them and track the overall
 * amplitude envelope.
 */
class MicSensor {

    public:
	static const int FULLSCALE  = 1024;	// analog read full scale

	/**
	 * MicSensor object constructor
	 *
	 * @parm port to be used for microphone input
	 *
	 * initialize calibration and sample values
	 */
  	MicSensor( int port );

	/**
	 * take and process a sample
	 *
 	 * @return	raw instantaneous un-normalized value
	 */
  	int sample();

	/**
	 * get the current (instantaneous) envelope amplitude
	 *
	 * @return uncalibrated value (0-512)
	 */
	int rawAmplitude() {
		return( currentWaveHeight );
	}
	  
	/**
	 * get the recent (smoothed) envelope amplitude
	 *
	 * @return uncalibrated value (0-512)
	 */
	int smootheAmplitude() {
		return (averageWaveHeight );
	}

	/**
	 * get the recent (smoothed) ambient noise level
	 *
	 * @return uncalibrated value (0-512)
	 *
	 * Mic sensor levels are uncalibrated and affected by
	 * lots of things.  As a sanity check we try to compute
	 * the recent mean low envelope amplitude, and call this
	 * the "ambient noise level".  A client can get this value
	 * to decide how much significance can be attributed to
	 * any particular value.
	 */
	int ambient() {
		return( averageBackground );
	}
	  
	/*
	 * sampling period 
	 *
	 *  Most people probably don't want to track the
	 *  instantaneous amplitude envelope, but rather
	 *  just want to know how noisy it has been recently.
	 *	
	 *  The client can specify a sampling interval, over
	 *  which we accumulate min/max and after which we
	 *  reset to start new accumulations.
	 */

	/**
	 * set the sampling period
	 *
	 * @param ms	number of milliseconds per sample
	 */
	void setPeriod( int ms ) {
		sampleInterval = ms;
	}

	/**
	 * get the sampling period
	 *
	 * @return ms	number of milliseconds per sample
	 */
	int getPeriod() {
		return(sampleInterval);
	}

	/**
	 * lowest (smoothed) envelope amplitude during last interval
	 *
	 * @param reset	start a new sample period
	 *
	 * @return uncalibrated value (0-512)
	 *
	 * If the user does a read and reset, followed promptly
	 * by another read, we might return a bogus value.  To
	 * prevent this we save the previous values, and if another
	 * read is done before any new samples, we (again) return 
	 * those previous values.
	 */
	int minVolume( bool reset ) {
		int ret = (recentMinVolume == FULLSCALE) ?  prevMinVolume: recentMinVolume;
		if (reset) {
			prevMinVolume = ret;
			newInterval( 0 );
		}
		return( ret );
	}

	/**
	 * lowest (smoothed) envelope amplitude during last interval
	 *
	 * @param reset	start a new sample period
	 *
	 * @return uncalibrated value (0-512)
	 *
	 * If the user does a read and reset, followed promptly
	 * by another read, we might return a bogus value.  To
	 * prevent this we save the previous values, and if another
	 * read is done before any new samples, we (again) return 
	 * those previous values.
	 */
	int maxVolume( bool reset ) {
		int ret = (recentMaxVolume == 0) ?  prevMaxVolume: recentMaxVolume;
		if (reset) {
			prevMaxVolume = ret;
			newInterval( 0 );
		}
		return( ret );
	}

	/**
	 * number of waves per second for the last period
	 *
	 * @return	samples/second
	 */
	int hertz() {
		return( recentHertz );
	}

private:
	// configuration information
	int micPort;		// port for the microphone input
	int sampleInterval;	// ms per sample interval

	// raw information about the previous sample
	unsigned long recentTime;// start time for this interval
	int prevVal;		// previous sample from this mic
	bool rising;		// samples headed north
	bool louder;		// volume envelope getting louder

	// processed information for the clients
	int recentMinVolume;	// lowest volume in this interval
	int prevMinVolume;	// lowest volume in previous interval
	int recentMaxVolume;	// highest volume in this interval
	int prevMaxVolume;	// highest volume in previous interval
	int currentWaveHeight;	// amplitude at last reversal
	int averageWaveHeight;	// smoothed amplitude
	int averageBackground;	// current estimate of ambient
	int wavesCounted;	// waves counted in this interval
	int recentHertz;	// waves per second in previous interval
	  
	/*
	 * called at each reversal to note a new point on the envelope
	 */
	void markWaveHeight (int value);

	/*
	 * called on each sample to determine whether or not the samples 
	 * have switched from rising to falling or falling to rising.
	 */
	bool detectReversal(int cur, int prev, bool wasrising);

	/*
	 * called to reset per tatistics at the start of a new interval
	 *
	 * @param time of reset
	 */
	void newInterval( unsigned long now );
};
