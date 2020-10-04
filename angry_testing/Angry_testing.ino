          enum ledStates {INCREASE, DECREASE, STAY, WAVE, OFF, ON, RELIEVEDINC, RELIEVEDDEC}; // Here we make nicknames for the different states our program supports.
enum ledStates ledState; // We define 'ledState' as type ledStates'
enum ledStates previousLedState = ledState;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long relievedMillis = 0;

int errorCounter = 0;

const int inputPin1(7);
int buttonState1 = 0;
bool flag1 = false;

const int inputPin2(3);
bool flag2 = false;

const int inputPin3 (2);
bool flag3 = false;


const int LDRpin1 = A4;
float LDRval1 = 0;
bool LDRflag1 = false;
bool LDRtrigger1 = false;

const int LDRpin2 = A5;
float LDRval2 = 0;
bool LDRflag2 = false;
bool LDRtrigger2 = false;

int brightness = 0; // our main variable for setting the brightness of the LED
float velocity = 1.0; // the speed at which we change the brightness.
int ledPin = 9; // we use pin 9 for PWM
int p = 0; // use to keep track how often we plot
int plotFrequency = 3; // how often we plot, every Nth time.


void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT); // set ledPin as an output.

  pinMode(inputPin1, INPUT);
  pinMode(inputPin2, INPUT);
  pinMode(inputPin3, INPUT);
  
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

  buttonState1 = digitalRead(7);
  
 
  LDRval1 = analogRead(LDRpin1);
  LDRval2 = analogRead(LDRpin2); 

  if(LDRval1 < 60){
    Serial.print("left!");
    LDRflag1 = true;
  } else {
    LDRflag1 = false;
  }
  if (LDRval2 < 60){
    Serial.print("right!");
    LDRflag2 = true;
  }else if (LDRflag2 = true && LDRval2 <= 60 ){
     LDRtrigger2 = true;
     LDRflag2 = false;
     Serial.print("triggered");
     
  } 
  
  
}

void compose() {
  // this is a state machine which allows us to decouple the various operations from timed loops. 
  // instead we just switch from state to state when particular conditions are met.
  // we switch states by calling the changeState() function.
  
  switch (ledState){


  
  case INCREASE:
  
  if(digitalRead(7) == HIGH && flag1 == false){
    errorCounter = errorCounter + 1;
    flag1 = true;
  }

  if(digitalRead(3) == HIGH && flag2 == false){
    errorCounter = errorCounter + 1;
    flag2 = true; 
  }
  if(digitalRead(2) == HIGH && flag3 == false){
    errorCounter = errorCounter + 1;
    flag3 = true; 
  }
  

  if(errorCounter > 0){
    changeState(ON);
  }
    brightness = 255;

    plot("INCREASING", brightness);
        
   
    break;
   
  case DECREASE:
    brightness = decrease_brightness(brightness, 4);
  
    plot("DECREASING", brightness);
      if (brightness == 0||currentMillis - startMillis >= random(1000)){
      changeState(INCREASE);
      }
     break;

  case WAVE:
    plot("WAVE", brightness);
    
    brightness = sinewave(1000,256,0); // you can tweak the parameters of the sinewave
    analogWrite(ledPin, brightness);
    
    if (currentMillis - startMillis >= 5000){ //change state after 5 secs by comparing the time elapsed since we last change state
      changeState(DECREASE);
      }
    break;
    
  case STAY:
    plot("STAY", brightness);
    brightness = brightness;
    break;

  case ON:
  if(digitalRead(7) == LOW && flag1 == true){
    errorCounter = errorCounter -1;
    flag1 = false;
    changeState(RELIEVEDINC);
  }
  if(digitalRead(3) == LOW && flag2 == true){
    errorCounter = errorCounter -1;
    flag2 = false;
    changeState(RELIEVEDINC);
  }
  if(digitalRead(2) == LOW && flag3 == true){
    errorCounter = errorCounter -1;
    flag3 = false;
    changeState(RELIEVEDINC);
  }
   

  
    if(digitalRead(7) == HIGH && flag1 == false){
    errorCounter = errorCounter + 1;
    flag1 = true;
  }

  if(digitalRead(3) == HIGH && flag2 == false){
    errorCounter = errorCounter + 1;
    flag2 = true; 
  }
  if(digitalRead(2) == HIGH && flag3 == false){
    errorCounter = errorCounter + 1;
    flag3 = true; 
  }
   

 if(errorCounter == 0){
  
  changeState(ON);
 }
    plot("ON", brightness);
    brightness = 255;
    if(errorCounter == 1){
      if (currentMillis - startMillis >= 1000 + random(-200, 200)){ 
      changeState(OFF);
      } 
    }
     if(errorCounter == 2){
      if (currentMillis - startMillis >= 500 + random(-100, 100)){ 
      changeState(OFF);
      } 
    }
     if(errorCounter == 3){
      if (currentMillis - startMillis >= 200 + random(-100,100)){ 
      changeState(OFF);
      } 
    }
    

    break;

  case OFF:
    plot("OFF", brightness);
    brightness = 50;
     if(errorCounter == 1){
      if (currentMillis - startMillis >= 1000 + random(-400, 400)){ 
      changeState(ON);
      } 
    }
     if(errorCounter == 2){
      if (currentMillis - startMillis >= 500 + random(-200, 200)){ 
      changeState(ON);
      } 
    }
     if(errorCounter == 3){
      if (currentMillis - startMillis >= 200 + random(-150,150)){ 
      changeState(ON);
      } 
    }
    
    break;


  case RELIEVEDINC:
    if (relievedMillis == 0){
      relievedMillis = millis();
    }
  
    brightness = increase_brightness(brightness, 4);

    plot("INCREASING", brightness);
        
     if (brightness >= 250||currentMillis - startMillis >= random(1000)){
      changeState(RELIEVEDDEC);
      }

      if(currentMillis - relievedMillis >= 2000){
        relievedMillis = 0;
        changeState(ON);
      }
  break;

  case RELIEVEDDEC:
    brightness = decrease_brightness(brightness, 4);
  
    plot("DECREASING", brightness);
      if (brightness == 0||currentMillis - startMillis >= random(1000)){
      changeState(RELIEVEDINC);
      }
  break;
  }
}


void changeState(ledStates newState){
    // call to change state, will keep track of time since last state
    startMillis = millis();
    ledState = newState;
  }
  
void plot(char *state, int brightness){
    // use this function to plot a graph.
    // it will normalize the auto-scaling plotter

    if ((p % plotFrequency) == 0){
      Serial.print(state);
      Serial.print(", ");
      Serial.print(brightness);
      Serial.println(", 0, 300");
      Serial.print(errorCounter);
      //Serial.print(LDRval1);
      //Serial.print(LDRval2);
    }
    p++;
  }

int increase_brightness (int brightness, float velocity){
    return brightness = brightness + 1 * velocity;
  }

int decrease_brightness (int brightness, float velocity){
    return brightness = brightness - 1 * velocity;
  }

int sinewave(float duration, float amplitude, int offset){
    // Generate a sine oscillation, return a number.
    // In case you are using this for analogWrite, make sure the amplitude does not exceed 256
    float period = millis()/duration; // Duration in ms determines the wavelength.
    float midpoint = amplitude / 2; // set the midpoint of the wave at half the amplitude so there are no negative numbers
    int value = midpoint + midpoint * sin ( period * 2.0 * PI );
    value = value + offset; //offset allows you to move the wave up and down on the Y-axis. Should not exceed the value of amplitude to prevent clipping.
    return value;
  }
