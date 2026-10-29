const int buttonPin = 2;    // 按鈕接腳 (IN)
const int redPin = 9;       // 紅光 PWM 接腳 (R)
const int greenPin = 10;    // 綠光 PWM 接腳 (G)
const int bluePin = 11;     // 藍光 PWM 接腳 (B)

int moodScore = 5;          // 初始心情分數 (0 到 10)
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

unsigned long lastTouchTime = 0;        
unsigned long lastScoreDecreaseTime = 0; 

int brightness = 0;
int fadeAmount = 5;
unsigned long previousFadeMillis = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  lastTouchTime = millis();
  lastScoreDecreaseTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    static int actualButtonState = HIGH;
    if (reading != actualButtonState) {
      actualButtonState = reading;
      if (actualButtonState == LOW) {
        moodScore++;
        if (moodScore > 10) moodScore = 10;
        lastTouchTime = currentMillis; 
      }
    }
  }
  lastButtonState = reading;

  if (currentMillis - lastTouchTime > 5000) {
    if (currentMillis - lastScoreDecreaseTime >= 1000) { 
      lastScoreDecreaseTime = currentMillis;
      moodScore--;
      if (moodScore < 0) moodScore = 0; 
    }
  }

  long dynamicFadeInterval = map(moodScore, 0, 10, 60, 15); 

  if (currentMillis - previousFadeMillis >= dynamicFadeInterval) {
    previousFadeMillis = currentMillis;

    brightness = brightness + fadeAmount;
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount; 
    }

    // 共陽極邏輯反向運算 (0 最亮，255 熄滅)
    int r = 255, g = 255, b = 255;

    if (moodScore <= 3) {
      // 難過期：紅燈亮
      r = 255 - brightness;
      g = 255;
      b = 255;
    } else if (moodScore <= 7) {
      // 普通期：黃光
      r = 255 - brightness;
      g = 255 - brightness;
      b = 255;
    } else {
      // 開心狂歡期：藍綠光
      r = 255;
      g = 255 - brightness;
      b = 255 - brightness;
    }

    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);
  }
}