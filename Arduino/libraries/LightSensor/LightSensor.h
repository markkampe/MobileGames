/**
 * class to encapsulate an ardunio light sensor
 */
class LightSensor {

    public:
	static const int FULLSCALE = 1024;	// maximum sensor value
	static const int SAMPLES = 2;		// default number of samples
	static const int DELAY = 1;		// default miliseconds per sample

	/**
	 * LightSensor constructor
	 *
	 * @param analogPort	for sensor input
	 */
	LightSensor( int analogPort );

	/**
	 * take a set of samples and average together the values
	 *
	 * @param num of samples to average
	 * @param delay between samples
	 *
	 * @return uncalibrated value 0-1024
	 */
	int sample( int num, unsigned delay );	
	
	/**
	 * take a sample with reasonable parameters
	 *
	 * @return uncalibrated value 0-1024
	 *
 	 * If samples are taken in too rappid succession they
	 * become irratic, but experience suggests that if they
	 * are more than a millisecond apart, there is no problem
	 */
	int sample() {
		return sample( SAMPLES, DELAY );
	}

    private:
	int port;
};
