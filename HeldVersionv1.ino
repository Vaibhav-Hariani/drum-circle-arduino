#include <MIDI.h>

const int trigPinA = 13; 
const int echoPinA = 12; 
const int trigPinB = 11; 
const int echoPinB = 10; 
const int trigPinC = 9; 
const int echoPinC = 8; 
const int trigPinD = 7; 
const int echoPinD = 6; 
const int trigPinE = 5; 
const int echoPinE = 4; 

const int trigPins[] = {trigPinA, trigPinB, trigPinC, trigPinD, trigPinE};
const int echoPins[] = {echoPinA, echoPinB, echoPinC, echoPinD, echoPinE};
const int channels[] = {0,     2,        4,        6,        8};
const int num_sensors = 5;  
const int numMeasurements = 5;

MIDI_CREATE_DEFAULT_INSTANCE();

bool noteOnA = false;
bool noteOnB = false;
bool noteOnC = false;
bool noteOnD = false;
bool noteOnE = false;

bool noteOn[] = {noteOnA, noteOnB, noteOnC, noteOnD, noteOnE};

void setup() {
  Serial.begin(31250);
  
  for(int i = 0; i < num_sensors; i++){
    pinMode(trigPins[i],OUTPUT);
    pinMode(echoPins[i],INPUT);
    }
  MIDI.begin(MIDI_CHANNEL_OFF); 
}

void loop() {  
  int distances[num_sensors] = {0};
  for (int cur_sample = 0; cur_sample < numMeasurements; cur_sample++) {
      for(int cur_sens= 0; cur_sens < num_sensors; cur_sens++){      
        distances[cur_sens] += getDistance(trigPins[cur_sens],echoPins[cur_sens]);
      }
      delay(10);       
    }
  for(int i = 0; i < num_sensors; i++){
    Sensorstuff(distances[i],channels[i],noteOn[i]);
    }
}

void Sensorstuff(int distance, int channel, bool &noteOn) {
    float distance_cm = (float)distance / (numMeasurements);  
  //Changed this to distance_cm for a little bit more efficiency.
  
  //Where does this .0343 come from?
  distance_cm = distance_cm * 0.0343 / 2;
  distance_cm = constrain(distance_cm, 0, 200);
  int midiValue = map(distance_cm, 24, 0, 0, 127);
  midiValue = constrain(midiValue, 0, 127);
  
  if (distance_cm < 24) { 
    if (!noteOn) {
      //Send Note On message if not on
      sendMidiData(channel, midiValue);
      noteOn = true;
    }
  } else if (noteOn) {
      //Send Note Off message if previously on
      MIDI.sendNoteOff(channel, midiValue, 1);
      MIDI.sendNoteOff(channel + 1, midiValue, 1);
      noteOn = false;
    }
  }


long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  //Why is this 10? Shouldn't it be 2 like above?
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH);
}

void sendMidiData(int channel, int midiValue) {
  //Have questions about this: In the docs for this (https://github.com/FortySevenEffects/arduino_midi_library)
  //They state that arg 0 is the midi note, arg1 is the velocity, and then the final arg is the channel?\

  //Perhaps we should rename/restructure this function to make it more readable?
  //Also, not sure if we need that second line, so we should test that
  
  MIDI.sendNoteOn(channel, midiValue, 1);
  delay(10);
  MIDI.sendNoteOn(channel + 1, midiValue, 1);
}
