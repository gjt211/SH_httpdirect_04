#ifdef _ENA_SWITCH

//For the switch input & debounce
unsigned long ltime = 0;           // the last time the output pin was sampled
int debounce_count = 10;  // number of millis/samples to consider before declaring a debounced input
int counter = 0;          // how many times we have seen new value
int reading;              // the current value read from the input pin
int current_state = LOW;  // the debounced input value
boolean send_switch = false;

// =============================================================
// Setup OS timer for reading the switch input
void user_init(void) {
  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, 20, true);
}
//--------------------------------------------------------------
// start of timerCallback
void timerCallback(void *pArg) {
  reading = (digitalRead(__SWIN));
  if(reading == current_state && counter > 0) { counter--; }
  if(reading != current_state) { counter++; }
    
  // If the Input has shown the same value for long enough let's switch it
  if(counter >= debounce_count)
  {
    counter = 0;
    current_state = reading;
    send_switch = true;
  }
  ltime = millis();
} // End of timerCallback


#endif

