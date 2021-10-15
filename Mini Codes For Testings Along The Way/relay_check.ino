#define BUTTON 2 // the number of the pushbutton pin
#define RELAY 3 // the number of the relay pin
 
// variables will change:
int buttonState = 0; // variable for reading the pushbutton status
 
void setup() {
 // initialize the LED pin as an output:
 pinMode(RELAY, OUTPUT);
 // initialize the pushbutton pin as an input:
 //pinMode(BUTTON, INPUT_PULLUP);
}
 
void loop() {
 // read the state of the pushbutton value:
 //buttonState = digitalRead(BUTTON);
 
 digitalWrite(RELAY, HIGH);
 delay(5000);
 digitalWrite(RELAY, LOW);
 
}
