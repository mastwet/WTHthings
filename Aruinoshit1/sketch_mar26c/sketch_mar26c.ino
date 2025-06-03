
const byte LATCH = 17; 
const byte DATA  = 16; 
const byte CLOCK = 18; 
const byte chips = 1; // number of 74HC165 (8bit) chips used

static unsigned long lastMillis;
static unsigned long frameCount;
static unsigned int framesPerSecond;
const int seconds = 1;

void setup() { 
  Serial.begin(115200); 
  pinMode(LATCH, OUTPUT); 
  pinMode(CLOCK, OUTPUT); 
  pinMode(DATA, INPUT); 
  digitalWrite(CLOCK, HIGH); 
  digitalWrite(LATCH, HIGH);
} 

void loop() { 
  unsigned long now = millis();
  frameCount ++; // Once around the moon ...
  
  byte Switch[chips];
  
  digitalWrite(CLOCK, HIGH); 
  digitalWrite(LATCH, LOW); 
  digitalWrite(LATCH, HIGH); 
  
  for (byte i=0; i<chips; i++) { 
    Switch[i] = shiftIn(DATA, CLOCK, MSBFIRST);
  }
  
  if (now - lastMillis >= seconds * 1000) {
    framesPerSecond = frameCount / seconds;
    
    for (byte i=0; i<8; i++) { 
      Serial.print((Switch[0] & (1<<i)) != 0 ? 0 : 1); 
    }
     
    Serial.print(" "); 
    
    for (byte i=0; i<8; i++) { 
      Serial.print((Switch[1] & (1<<i)) != 0 ? 0 : 1); 
    } 
    
    Serial.print(" "); 
    Serial.print(framesPerSecond);
    Serial.println(" fps"); 

    frameCount = 0;
    lastMillis = now;
  }
}