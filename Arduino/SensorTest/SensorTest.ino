#include <LightSensor.h>
#include <VuMeter.h>
#include <MicSensor.h>

const int MICSENSOR = A0;    // sensor pin connected to microphone
const int LIGHTSENSOR = A1;  // sensor pin connected to photocell
const int INDSENSOR = A2;    // float for sound, ground for light
const int FIRST_VU = 4;
const int NUM_VU = 10;

const int report = 100;  // print out a value evern n ms
const int delayPeriod = 1;   // one sample per millisecond

MicSensor *mic;
VuMeter *meter;
LightSensor *light;

unsigned long lastSampleTime;
int lastSampleValue;
int numSamples;
bool vu_sound = true;

void setup()
{
  mic = new MicSensor( MICSENSOR );
  mic->setPeriod( report );
  
  light = new LightSensor( LIGHTSENSOR );
  
  meter = new VuMeter( FIRST_VU, NUM_VU );
  
  lastSampleTime = millis();
  
  // figure out to send to the VU meter
  vu_sound = analogRead( INDSENSOR ) > 512;
  
  Serial.begin(9600);
  Serial.print("\n");
}

const int NONE = 0;
const int RESET = 1;
const int INTERVAL = 2;
const int SMOOTHE = 3;
const int RAW = 4;

void micPrint(int mode) {
      
      // everything starts with a time
        if (mode != NONE) {
            Serial.print(millis());
            Serial.print(": ");
        }
        
        int vol = 0;
        
	switch (mode) {
        case NONE:
              vol = mic->maxVolume(false);
              break;
	case RESET:  // minimum and maximum volume range w/reset
               Serial.print(numSamples);
               Serial.print(",");
               Serial.print(mic->hertz());
               Serial.print("hz,");
               Serial.print(mic->minVolume(false));
               Serial.print("-");
               vol = mic->maxVolume(true);
               Serial.print(vol);
               Serial.print("/");
               Serial.print(mic->ambient());
               
		break;
	case INTERVAL:  // minimum and maximum volume w/o resets
               Serial.print(numSamples);
               Serial.print(",");
               Serial.print(mic->hertz());
               Serial.print("hz,");
               Serial.print(mic->minVolume(false));
               Serial.print("-");
               vol = mic->maxVolume(false);
               Serial.print(vol);
               Serial.print("/");
               Serial.print(mic->ambient());

                break;
        case SMOOTHE:  // recent smoothed values
                vol = mic->smootheAmplitude();
                Serial.print(vol);  
                Serial.print("/");
                Serial.print(mic->ambient());

                break;
         case RAW:  // raw values values
                vol = mic->rawAmplitude();
                Serial.print(vol);  
                Serial.print("/");
                Serial.print(mic->ambient());
                break;
	}
        // everything ends with a newline
        if (mode != NONE) {
          Serial.print("\n");
        }
}

// test a range of sample counts and delays
int lightTest(int max_samples, unsigned max_delay) {
  
  int max = 0;
  int min = 1024;
  
  for( int s = 1; s <= max_samples; s *=2 ) {
    for( int d = 1; d <= max_delay; d *=2 ) {
      int v = light->sample(s,d);
      if (v < min) min = v;
      if (v > max) max = v;
    }
  }
   
   Serial.print("light: ");
   Serial.print(min);
   Serial.print("-");
   Serial.print(max);
   Serial.print("\n");
   
   return( (min + max)/2 );
}

void loop()
{
	lastSampleValue = mic->sample();  // collect another sample
        numSamples++;
        unsigned long now = millis();
        if (now < lastSampleTime || now > lastSampleTime + report) {
          // display accumulated sound info
          micPrint(SMOOTHE);
          if (vu_sound) {
            meter->setLevel( mic->smootheAmplitude(), 512 );
          }
          
          // display current light level
          lastSampleValue = lightTest( 2, 1 );
          // lastSampleValue = light->sample();
          if (!vu_sound) {
            meter->setLevel( lastSampleValue, 1024 );
          }
          
          // reset the sample counters
          lastSampleTime = millis();
          numSamples = 0;
        }
        // with sound, higher resolution is better, delays seem unnecessary
	// delay(delayPeriod);
}

