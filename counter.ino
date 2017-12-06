#ifdef _ENA_CNTR
//For the switch input & debounce
unsigned long ltime = 0;                // the last time the output pin was sampled
int debounce_count = 10;                // number of millis/samples to consider before declaring a debounced input
int counter = 0;                        // how many times we have seen new value
int reading;                            // the current value read from the input pin
int current_state = LOW;                // the debounced input value
unsigned long actual_count = 0;         // We store the actual count here
unsigned long mSinterval = 300000;      // 5 minutes

//===========================================================================
// Increment the count if the input has changed
// The count is reset when the value is published in function sendCNTR().
void readCNTR(){
  static uint8_t steps = 0;

  // If we have gone on to the next millisecond
  if(millis() != ltime)
  {
    reading = (digitalRead(__SWIN));
    if(reading == current_state && counter > 0)
    {
      counter--;
    }
    if(reading != current_state)
    {
       counter++; 
    }

    if(counter >= debounce_count){
      counter = 0;
      current_state = reading;
      steps++;
      if (steps >=2){
        steps = 0;
        actual_count++;
      }
    }
    ltime = millis();
  }
}
//===========================================================================
// Publish the counter count each time this function is called.
// It also resets the count to zero at that time.
void sendCNTR(){

    String cnts;
    cnts=String(actual_count);

    if (post_http_data("dat",cnts,"OUT","CN1")){
      actual_count = 0;       // Reset the count
    } else {
      // POST failed so dont reset the counter
    }
    
}
#endif
