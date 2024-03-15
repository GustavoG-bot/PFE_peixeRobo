float cm=0.00;
int SonarPin=A0;
int sensorValue;


void setup(){
pinMode(SonarPin,INPUT);
Serial.begin(9600);
}


void loop(){
sensorValue=analogRead(SonarPin);
delay(50);
//Serial.println(sensorValue);
Serial.print(cm);
Serial.println("cm");
delay(1000);
}
