// A simple demo showing how to synchronize movement of steppermotors-

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <RBD_Timer.h>
#include <RBD_Button.h>
#include <elapsedMillis.h>



enum Stage
{
  Init,
  WaitToStart,
  Play,
  End,
  Restart
};


int buttonPin = 11;
int buttonLedPin = 12;

RBD::Button button(buttonPin); // input_pullup by default

AccelStepper stepper1(AccelStepper::DRIVER, 8, 9);
AccelStepper stepper2(AccelStepper::DRIVER, 6, 7);
AccelStepper stepper3(AccelStepper::DRIVER, 4, 5);

// Up to 10 steppers can be handled as a group by MultiStepper
MultiStepper steppers;

Stage currentStage = Init;

elapsedMillis elapsedTime; 

void setup() {
  Serial.begin(115200);
  Serial.println("Roilette started.");
  randomSeed(analogRead(0));
  Serial.println("Random seeded.");


  stepper1.setMaxSpeed(7000);
  stepper2.setMaxSpeed(7000);
  stepper3.setMaxSpeed(7000);
  stepper1.setAcceleration((rand() % 200) + 1);
  stepper2.setAcceleration((rand() % 200) + 1);
  stepper3.setAcceleration((rand() % 200) + 1);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
  steppers.addStepper(stepper3);

  Serial.println("Steppers initialized.");

  pinMode(buttonLedPin, OUTPUT);   

  Serial.println("Setup complete.");

}

void setStepperPosition(long *positions, int stepperNo, long newPosition)
{
  *(positions + stepperNo - 1) = newPosition;
  Serial.print("New position for stepper ");
  Serial.println(stepperNo);
  Serial.print(" set to: ");
  Serial.println(newPosition);
}

void turnOnButtonLed(bool on)
{
  digitalWrite(buttonLedPin, on ? HIGH : LOW);
}

void loop() {
  long positions[3]; // Array of desired stepper positions

  switch(currentStage)
  {
    case Init:
      Serial.println("Stage: Init");
      currentStage = WaitToStart;
      turnOnButtonLed(true);
      button.onReleased(); // dummy read
      break;
    case WaitToStart:
      if(button.onReleased()) {
        Serial.println("Button Released in WaitToStart");
        currentStage = Play;
/*        elapsedTime = 0;
        timeToPlay = random(10,60) * 1000;
        Serial.print("timeToPlay: ");
        Serial.println(timeToPlay);
*/
        positions[0] = random(100,600) * 100;
        positions[1] = random(100,600) * 100;
        positions[2] = random(100,600) * 100;
        steppers.moveTo(positions);
      
        turnOnButtonLed(false);
      }
      break;
    case Play:
      steppers.runSpeedToPosition(); // Blocks until all are in position
      currentStage = End;
      elapsedTime = 0;
      break;
    case End:
      if(elapsedTime > 2 * 1000)
      {
        Serial.println("Elapsed time in End");
        currentStage = Restart;
        turnOnButtonLed(true);
        currentStage = Init;
      }
      break;
    case Restart:
      if(button.onReleased()) {
        Serial.println("Button release in Restart");
        currentStage = Init;
      }
      break;
  };
}

