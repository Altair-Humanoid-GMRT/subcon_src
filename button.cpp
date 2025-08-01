#include "button.h"

#include "packet.h"

/* -------------- ButtonActionControl Scope ---------------- */
ButtonActionControl::ButtonActionControl() {}
ButtonActionControl::~ButtonActionControl() {}

void ButtonActionControl::begin(IMUControl* imu_controller) {
  /* pinmoding */
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_MUX, INPUT);

  /* start display */
  while (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Retrying the display..."));
  }
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();

  /* splash screen */
  display.drawBitmap(32, 0, ALFAROBI_LOGO, LOGO_HEIGHT, LOGO_WIDTH, WHITE);
  display.display();
  for (uint8_t i = 0; i < 3; ++i) {
    display.invertDisplay(true);
    delay(100);
    digitalWrite(PIN_BUZZER, HIGH);
    display.invertDisplay(false);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);
  }

  /* initialize IMU */
  imu_controller->begin(&display);
}

void ButtonActionControl::buttonEvent() {
  /* debouncing */
  if (millis() - debounce_time > DEBOUNCE_MS) {
    /* false counter for buzzer sound */
    uint8_t button_pressed_cnt = 0;

    /* check every mux channels */
    for (uint8_t i = 0; i < 8; i++) {
      digitalWrite(PIN_S0, HIGH && (i & B00000001));
      digitalWrite(PIN_S1, HIGH && (i & B00000010));
      digitalWrite(PIN_S2, HIGH && (i & B00000100));
      button_state[i] = (analogRead(PIN_MUX) < 100) ? true : false;
      button_pressed_cnt =
          (button_state[i]) ? button_pressed_cnt + 1 : button_pressed_cnt;
    }

    /* button latching control */
    if (button_pressed_cnt > 0 && !button_down) {
      /* play buzzer */
      digitalWrite(PIN_BUZZER, HIGH);
      button_down = true;

      /* time control */
      start_time = millis();
      hold_time = start_time;
    } else if (button_pressed_cnt == 0) {
      /* stop buzzer */
      digitalWrite(PIN_BUZZER, LOW);
      button_down = false;
      if (button_msg != 0) {
        reset = true;
      }
    }

    /* button pressed */
    else if (button_down) {
      /* take time diff */
      uint64_t time_diff = hold_time - start_time;

      /* take button action */
      if (time_diff == 0) {
        /* take button action */
        if (button_state[UP_ARROW]) {
          button_action = BUTTON_UP;
          //          button_msg    = "^";
        } else if (button_state[RIGHT_ARROW]) {
          button_action = BUTTON_RIGHT;
          //          button_msg    = ">";
        } else if (button_state[DOWN_ARROW]) {
          button_action = BUTTON_DOWN;
          //          button_msg    = "v";
        } else if (button_state[LEFT_ARROW]) {
          button_action = BUTTON_LEFT;
          //          button_msg    = "<";
        } else if (button_state[A_BUTTON]) {
          button_action = BUTTON_A;
          if (display_menu == L_1_1_A) {
            button_msg = 1;
          } else if (display_menu == L_1_2_A) {
            button_msg = 5;
          }
          //          button_msg    = 1;
          //          reset         = 0;
        } else if (button_state[B_BUTTON]) {
          button_action = BUTTON_B;
          if (display_menu == L_1_1_A) {
            button_msg = 2;
          } else if (display_menu == L_1_2_A) {
            button_msg = 6;
          }
          //          button_msg    = 2;
          //          reset         = 0;
        } else if (button_state[C_BUTTON]) {
          button_action = BUTTON_C;
          if (display_menu == L_1_1_A) {
            button_msg = 3;
          } else if (display_menu == L_1_2_A) {
            button_msg = 7;
          }
          //          button_msg    = 3;
          //          reset         = 0;
        } else if (button_state[D_BUTTON]) {
          button_action = BUTTON_D;
          if (display_menu == L_1_1_A) {
            button_msg = 4;
          } else if (display_menu == L_1_2_A) {
            button_msg = 8;
          }
          //          button_msg    = 4;
          //          reset         = 0;
        }
      } else if (time_diff > BUTTON_HOLD_MS) {
        /* restart time */
        start_time = millis();
        hold_time = start_time;

        /* reset debouncer */
        debounce_time = millis();
        return;
      }

      /* take time */
      hold_time = millis();
    }

    /* reset debouncer */
    debounce_time = millis();
  }
}

void ButtonActionControl::displayEvent() {
  /* get the button action */
  if (button_action != BUTTON_NONE) {
    refresh_display = true;
  }

  /* button action */
  switch (display_menu) {
    case L_1:
      /* process the button input */
      switch (button_action) {
        case BUTTON_UP:
          display_menu = L_4;
          break;

        case BUTTON_DOWN:
          display_menu = L_2;
          break;

        case BUTTON_RIGHT:
          display_menu = L_1_1;
          break;
      }
      break;

    case L_1_1:
      /* process the button input */
      switch (button_action) {
        case BUTTON_UP:
          display_menu = L_1_2;
          break;

        case BUTTON_DOWN:
          display_menu = L_1_2;
          break;

        case BUTTON_LEFT:
          display_menu = L_1;
          break;

        case BUTTON_RIGHT:
          display_menu = L_1_1_A;
          break;
      }
      break;

    case L_1_1_A:
      /* process the button input */
      switch (button_action) {
        case BUTTON_A:
          // button_msg = 1;
          break;

        case BUTTON_B:
          // button_msg = 2;
          break;

        case BUTTON_LEFT:
          //          display_menu = L_1_1;
          // button_msg = 3;
          display_menu = L_1_1;
          break;

        case BUTTON_D:
          // button_msg = 4;
          break;
        case BUTTON_NONE:
          // button_msg = 5;
          break;
        default:
          //          button_msg = 0;
          break;
      }
      break;

    case L_1_2:
      /* process the button input */
      switch (button_action) {
        case BUTTON_UP:
          display_menu = L_1_1;
          break;

        case BUTTON_DOWN:
          display_menu = L_1_1;
          break;

        case BUTTON_LEFT:
          display_menu = L_1;
          break;

        case BUTTON_RIGHT:
          display_menu = L_1_2_A;
          break;
      }
      break;

    case L_1_2_A:
      /* process the button input */
      switch (button_action) {
        case BUTTON_A:
          // button_msg = 5;
          break;

        case BUTTON_B:
          // button_msg = 6;
          break;

        case BUTTON_LEFT:
          //          display_menu = L_1_2;
          // button_msg = 7;
          display_menu = L_1_2;
          break;

        case BUTTON_D:
          // button_msg = 8;
          break;
        default:
          //          button_msg = 0;
          break;
      }
      break;

    case L_2:
      /* process the button input */
      switch (button_action) {
        case BUTTON_UP:
          display_menu = L_1;
          break;

        case BUTTON_DOWN:
          display_menu = L_3;
          break;

        case BUTTON_D:
          break;
      }
      break;

    case L_3:
      /* process the button input */
      switch (button_action) {
        case BUTTON_UP:
          display_menu = L_2;
          break;

        case BUTTON_DOWN:
          display_menu = L_4;
          break;

        case BUTTON_D:
          break;
      }
      break;

    case L_4:
      /* process the button input */
      switch (button_action) {
        case BUTTON_UP:
          display_menu = L_3;
          break;

        case BUTTON_DOWN:
          display_menu = L_1;
          break;

        case BUTTON_RIGHT:
          display_menu = L_4_A;
          break;

        default:
          break;
      }
      break;
  }
  button_action = BUTTON_NONE;

  /* refresh the display as required */
  if (refresh_display) {
    /* clear the previous display */
    display.clearDisplay();

    switch (display_menu) {
      case L_1:
        display.setCursor(0, 0);
        display.print("______MAIN MENU______");
        display.setCursor(0, 12);
        display.print(">> [Match Gameplay]");
        display.setCursor(0, 24);
        display.print(" Select Pose");
        display.setCursor(0, 36);
        display.print(" Play Sequential");
        display.setCursor(0, 48);
        display.print(" Turn Off");
        break;

      case L_1_1:
        display.setCursor(0, 0);
        display.print("___MATCH GAMEPLAY____");
        display.setCursor(0, 12);
        display.print(">> [Regional KRI]");
        display.setCursor(0, 24);
        display.print(" National KRI");
        break;

      case L_1_1_A:
        display.setCursor(0, 0);
        display.print("____REGIONAL KRI_____");
        display.setTextSize(2);
        // display.setCursor(0, 12); display.print(">> [Regional KRI]");
        // display.setCursor(0, 24); display.print(" National KRI");
        switch (button_msg) {
          case 1:
            display.setCursor(0, 12);
            display.print("Button A");
            break;
          case 2:
            display.setCursor(0, 12);
            display.print("Button B");
            break;
          case 3:
            display.setCursor(0, 12);
            display.print("Button C");
            break;
          case 4:
            display.setCursor(0, 12);
            display.print("Button D");
            break;
          default:
            display.setCursor(0, 12);
            display.print("");
            break;
        }

        display.setTextSize(1);
        break;

      case L_1_2:
        display.setCursor(0, 0);
        display.print("___MATCH GAMEPLAY____");
        display.setCursor(0, 12);
        display.print("Regional KRI");
        display.setCursor(0, 24);
        display.print(">> [National KRI]");
        break;

      case L_1_2_A:
        display.setCursor(0, 0);
        display.print("____NATIONAL KRI_____");
        // display.setCursor(0, 12); display.print(">> [Regional KRI]");
        // display.setCursor(0, 24); display.print(" National KRI");
        display.setTextSize(2);
        switch (button_msg) {
          case 5:
            display.setCursor(0, 12);
            display.print("Button A");
            break;
          case 6:
            display.setCursor(0, 12);
            display.print("Button B");
            break;
          case 7:
            display.setCursor(0, 12);
            display.print("Button C");
            break;
          case 8:
            display.setCursor(0, 12);
            display.print("Button D");
            break;
          default:
            display.setCursor(0, 12);
            display.print("");
            break;
        }
        display.setTextSize(1);
        break;

      case L_2:
        display.setCursor(0, 0);
        display.print("______MAIN MENU______");
        display.setCursor(0, 12);
        display.print(" Match Gameplay");
        display.setCursor(0, 24);
        display.print(">> [Select Pose]");
        display.setCursor(0, 36);
        display.print(" Play Sequential");
        display.setCursor(0, 48);
        display.print(" Turn Off");
        break;

      case L_3:
        display.setCursor(0, 0);
        display.print("______MAIN MENU______");
        display.setCursor(0, 12);
        display.print(" Match Gameplay");
        display.setCursor(0, 24);
        display.print(" Select Pose");
        display.setCursor(0, 36);
        display.print(">> [Play Sequential]");
        display.setCursor(0, 48);
        display.print(" Turn Off");
        break;

      case L_4:
        display.setCursor(0, 0);
        display.print("______MAIN MENU______");
        display.setCursor(0, 12);
        display.print(" Match Gameplay");
        display.setCursor(0, 24);
        display.print(" Select Pose");
        display.setCursor(0, 36);
        display.print(" Play Sequential");
        display.setCursor(0, 48);
        display.print(">> [Turn Off]");
        break;

      case L_4_A:
        display.setTextSize(2);
        display.setCursor(0, 12);
        display.print("Turning\noff...");
        break;
    }

    /* display the new one */
    display.display();
    refresh_display = false;
  }
}

void ButtonActionControl::actionController() {
  //  if(reset) {
  //    button_msg = 0;
  //    Serial.print(button_msg);
  //    reset = false;
  //    return;
  //  }
  switch (display_menu) {
    case REGIONAL_KRI:
      // Serial.print("mode:"); Serial.println("REGIONAL_KRI");
      // Serial.print("button:"); Serial.println(button_msg);
      //      if(button_action == BUTTON_A) {
      //        button_msg = 1;
      //        reset = 0;
      //      }
      //      else if(button_action == BUTTON_B) {
      //        button_msg = 2;
      //        reset = 0;
      //      }
      //      else if(button_action == BUTTON_C) {
      //        button_msg = 3;
      //        reset = 0;
      //      }
      //      else if(button_action == BUTTON_D) {
      //        button_msg = 4;
      //        reset = 0;
      //      }
      break;

    case NATIONAL_KRI:
      //      Serial.print("mode:"); Serial.println("NATIONAL_KRI");
      //      Serial.print("button:"); Serial.println(button_msg);
      break;

    case TURN_OFF:
      //      Serial.print("mode:"); Serial.println("TURN_OFF");
      //      Serial.print("button:"); Serial.println(button_msg);
      break;

    default:
      // Serial.print("mode:"); Serial.println("_");
      // Serial.print("button:"); Serial.println(button_msg);
      //      button_msg = "0";
      break;
  }
}

void ButtonActionControl::send() {
  //  if(reset == 2) {
  //    Serial.print('B');
  //    Serial.print(5);
  //    Serial.print('A');
  //    reset--;
  //  }
  //  else if(reset == 1){
  //    Serial.print('B');
  //    Serial.print('A');
  //  }
  //  else if(reset == 0) {
  //    Serial.print('B');
  //    Serial.print(button_msg);
  //    Serial.print('A');
  //    reset = 2;
  //  }
  // Serial.print('B');
  // Serial.print('A');
  packet.buttonState = static_cast<float>(5);

  if (reset) {
    button_msg = 0;
    reset = false;
  }
}

void ButtonActionControl::buttonService() {
  /* combine! */
  this->buttonEvent();
  this->displayEvent();
  this->actionController();
  this->send();
}
