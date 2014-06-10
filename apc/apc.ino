const uint8_t clock = 2;
const uint8_t strobe = 3;
const uint8_t di = 4;

uint8_t switchstate = 0;
uint8_t newstate = 0;
uint8_t state = 0;
typedef enum _states {
  COMMAND=0, TARGETS
} States;
uint8_t command = 0;


void setup()
{
  pinMode(clock, OUTPUT);
  digitalWrite(clock, LOW);
  pinMode(strobe, OUTPUT);
  digitalWrite(strobe, LOW);
  pinMode(di, OUTPUT);
  digitalWrite(di, LOW);
  Serial.begin(9600);
  Serial.println("ready");
}

void loop()
{
  while (Serial.available()) {
    char b = Serial.read();
    switch (state) {
      case COMMAND:
        b = tolower(b);
        if (strchr("nft", b)) {
            command = b;
            newstate = switchstate;
            state = TARGETS;
        } else if (b == 'c') {
          Serial.println("all relays off");
          switchstate = 0;
          doShift();
        } else if (b == 'h') {
          help();
        } else if (b == 'q') {
          query();
          Serial.println(buf);
        }
        break;
      case TARGETS:
        if (b >= '0' && b <= '7') {
          uint8_t rbit = (1 << (b - '0'));
          switch(command) {
            case 'n':
              newstate |= rbit;
              break;
            case 'f':
              newstate &= ~rbit;
              break;
            case 't':
              newstate ^= rbit;
              break;
          }
        } else if (b == 27 || b == 'x') {
          Serial.println("Abort");
          state = COMMAND;
        } else {
          switchstate = newstate;
          query()
          doShift();
          state = COMMAND;
        }
        break;
    }
  }
}

void doShift()
{
  shiftOut(di,clock,LSBFIRST,switchstate);
  digitalWrite(strobe, HIGH);
  delay(1);
  digitalWrite(strobe, LOW);
}

void help()
{
  Serial.println("*** HELP ***");
  Serial.println("Commands are followed by one or more numbers indicating the relay to change");
  Serial.println("nxx... turn relays on");
  Serial.println("fxx... turn relays off");
  Serial.println("txx... toggle relays");
  Serial.println("c turn off all relays");
  Serial.println("xx above is one or more numbers between 0 and 7, corresponding to relays 1 through 8");
  Serial.println("Press the ESC or x key while entering numbers to cancel the current command");
}

void query()
{
    for (unsigned char b=128; b; b >>= 1) {
        Serial.print((switchstate & b) ? "1" : "0");
    }
    Serial.println();
}
