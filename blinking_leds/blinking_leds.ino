void setup() {
  for (int i = 2; i <= 6; i++) {
    pinMode(i, OUTPUT);    
  }
}

void loop() {
  
  for (int i = 2; i <= 6; i++) {
    
    int current_led = i; 

    int next_led = i + 1;

    if (next_led == 7) {
      next_led = 2;
    }
    
    blink(current_led, next_led);
  } 
  
}

void blink(int led_one, int led_two) {
  digitalWrite(led_two, HIGH);
  delay(100);
  digitalWrite(led_one, LOW);
  delay(100);
}