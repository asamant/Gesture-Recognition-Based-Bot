int incomingByte = 0; // for incoming serial data

void setup() {
  
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);

}

void loop() {
  // if data on Serial line is available
    int delayTime = 1250;
    if (Serial.available() > 0)
    {
    // read the incoming byte
    analogWrite(9, 100);
    incomingByte = Serial.read();
    Serial.print("Incoming byte ");
    Serial.println(incomingByte);
    switch(incomingByte)
    {
      case 0:
        //Serial.write('r');
      break;
        
      case 1:
        turnRight(delayTime);
        //Serial.write('r');
      break;
      
      case 2:
        turnLeft(delayTime);
        //Serial.write('r');
      break;
      
      case 3:
        moveStraight(delayTime);
        //Serial.write('r');
      break;
      
      case 4:
        moveBack(delayTime);
        //Serial.write('r');
      break;
      
      case 5:
        stop(delayTime);
        //Serial.write('r');
      break;
      
      default:
        Serial.println("!!! Incorrect Incoming Byte !!!");
        stop(delayTime);
      break;
    }
    //Serial.write('r');
  }
  else
  {
    //Serial.write('r');
    Serial.println("!!! No Serial Data Available !!!");
    stop(delayTime);
  }
}

void moveStraight(int s) {
	digitalWrite(4, HIGH);
	digitalWrite(5, LOW);
	digitalWrite(6, HIGH);
	digitalWrite(7, LOW);
	delay(s);

}

void moveBack(int s) {
	digitalWrite(4, LOW);
	digitalWrite(5, HIGH);
	digitalWrite(6, LOW);
	digitalWrite(7, HIGH);
	delay(s);

}

void turnLeft(int s) {
	//analogWrite(9, 65);
	digitalWrite(4, HIGH);
	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(7, HIGH);
	delay(s);
	//analogWrite(9, 50);

}

void turnRight(int s) {
	digitalWrite(4, LOW);
	digitalWrite(5, HIGH);
	digitalWrite(6, HIGH);
	digitalWrite(7, LOW);
	delay(s);

}

void stop(int s) {
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(7, LOW);
	delay(s);

}
