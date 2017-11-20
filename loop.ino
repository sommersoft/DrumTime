void loop() {
  int loopYes = 1;
  if (! loopYes) {
    Serial.print(F("Loop Start: "));
    Serial.println(millis());
  }
  currMillis = millis();

  /* io.run() was causing the loop to take 100+ms per cycle, since its timeout defaults to 100ms.
   * this loop needs a higher cycle rate (lower cycle time) to reach an acceptable fidelity of
   * sensor readings. io.run() does allow the timeout to be set during the function call, which
   * can really reduce the cycle time. however, io.run() calls for subscription updates every
   * time it is run. in an effort to both reduce cycle time by using a shorter timeout and practice
   * good AIO etiquitte, I decided to reduce the amount of times io.run() is called while using the
   * shorter timeout. A 15ms timeout works well enough for my datapoints, but obviously won't work for
   * every situation (pings worked at 5ms, but feeds got chopped sometimes). 
   */
  if (currMillis - iorunMillis >= 5000) {
    //if (io.getMode() != WIFI_OFF) {io.run(10);}
    io.run(15);
    iorunMillis = currMillis;
  }
  
  if (io.status() == AIO_CONNECTED) {
    if (currMillis - activityMillis < 300000) { pixSingle_blu(0); }
    //Serial.print(F("io.status: ")); Serial.println(io.status());
  } else { //if we aren't connected, no use in continuing
    pixOff();
    pixSingle_red(0);
    return;
  }
  pushTelem();
  if (currMillis - preMillis >= 100) {
      /*
       * Read LIS3DH #1 for expenditures. LIS3DH is attached to the drum set, and click is used to
       * simplify the code (vs using accel data and doing math). Also checks if the LIS3DH is
       * working; if not, it tries to reconnect...if reconnect fails telemtry status is updated.
      */
      if (lis_exp.isAlive()) {
        uint8_t drum = lis_exp.getClick();
        if ((drum & 0x30)) {
          Serial.println(F("Clickin Beat Y0!!"));
          expend_new++;
          activityMillis = currMillis;
          TELEM_STATUS = TELEM_NORM_ACTIVE;
        }
      } else {
        //Serial.println(F("LIS DISCONNECTED"));
        if (! lis_exp.begin(0x18)) {
          TELEM_STATUS = TELEM_ERROR_LIS;
        } else {
          lis_exp.setRange(LIS3DH_RANGE_2_G);
          lis_exp.setClick(1, LIS_EXP_THRESH);
        }
      }

      /*
       * Read LIS3DH #2 for deposits. LIS3DH #2 is attached to band equipment, and click is used to
       * simplify the code (vs using accel data and doing math). Also checks if LIS3DH #2 is
       * working; if not, it tries to reconnect. We don't need constant connection, so if it isn't
       * connected, we don't show a telemetry error. This LIS3DH uses the secondary address (0x19).
      */
      if (lis_dep.isAlive()) {
        uint8_t band = lis_dep.getClick();
        if ((band & 0x30)) {
          Serial.println(F("You've earned a click!"));
          deposit_new++;
          activityMillis = currMillis;
          TELEM_STATUS = TELEM_NORM_ACTIVE;
        }
      } else {
        //Serial.println(F("LIS2 DISCONNECTED"));
        if (lis_dep.begin(0x19)) {
          lis_dep.setRange(LIS3DH_RANGE_8_G); // even out the range between snare pad & bells (needs cal)
          lis_dep.setDataRate(LIS3DH_DATARATE_100_HZ); // slow the data rate down due to lenght of cable
          lis_dep.setClick(1, LIS_DEP_THRESH);
        }
      }
      preMillis = currMillis;
    }

    /*
     * minute-to-win-it time! at a minute of read cycles, its easier to clean up the dep/exp values.
     * after using actual sensor data by finger drumming, i realized that calculating based on maximum
     * possible points was never going to calculate properly. playing percussion instruments will max-out
     * at like 160ish beats per minute, and on average will be close to 110bpm. band instruments could be
     * as low as 60bpm when practicing...
    */
    if (currMillis - minuteMillis >= 60000) {
      Serial.print(F("Dep: ")); Serial.print(deposits_new); Serial.print("\t Exp: "); Serial.println(expend_new);
      //if (deposits_new >=60) { deposits_mid++;}
      //if (expend_new >= 60) {expend_mid++;}
      if (deposits_new > 0) { deposits_mid = deposits_new; } //for testing
      if (expend_new > 0) { expend_mid = expend_new; } //for testing
      Serial.print(F("Dep mid: ")); Serial.print(deposits_mid);
      Serial.print(F("\t Exp mid: ")); Serial.println(expend_mid);
      
      deposits_new = 0;
      expend_new = 0;
      minuteMillis = currMillis;
    }

    /*
     * check for activity. if no activity for >5 minutes, turn off the NeoPixels, check to see if we need
     * to publish an update, and return to the start
    */
    if (currMillis - activityMillis >= 300000) {
      //Serial.println(F("No Activity. Turning off pixels.")); //it works!
      pixOff();
      if (deposits_mid > 0 || expend_mid > 0) { pubAIO(); }
      if (TELEM_STATUS != TELEM_ERROR_LIS || TELEM_STATUS != TELEM_ERROR_UNK) {
        TELEM_STATUS = TELEM_NORM_IDLE;
        TELEM_SEND_ERROR = false;
      } else {
        TELEM_SEND_ERROR = true;
      }
      return;
    }
    
    /* 
     *  update NeoPixels with the current balance
     */
    if (currMillis - blinkMillis >= 1000) {
      if (balance_start > 30) { //Full Green
        pixBalance(1);
      } else if (balance_start <=30 && balance_start >= 15) { //Half Green
          pixBalance(2);
      } else if (balance_start < 15 && balance_start > 5) { //Half Yellow
        pixBalance(3);
      } else if (balance_start <= 5 && balance_start >= 1) { //Flashing Half Yellow
          if (! blinkOn) {
            pixBalance(3);
            blinkOn = true;
          } else {
            pixOff();
            blinkOn = false;
          }
      } else { //Flashing Red
          if (! blinkOn) {
            pixBalance(4);
            blinkOn = true;
          } else {
            pixOff();
            blinkOn = false;
          }
      }
      blinkMillis = currMillis;
    }
    /*
     * update AIO every five minutes if necessary
    */
    if (currMillis - pubMillis >= 300000) {
      if (deposits_mid > 0 || expend_mid > 0) { 
        pubAIO(); 
      } else {
        pubMillis = currMillis;
      }
    }
  } else {
    //Serial.println(freetime_start);
    TELEM_STATUS = TELEM_NORM_FREETIME;
    pixSingle_off(free_pix);
    if (free_pix != 6) {
      free_pix++;
    } else {
      free_pix = 1;
    }
    pixSingle_grn(free_pix);
    delay(150);
  }
  if (!loopYes) {
    Serial.print(F("Loop End: "));
    Serial.println(millis());
  }
}

void pushTelem() {
  /*
   * send "telemetry" updates to AIO once an hour, or every 30 minutes if there is an error.
  */
  if (currMillis - telemMillis >= 3600000) {
    String telem = "DrumTime Status (1h): "; 
    telem += TELEM_STATUS;
    DrumTime->set("telemetry", telem);
    DrumTime->save();
    telemMillis = millis();
  } else if (currMillis - telemMillis >= 1800000 && TELEM_SEND_ERROR == true) {
    String telem = "DrumTime Status (30m): "; 
    telem += TELEM_STATUS;
    DrumTime->set("telemetry", telem);
    DrumTime->save(); 
  }
}

void pubAIO() {
  //Do final accounting, publish, and reset accounting vars
  deposits_final += deposits_mid;
  expend_final += expend_mid;
  Serial.print(F("Deposits final: "));
  Serial.println(deposits_final);
  Serial.print(F("Expenditures final: "));
  Serial.println(expend_final);
  
  balance_new = (balance_start + deposits_final) - expend_final;
  
  
  if (deposits_final > 0) {DrumTime->set("deposits", deposits_final);}
  if (expend_final > 0) {DrumTime->set("expenditures", expend_final);}
  if (balance_new != balance_start) {DrumTime->set("balance", balance_new);}
  DrumTime->save();
  
  Serial.print(F("New Balance: "));
  Serial.println(balance_new);
  
  deposits_new = 0;
  deposits_mid = 0;
  deposits_final = 0;
  expend_new = 0;
  expend_mid = 0;
  expend_final = 0;
  balance_start = balance_new;
  pubMillis = millis();
  Serial.println(F("AIO Published!"));
}

/*
 * When FreeTime is turned ON, on-board accounting is turned off
 * to allow for...well, free time of course.
 */
void freetime(AdafruitIO_Data *data) {
  Serial.print("received drumtime.freetime: ");
  freetime_start = data->value();
  Serial.println(freetime_start);
}

/*
 * We'll listen for balance updates in case the balance gets updated
 * off-board (e.g. on AIO dashboard). Think along the lines of
 * "DrumTime as a project is broken, let me help."
 */
void balance_sub(AdafruitIO_Data *data) {
  Serial.print("received drumtime.balance: ");
  balance_start = atol(data->value());
  Serial.println(balance_start);
}

/*
 * We'll listen for deposit updates in case deposits are made
 * off-board (e.g. on AIO dashboard). Think along the lines of
 * "Good Job, here's a reward".
 * This probably won't happen much, since it's easier to just update
 * the balance from the AIO dashboard (less calculation).
 * 
 * UPDATE: circular reference is circular. everytime the HUZZAH
 * sends a deposit, MQTT triggers an update and it gets added
 * to deposits_new. Its like BOGO, only reverse. Pondering must
 * happen. (statefull or value matching are initial thoughts)
 */
void deposits_sub(AdafruitIO_Data *data) {
  Serial.print("received drumtime.deposits: ");
  deposits_final += atol(data->value());
  Serial.println(deposits_final);
}


