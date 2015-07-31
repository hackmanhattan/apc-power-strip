static const int startpin = 5;
static const int numpins = 8;

void setup() {
  for (int i=0; i < numpins; ++i) {
    pinMode(startpin+i, INPUT);
    digitalWrite(startpin+i, LOW);
  }
  Serial.begin(57600);
  //Serial.println("ready");
}

void loop() {
  static int mode = 0;
  
  if (mode == 0) {
    if (digitalRead(startpin) == LOW) {
      mode = 1;
      delay(2);
      for (int i=1; i < numpins; ++i) {
        if (digitalRead(i+startpin) == LOW) {
          //Serial.print("pin "); Serial.print(i); Serial.println(" down");
          Serial.print("t"); Serial.println(i-1);
        }
      }
    }
  } else {
    if (digitalRead(startpin) == HIGH) {
      delay(2);
      mode = 0;
    }
  }
}
