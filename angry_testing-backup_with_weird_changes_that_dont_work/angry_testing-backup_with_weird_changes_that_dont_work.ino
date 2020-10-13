enum ledStates {INCREASE, DECREASE, STAY, WAVE, OFF, ON, RELIEVEDINC, RELIEVEDDEC}; // Here we make nicknames for the different states our program supports.
enum ledStates ledState; // We define 'ledState' as type ledStates'
enum ledStates previousLedState = ledState;


int errorCounter = 0;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long relievedMillis = 0;
unsigned long angryMillis = 0;

int brightness = 0; // our main variable for setting the brightness of the LED
float velocity = 1.0; // the speed at which we change the brightness.
int ledPin = 9; // we use pin 9 for PWM
int p = 0; // use to keep track how often we plot
int plotFrequency = 3; // how often we plot, every Nth time.

int hasBeenTrgrd = 0;

const int LDRpin1 = A4;
float LDRval1 = 0;

const int LDRpin2 = A5;
float LDRval2 = 0;

int errorThreshold = 450;
void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT); // set ledPin as an output.
  pinMode(LDRpin1, INPUT);
  pinMode(LDRpin2, INPUT);
 

  Serial.begin(9600); // initiate the Serial monitor so we can use the Serial Plotter to graph our patterns
}

void loop() {
  // put your main code here, to run repeatedly:
  compose();
  delay(10);

  analogWrite(ledPin, brightness);
  currentMillis = millis(); //store the current time since the program started
}

void compose() {
  // this is a state machine which allows us to decouple the various operations from timed loops.
  // instead we just switch from state to state when particular conditions are met.
  // we switch states by calling the changeState() function.



  switch (ledState) {





    case INCREASE:

      if (brightness < 255){
        brightness = increase_brightness(brightness, 4);
      }else {
        brightness = 255;
      }
      
      

      plot("INCREASING", brightness);

      LDRcalc();
      
       
      if ((LDRval1 + LDRval2)/2 < errorThreshold) {
        hasBeenTrgrd = 1;
        changeState(ON);
      }
      break;

    case DECREASE:
      brightness = decrease_brightness(brightness, 4);

      plot("DECREASING", brightness);
      if (brightness == 0 || currentMillis - startMillis >= random(1000)) {
        changeState(INCREASE);
      }
      break;

    case WAVE:
      plot("WAVE", brightness);

      brightness = sinewave(1000, 256, 0); // you can tweak the parameters of the sinewave
      analogWrite(ledPin, brightness);

      if (currentMillis - startMillis >= 5000) { //change state after 5 secs by comparing the time elapsed since we last change state
        changeState(DECREASE);
      }
      break;

    case STAY:
      plot("STAY", brightness);
      brightness = brightness;
      break;

    case ON:
      //Serial.print(errorCounter);

      plot("ON", brightness);
      brightness = 255;
       LDRcalc();
      if (currentMillis - startMillis >= ((LDRval1 )+(LDRval2 ))/2) {
        changeState(OFF);
      }
      if ((LDRval1 + LDRval2)/2 > errorThreshold && hasBeenTrgrd == 1) {
        changeState(RELIEVEDINC);
        hasBeenTrgrd = 0;
      }
      break;

    case OFF:
      plot("OFF", brightness);
      brightness = 50;
      LDRcalc();
            
      if (currentMillis - startMillis >= ((LDRval1 )+(LDRval2 ))/2) {
        changeState(ON);
      }
      break;
      
    case RELIEVEDINC:
    plot("RELIEVEDINC", brightness);

    LDRcalc();

     
      if (relievedMillis == 0) {
        relievedMillis = millis();
      }
        brightness = sinewave(1000, 255, 0);  

      if (currentMillis - relievedMillis >= 2000) {
        relievedMillis = 0;
        changeState(RELIEVEDDEC);
      }
      break;

    case RELIEVEDDEC:
      plot("RELIEVEDDEC", brightness);
      LDRcalc();

     if(angryMillis == 0){
      angryMillis = millis();
     }

      brightness = sinewave(1000, 255, 0);
      
     if(currentMillis - startMillis >= currentMillis - angryMillis){
      angryMillis = 0;
      changeState(INCREASE);
     }
      break;
  }
}

void changeState(ledStates newState) {
  // call to change state, will keep track of time since last state
  startMillis = millis();
  ledState = newState;
}

void LDRcalc(){
   LDRval1 = analogRead(LDRpin1);
   LDRval2 = analogRead(LDRpin2);
   if(hasBeenTrgrd = 1){
    errorThreshold = 520;
   } else {
    errorThreshold = 450;
   }
}

void plot(char *state, int brightness) {
  // use this function to plot a graph.
  // it will normalize the auto-scaling plotter

  if ((p % plotFrequency) == 0) {
    Serial.print(state);
    Serial.print(", ");
    Serial.print(brightness);
    Serial.println(", 0, 300");
    Serial.print(LDRval1);
    Serial.print(LDRval2);
  //  Serial.print(hasBeenTrgrd);
    Serial.print((LDRval1+LDRval2)/2);
    Serial.print(errorThreshold);
      
  }
  p++;
}

int increase_brightness (int brightness, float velocity) {
  return brightness = brightness + 1 * velocity;
}

int decrease_brightness (int brightness, float velocity) {
  return brightness = brightness - 1 * velocity;
}

int sinewave(float duration, float amplitude, int offset) {
  // Generate a sine oscillation, return a number.
  // In case you are using this for analogWrite, make sure the amplitude does not exceed 256
  float period = millis() / duration; // Duration in ms determines the wavelength.
  float midpoint = amplitude / 2; // set the midpoint of the wave at half the amplitude so there are no negative numbers
  int value = midpoint + midpoint * sin ( period * 2.0 * PI );
  value = value + offset; //offset allows you to move the wave up and down on the Y-axis. Should not exceed the value of amplitude to prevent clipping.
  return value;
}
