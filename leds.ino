//=====================================================================================
void tick()
{
  //toggle state
  int state = digitalRead(__DATALED);  // get the current state of pin
  digitalWrite(__DATALED, !state);     // set pin to the opposite state
}

//=====================================================================================
// Uses global variable uint8_t 'ledmode'

void StatusTick()   //Called every 200mS (0.2 seconds) from a timer
{
  const uint8_t status_tick_max = 17;
  static uint8_t status_tick_count = 0;

  digitalWrite(__DATALED,_DATALEDON);
  //boolean led_state;
  
  if ((status_tick_count == 0) && (ledmode > 0)){     // 1 flash
    //led_state = _STATUSLEDON;
    digitalWrite(__STATUSLED,_STATUSLEDON);
  }

  if ((status_tick_count == 2) && (ledmode > 1)){     // 2 flashes
    digitalWrite(__STATUSLED,_STATUSLEDON);
  }

  if ((status_tick_count == 4) && (ledmode > 2)){     // 3 flashes
    digitalWrite(__STATUSLED,_STATUSLEDON);
  }

  if ((status_tick_count == 6) && (ledmode > 3)){     // 4 flashes
    digitalWrite(__STATUSLED,_STATUSLEDON);
  }

  if ((status_tick_count == 8) && (ledmode > 4)){     // 5 flashes
    digitalWrite(__STATUSLED,_STATUSLEDON);
  }

  if ((status_tick_count == 10) && (ledmode > 5)){     // 6 flashes
    digitalWrite(__STATUSLED,_STATUSLEDON);
  }

  if (status_tick_count % 2){                         //Detect ODD number
    digitalWrite(__STATUSLED,_STATUSLEDOFF);
  }

  status_tick_count++;
  if (status_tick_count >= status_tick_max){
    status_tick_count = 0;
  }
  digitalWrite(__DATALED,_DATALEDOFF);
}



