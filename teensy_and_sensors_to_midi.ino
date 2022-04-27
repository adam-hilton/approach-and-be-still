#include <HCSR04.h>

//real time adjustments
int maxDist = 250; // max distance in cm for sensors
int minDist = 50; // max distance in cm for sensors
int distanceThresh = 230;
int closeDiff = 10;


int SLval; // initial read vals
int SRval;
int tapeCC;
int soundCC;
int engineCC;
int incrementor = 4;
int highIncrementor = 10;
int soundOff = 127;
int soundOn = 0;
int maxRead;
int minRead;

// stores overall values for each type of sensor
int Sval;

// stores MIDI values
int midiCCS;

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

UltraSonicDistanceSensor distanceSensor1(27, 28, maxDist);
UltraSonicDistanceSensor distanceSensor2(15, 14, maxDist);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  SLval = max(distanceSensor1.measureDistanceCm(), minDist);
  SRval = max(distanceSensor2.measureDistanceCm(), minDist);
  Sval = max(SLval,SRval); 

// smoothing for raw sensor cm values:
  total = total - readings[readIndex];
  maxRead = max(Sval,readings[readIndex]);
  minRead = min(Sval,readings[readIndex]);
  readings[readIndex] = Sval;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  average = total / numReadings;

  if (average == minDist) {
    average = maxDist;
  }


//  check if someone is close enough, increment the tape CC value

/*
 * would love to make the sound on/off less jumpy. Maybe another average?
 */

if (average <= distanceThresh){
  if ((maxRead - minRead) < closeDiff) {
  soundCC = soundOn;
  if (tapeCC > incrementor) {
    tapeCC = tapeCC - incrementor;
    }  
  } else if ((maxRead - minRead) > closeDiff) {
    soundCC = soundOff;
    if (tapeCC < 123) {
    tapeCC = tapeCC + incrementor;
    }  
  }
  
//  if (tapeCC < (127-incrementor)) {
//    tapeCC = tapeCC + incrementor;
//  }
  
}
else if (average > distanceThresh) {
  if (tapeCC > incrementor) {
  tapeCC = tapeCC - incrementor;
  }
  soundCC = soundOff;
}

engineCC = map(tapeCC, 0, 127, 127, 0);

Serial.print(average);
Serial.print(", ");
Serial.print(tapeCC);
Serial.print(", ");
Serial.print(engineCC);
Serial.print(", ");
Serial.println(soundCC);



usbMIDI.sendControlChange(4,tapeCC,1);
usbMIDI.sendControlChange(5,soundCC,2);
usbMIDI.sendControlChange(6,engineCC,3);

  //loop delay time
  delay(50);
}
