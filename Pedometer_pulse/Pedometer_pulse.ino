#include <SoftwareSerial.h>
SoftwareSerial BTserial(2, 3);

/*****************************************---Pins----************************************************************/
const int xpin = A5; //accelerometer 
const int ypin = A4; //accelerometer 
const int zpin = A3; //accelerometer 
int pulsePin = A2;	 //pulse sensor

/************************************----Variables for Pulse--------------****************************************/
int count;
unsigned long bpm;
unsigned long T[22];
int oldData, newData;

const int numReadings = 10;

int readings[numReadings]; // the readings from the analog input
int readIndex = 0; // the index of the current reading
int total = 0; // the running total
int average = 0; // the average
int border = 600;

/************************************----Variables for Pedometer--------------****************************************/

float threshhold = 20;

float xval[100] = {0};
float yval[100] = {0};
float zval[100] = {0};

float xavg, yavg, zavg;

int steps=0, flag = 0;

/*************************************----Setup----***********************************************************************/

void setup()
{
  
  BTserial.begin(9600);
  //Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  //calibrate();
}

/*****************************************----Main----**************************************************************/

void loop()
{  
 
  calcSteps();
  //delay(1000);

  if (BTserial.available()){
    
    char val = BTserial.read();
      if (val=='1')
      {
       calcPulse();
       val=0;
      }
    }

}

/***********************************-----calcPulse()-------**********************************************************/

void calcPulse()
 {
  count = 0;
  while (true) {

    calcSignal();
    newData = average;

    while (true) {

      if (newData > border) {

        oldData = newData;
        calcSignal();
        newData = average;

        if (newData < border && oldData > border) {

          count += 1;
          T[count - 1] = millis();

          if (count > 1) {
            bpm = 60000 * (count - 1) / (T[count - 1] - T[0]);
            
           BTserial.print("bpm= ");
           BTserial.println(bpm);

           // Serial.print("bpm= ");
            //Serial.println(bpm);
          }

        }

      } else break;

    }

    if (count == 21)
      break;

  }
  BTserial.println("------------------------------------------------------- ");
  BTserial.print("Yekun bpm= ");
  BTserial.println(bpm);
  BTserial.println("------------------------------------------------------- ");
/*
  Serial.println("------------------------------------------------------- ");
  Serial.print("Yekun bpm= ");
  Serial.println(bpm);
  Serial.println("------------------------------------------------------- ");
  */
 }

 
/***********************************-----calcSignal()-------**********************************************************/

void calcSignal() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(pulsePin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.println(average);
  delay(5); // delay in between reads for stability
}

/*****************************************---Steps---*********************************************************/

void calcSteps()
{
  int acc = 0;
  float totvect[100] = {0};
  float totave[100] = {0};

  float xaccl[100] = {0};
  float yaccl[100] = {0};
  float zaccl[100] = {0};



  for (int a = 0; a < 100; a++)
  {
    xaccl[a] = float(analogRead(xpin) - 345);
    delay(1);


    yaccl[a] = float(analogRead(ypin) - 346);
    delay(1);


    zaccl[a] = float(analogRead(zpin) - 416);
    delay(1);

    // inital acc readings compared to calibrated avg readings
    totvect[a] = sqrt(((xaccl[a] - xavg) * (xaccl[a] - xavg)) + ((yaccl[a] - yavg) * (yaccl[a] - yavg)) + ((zval[a] - zavg) * (zval[a] - zavg)));

    //cal of acceleration vector: http://physics.stackexchange.com/questions/41653/how-do-i-get-the-total-acceleration-from-3-axes
    totave[a] = (totvect[a] + totvect[a - 1]) / 2 ;

    delay(100);

    //cal steps
    if (totave[a] > threshhold && flag == 0)
    {
      steps = steps + 1;
      flag = 1;

    }
    else if (totave[a] > threshhold && flag == 1)
    {
      // nothing happens
    }
    if (totave[a] < threshhold   && flag == 1)
    {
      flag = 0;
    }

    if (steps < 1) {
      steps = 1;
      
    }
    BTserial.print("steps: ");
    BTserial.println(steps - 1);

    //Serial.print("steps: ");
    //Serial.println(steps - 1);
  }

  delay(1000);    
}

/*****************************************----Calibrate----****************************************************/

void calibrate()
{
  float sum = 0;
  float sum1 = 0;
  float sum2 = 0;

  for (int i = 0; i < 100; i++) {

    xval[i] = float(analogRead(xpin) - 345);
    sum = xval[i] + sum;

  }

  delay(100);
  xavg = sum / 100.0;

  Serial.println(xavg);

  for (int j = 0; j < 100; j++)
  {
    yval[j] = float(analogRead(ypin) - 346);

    sum1 = yval[j] + sum1;
  }
  yavg = sum1 / 100.0;

  Serial.println(yavg);
  delay(100);
  for (int q = 0; q < 100; q++)
  {
    zval[q] = float(analogRead(zpin) - 416);

    sum2 = zval[q] + sum2;
  }
  zavg = sum2 / 100.0;
  delay(100);
  Serial.println(zavg);

}


