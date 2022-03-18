#define DURING_ON_MAX 15000
#define TIME_STAIR_BUSY 3500
#define GAP_ON_OFF 300
#define PIR_TOP 52
#define PIR_BOTTOM 53
#define NB_LED 14

int startFrom = 0;
int endTo = 0;
int useNow = 0;
int endUseFrom = 0;
int state = HIGH;
int lastSensorOn = 0;
const int leds[] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35};

void setup() {
  // on init les I/O
  for(int i=0; i<NB_LED ;i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH);
  }
  pinMode(PIR_TOP, INPUT);
  pinMode(PIR_BOTTOM, INPUT);

  // on attends que les capteurs soit libres
  while(digitalRead(PIR_TOP) != LOW || digitalRead(PIR_BOTTOM) != LOW) {
    delay(500);
  }
  digitalWrite(leds[NB_LED-1], LOW);
  digitalWrite(leds[0], LOW);
  delay(1500);
  digitalWrite(leds[NB_LED-1], HIGH);
  digitalWrite(leds[0], HIGH);
}

void changeLedState(int pState) {
  int gapReduce = (state == LOW && lastSensorOn == PIR_BOTTOM) ? GAP_ON_OFF/2 : GAP_ON_OFF;
  if(startFrom > endTo) {
    for(int i=startFrom; i>=endTo ;i--) {
      digitalWrite(leds[i], pState);
      delay(gapReduce);
    }
  } else {
    for(int i=startFrom; i<=endTo ;i++) {
      digitalWrite(leds[i], pState);
      delay(gapReduce);
    }
  }
}

void loop() {
  
  startFrom = -1;

  // on regarde d'ou vient notre guguss
  if(digitalRead(PIR_TOP) && lastSensorOn!=PIR_TOP) { // du haut vers le bas car il descend
    startFrom = 0;
    endTo = NB_LED-1;
    lastSensorOn = PIR_TOP;
  } else if(digitalRead(PIR_BOTTOM) && lastSensorOn!=PIR_BOTTOM) { // du bas vers le haut car il monte
    startFrom = NB_LED-1;
    endTo= 0;
    lastSensorOn = PIR_BOTTOM;
  }

  // si l'utilisateur est déjà dans l'escalier et qu'on a capter quelque chose c'est qu'il arrive au bout
  if((millis()-useNow)<DURING_ON_MAX && state==LOW && startFrom>=0) {
    int temp = startFrom; // on inverse le mouvement car c'est pas une arrivé sur l'escalier mais une sortie
    startFrom = endTo;
    endTo = temp;
  }
  
  if(startFrom>=0) {
    state = state ? LOW : HIGH;
    changeLedState(state); // on allume les leds
    if(!state) {
      useNow = millis();
    } else {
      useNow = 0;
      endUseFrom = millis();
    }
  } else {
    if(useNow>0 && (millis()-useNow)>DURING_ON_MAX && state==LOW) {
      state = HIGH;
      changeLedState(state); // on éteint
      useNow = 0;
      lastSensorOn = 0;
    } else {
      if(endUseFrom>0 && (millis()-endUseFrom)>TIME_STAIR_BUSY) {
        endUseFrom=0;
        lastSensorOn = 0;
      }
      delay(250);
    }
  }
  
}
