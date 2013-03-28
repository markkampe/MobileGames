/*
 * test program for Lillypad light sensor
 */

/*
 * the light sensor has a periodicity that is related to
 * the sampling rate.  At 10ms samples, we found the periodicity
 * to be around 4 samples.  The number 8 was chosen to sample
 * over two full waves.
 */
const int sampleWait = 20;
const int numSamples = 16; // this number must be a power of 2


const int sensor = A0; //determines which sensor is read
int samples[numSamples];

void sampleInit() {
    for( int i = 0; i < numSamples; i++ )
        samples[i] = 0;
}

int getSensorValue() {
    return analogRead ( sensor );
}

void printOver(int value, int over) { 
  Serial.print("Average of ");
  Serial.print(over);
  Serial.print(" = ");
  Serial.print(value);
  Serial.print("\n");
}

#ifdef OLD
void runAverages(){
    int avg = 0; 
    for( int i = 1; i <= numSamples; i *= 2 ) {
        for (int j = 0; j < i; j++ ) {
            avg += samples[j];
        }
        printOver(avg/i, i);
        avg = 0;
    }
}
#endif

int getAverage() {
  int avg = 0;
  for( int i = 0; i < numSamples; i++ ) {
    avg += samples[i];
  }
  return( avg/numSamples );
}

void setup() {
  // initialize the Serial port for diagnostics  
  Serial.begin(9600);
  // initialize the sample averager
  sampleInit();
  
#ifdef LATER
  Serial.print("# sample period ");
  Serial.print(sampleWait);
  Serial.print("ms, average over ");
  Serial.print(numSamples);
  Serial.print(" samples\n");
#endif
  Serial.print("sample,value\n");
}

void loop() {
  static int i = 0;
  int val = getSensorValue();
  samples[(i++)%numSamples] = val;
  
  if ( (i%numSamples) == 0 ) { 
    Serial.print(i++);
    Serial.print(",");
    Serial.print(getAverage());
    Serial.print("\n");
  }

  delay( sampleWait );
}

