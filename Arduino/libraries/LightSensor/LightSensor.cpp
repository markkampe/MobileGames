#include "arduino.h"
#include "LightSensor.h"

/*
 * constructor ... allocate the sample array
 */
LightSensor::LightSensor( int analogPort ) {
	
	port = analogPort;
}

/**
 * take a set of samples and average together the values
 *
 * @param num of samples to average
 * @param delay between samples
 */
int LightSensor::sample( int num, unsigned ms ) {

	long total = 0;

	// take specified number of samples at specified interval
	for( int i = 0; i < num; i++ ) {
		if (i != 0)
			delay( ms );
		total += analogRead( port );
	}
	total /= num;
	return ((int) total);
}
