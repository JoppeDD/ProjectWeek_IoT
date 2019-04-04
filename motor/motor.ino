#include <Stepper.h>
#define STEPS 2038 // the number of steps in one revolution of your motor (28BYJ-48)
Stepper stepper(STEPS, 8, 10, 9, 11);
const int lockedPin = 7;
boolean locked = false;

void setup() {
  stepper.setSpeed(6);
  pinMode(lockedPin, INPUT);
  Serial.begin(115200);
}
void loop() {
   Serial.print("Pin value: ");
   Serial.println(digitalRead(lockedPin));
   if(!locked){
    if(digitalRead(lockedPin)==HIGH){
      stepper.step(-510);
      locked = true;
    }
   }
   if(locked){
    if(digitalRead(lockedPin)==LOW){
      stepper.step(510);
      locked=false;
    }
   }

   delay(1000);
}
