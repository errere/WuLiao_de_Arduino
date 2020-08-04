uint8_t en = 0;
uint16_t dt = 500;
void isrexint1(){
  en = 1;
  dt = map(analogRead(A0),0,1023,500,10000);
}

void setup() {
  // put your setup code here, to run once:
attachInterrupt(0, isrexint1, RISING); 
pinMode(9,OUTPUT);
pinMode(2,2);
Serial.begin(115200);
}


void loop() {
  // put your main code here, to run repeatedly:
if(en == 1){
  Serial.print("RUN TIME = ");
   Serial.println(dt);
digitalWrite(9,1);
delay(dt);
digitalWrite(9,0);
en = 0;
}
}


