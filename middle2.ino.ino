/*
  按鈕切換 LED 狀態（共陽極）
  狀態 0：恆亮（初始）
  狀態 1：慢閃爍
  狀態 2：不快不慢閃爍
  狀態 3：快閃爍
  按第 4 下回到狀態 0（恆亮）
*/

const int buttonPin = 2;   // 按鈕模組訊號腳 → D2
const int ledPin    = 3;   // LED → D3

// 按鈕模組按下時的電位，多數為 HIGH
// 如果按下去沒反應或反過來，把這行改成 LOW
const int PRESSED = HIGH;

// 共陽極 LED：LOW = 亮，HIGH = 滅
const int LED_ON  = LOW;
const int LED_OFF = HIGH;

// 閃爍間隔（毫秒）
const unsigned long SLOW_INTERVAL   = 1000;  // 慢
const unsigned long MEDIUM_INTERVAL = 500;   // 不快不慢
const unsigned long FAST_INTERVAL   = 150;   // 快

int mode = 0;                         // 0=恆亮 1=慢 2=中 3=快
int lastButtonState = !PRESSED;       // 上一次的按鈕狀態（用來偵測「剛按下」）
unsigned long lastDebounceTime = 0;   // 防彈跳計時
const unsigned long DEBOUNCE_DELAY = 50;

unsigned long lastBlinkTime = 0;      // 上次閃爍切換時間
int ledState = LED_ON;                // 目前 LED 狀態

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ledPin, LED_ON);       // 初始：恆亮
}

void loop() {
  // ---------- 1. 讀按鈕，偵測「按下的瞬間」 ----------
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();      // 狀態有變，重新計時
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // 穩定超過 50ms，才當作真的變化
    static int stableState = !PRESSED;
    if (reading != stableState) {
      stableState = reading;
      if (stableState == PRESSED) {   // 只在「按下」時切換模式
        mode = (mode + 1) % 4;        // 0→1→2→3→0
        lastBlinkTime = millis();
        ledState = LED_ON;            // 切換後先從亮開始
        digitalWrite(ledPin, ledState);
      }
    }
  }
  lastButtonState = reading;

  // ---------- 2. 依模式控制 LED ----------
  unsigned long interval = 0;

  switch (mode) {
    case 0:                           // 恆亮
      digitalWrite(ledPin, LED_ON);
      return;
    case 1: interval = SLOW_INTERVAL;   break;
    case 2: interval = MEDIUM_INTERVAL; break;
    case 3: interval = FAST_INTERVAL;   break;
  }

  // 閃爍：用 millis() 計時，不用 delay()，這樣按鈕才不會漏讀
  if (millis() - lastBlinkTime >= interval) {
    lastBlinkTime = millis();
    ledState = (ledState == LED_ON) ? LED_OFF : LED_ON;
    digitalWrite(ledPin, ledState);
  }
}