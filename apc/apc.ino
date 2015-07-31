const uint8_t clock = 2;
const uint8_t strobe = 3;
const uint8_t di = 4;

uint8_t switchstate = 0;
uint8_t newstate = 0;
uint8_t state = 0;
typedef enum _states {
  COMMAND=0, TARGETS, LIMIT
} States;
uint8_t command = 0;

static const int NUM_OUTLETS = 8;
uint32_t timeLimits[NUM_OUTLETS] = {0};
uint32_t startTimes[NUM_OUTLETS] = {0};

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
  static uint8_t rbit = 0;
  static uint32_t limit = 0;
  static uint32_t buf = 0;

  checkLimits();
  
  while (Serial.available()) {
    char b = Serial.read();
    switch (state) {
      case COMMAND:
        b = tolower(b);
        if (strchr("nftl", b)) {
            command = b;
            newstate = switchstate;
            state = TARGETS;
            rbit = 0;
        } else if (b == 'h') {
          help();
        } else if (b == 'q') {
          query();
        }
        break;
      case TARGETS:
        if ((b >= '0' && b <= '7') || b == '*') {
          if (b == '*') {
            rbit = 255;
          } else {
            rbit = (1 << (b - '0'));
          }
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
        } else if (b == '=') {
          state = LIMIT;
          limit = 0;
          buf = 0;
        } else if (b == ' ') {
          break;
        } else if (b == 27 || b == 'x') {
          Serial.println("Abort");
          state = COMMAND;
        } else {
          uint8_t switchedOn = newstate & (newstate ^ switchstate);
          switchstate = newstate;
          query();
          doShift();
          state = COMMAND;
          for (unsigned char b=128, n=7; b; b >>= 1, --n) {
            if (switchedOn & b) {
              startTimes[n] = millis();
            }
          }
        }
        break;
      case LIMIT:
        if (b == ' ') break;
        else if (b == 'x' || b == 27) {
          Serial.println("Abort");
          state = COMMAND;
        } else if (b >= '0' && b <= '9') {
          buf = buf * 10 + (b - '0');
        } else if (b == 'm') {
          limit += buf * 60000;
          buf = 0;
        } else if (b == 'h') {
          limit += buf * 3600000;
          buf = 0;
        } else {
          limit += buf * 1000;
          buf = 0;
          if (b != 's') {
            setLimits(rbit, limit);
            query();
            state = COMMAND;
          }
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

void checkLimits()
{
  uint8_t rbit = 0;
  for (unsigned char b=128, n=7; b; b >>= 1, --n) {
    if ((switchstate & b)) {
      if (startTimes[n] + timeLimits[n] >= millis()) {
        rbit |= b;
        Serial.print((short)n); Serial.println(" turning off due to timeout");
      }
    }
  }
  if (rbit) {
    switchstate &= ~rbit;
    doShift();
  }
}

void setLimits(uint8_t bits, uint32_t limit)
{
  for (unsigned char b=128, n=7; b; b >>= 1, --n) {
    if (bits & b) {
      timeLimits[n] = limit;
    }
  }
}

void printTime(uint32_t ms)
{
  uint32_t h=0,m=0;
  uint32_t s = ms / 1000;
  if (s >= 3600) {
    h = s / 3600;
    s -= h * 3600;
  }
  if (s >= 60) {
    m = s / 60;
    s -= m * 60;
  }
  if (h) {
    Serial.print(h); Serial.print(" hours");
  }
  if (m) {
    if (h) Serial.print(", ");
    Serial.print(m); Serial.print(" minutes");
  }
  if (s || (!m && !h)) {
    if (m) Serial.print(", ");
    Serial.print(s); Serial.print(" seconds");
  }
  Serial.println();
}

void help()
{
  Serial.println("*** HELP ***");
  Serial.println("Commands are followed by one or more numbers indicating the relay to change");
  Serial.println("nxx... turn relays on");
  Serial.println("fxx... turn relays off");
  Serial.println("txx... toggle relays");
  Serial.println("lxx... = yy set limit time.  A limit of 0 means unlimited.  You can use h,m,s to express time, e.g. 1h30m");
  Serial.println("xx above is one or more numbers between 0 and 7, corresponding to relays 1 through 8, or * to indicate all");
  Serial.println("Press the ESC or x key while entering numbers to cancel the current command");
}

void query()
{
    for (unsigned char b=128, n=7; b; b >>= 1, --n) {
      if ((switchstate & b)) {
        Serial.print((short)n); Serial.print(" on");
        if (timeLimits[n]) {
          Serial.print(", remaining "); printTime(startTimes[n] + timeLimits[n] - millis());
        }
        Serial.println();
      } else if (timeLimits[n]) {
        Serial.print((short)n); Serial.print(" limit "); printTime(timeLimits[n]);
      }
    }
    Serial.println();
}
