#include "button.h"

/* -------------- ButtonActionControl Scope ---------------- */
ButtonActionControl::ButtonActionControl() {}
ButtonActionControl::~ButtonActionControl() {}

void ButtonActionControl::begin(IMUControl *imu_controller)
{
  /* pinmoding */
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_MUX, INPUT);

  /* start display */
  while (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    Serial.println(F("Retrying the display..."));
  }
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();

  /* splash screen */
  display.drawBitmap(32, 0, ALTAIR_LOGO, LOGO_HEIGHT, LOGO_WIDTH, WHITE);
  display.display();
  for (uint8_t i = 0; i < 3; ++i)
  {
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

void ButtonActionControl::buttonEvent()
{
  /* debouncing */
  if (millis() - debounce_time > DEBOUNCE_MS)
  {
    /* false counter for buzzer sound */
    uint8_t button_pressed_cnt = 0;

    /* check every mux channels */
    for (uint8_t i = 0; i < 8; i++)
    {
      digitalWrite(PIN_S0, HIGH && (i & B00000001));
      digitalWrite(PIN_S1, HIGH && (i & B00000010));
      digitalWrite(PIN_S2, HIGH && (i & B00000100));
      button_state[i] = (analogRead(PIN_MUX) < 100) ? true : false;
      button_pressed_cnt = (button_state[i]) ? button_pressed_cnt + 1 : button_pressed_cnt;
    }

    /* button latching control */
    if (button_pressed_cnt > 0 && !button_down)
    {
      /* play buzzer */
      digitalWrite(PIN_BUZZER, HIGH);
      button_down = true;

      /* time control */
      start_time = millis();
      hold_time = start_time;
    }
    else if (button_pressed_cnt == 0)
    {
      /* stop buzzer */
      digitalWrite(PIN_BUZZER, LOW);
      button_down = false;
      if (button_msg != 0)
      {
        reset = true;
      }
    }

    /* button pressed */
    else if (button_down)
    {
      /* take time diff */
      uint64_t time_diff = hold_time - start_time;

      /* take button action */
      if (time_diff == 0)
      {
        /* take button action */
        if (button_state[UP_ARROW])
        {
          button_action = BUTTON_UP;
        }
        else if (button_state[RIGHT_ARROW])
        {
          button_action = BUTTON_RIGHT;
        }
        else if (button_state[DOWN_ARROW])
        {
          button_action = BUTTON_DOWN;
        }
        else if (button_state[LEFT_ARROW])
        {
          button_action = BUTTON_LEFT;
        }
        else if (button_state[A_BUTTON])
        {
          button_action = BUTTON_A;
          if (display_menu == REGIONAL_PLAY)
          {
            button_msg = 1;
          }
          else if (display_menu == NASIONAL_PLAY)
          {
            button_msg = 5;
          }
        }
        else if (button_state[B_BUTTON])
        {
          button_action = BUTTON_B;
          if (display_menu == REGIONAL_PLAY)
          {
            button_msg = 2;
          }
          else if (display_menu == NASIONAL_PLAY)
          {
            button_msg = 6;
          }
        }
        else if (button_state[C_BUTTON])
        {
          button_action = BUTTON_C;
          if (display_menu == REGIONAL_PLAY)
          {
            button_msg = 3;
          }
          else if (display_menu == NASIONAL_PLAY)
          {
            button_msg = 7;
          }
        }
        else if (button_state[D_BUTTON])
        {
          button_action = BUTTON_D;
          if (display_menu == REGIONAL_PLAY)
          {
            button_msg = 4;
          }
          else if (display_menu == NASIONAL_PLAY)
          {
            button_msg = 8;
          }
        }
      }
      else if (time_diff > BUTTON_HOLD_MS)
      {
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

void ButtonActionControl::displayEvent()
{
  /* get the button action */
  if (button_action != BUTTON_NONE)
  {
    refresh_display = true;
  }

  /* button action */
  switch (display_menu)
  {
  case MATCH:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = WIFI;
      break;

    case BUTTON_DOWN:
      display_menu = ODOMETRY;
      break;

    case BUTTON_RIGHT:
      display_menu = REGIONAL;
      break;
    }
    break;

  case REGIONAL:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = NASIONAL;
      break;

    case BUTTON_DOWN:
      display_menu = NASIONAL;
      break;

    case BUTTON_LEFT:
      display_menu = MATCH;
      break;

    case BUTTON_RIGHT:
      display_menu = REGIONAL_PLAY;
      break;
    }
    break;

  case REGIONAL_PLAY:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_A:
      break;

    case BUTTON_B:
      break;

    case BUTTON_C:
      break;

    case BUTTON_LEFT:
      display_menu = REGIONAL;
      break;

    case BUTTON_D:
      break;
    }
    break;

  case NASIONAL:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = REGIONAL;
      break;

    case BUTTON_DOWN:
      display_menu = REGIONAL;
      break;

    case BUTTON_LEFT:
      display_menu = MATCH;
      break;

    case BUTTON_RIGHT:
      display_menu = NASIONAL_PLAY;
      break;
    }
    break;

  case NASIONAL_PLAY:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_A:
      break;

    case BUTTON_B:
      break;

    case BUTTON_C:
      break;

    case BUTTON_LEFT:
      display_menu = NASIONAL;
      break;

    case BUTTON_D:
      break;
    }
    break;

  case ODOMETRY:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      if (selected_square > 0)
        selected_square--;
      break;
    case BUTTON_DOWN:
      if (selected_square < 11)
        selected_square++;
      break;
    case BUTTON_LEFT:
      display_menu = MATCH;
      selected_square = 0; // Reset selection when leaving
      break;
    case BUTTON_RIGHT:
      break;
    case BUTTON_D:
      button_msg = 40 + selected_square; // Send 40-51 (12 squares)
      break;
    }
    break;

  case SELECT_POSE:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = MATCH;
      break;

    case BUTTON_DOWN:
      display_menu = WIFI;
      break;

    case BUTTON_LEFT:
      display_menu = ODOMETRY;
      break;

    case BUTTON_RIGHT:
      display_menu = QUINTIC_WALK;
      break;
    }
    break;

  case QUINTIC_WALK:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = KICK;
      break;

    case BUTTON_DOWN:
      display_menu = KICK;
      break;

    case BUTTON_LEFT:
      display_menu = SELECT_POSE;
      break;
    }
    break;

  case KICK:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = QUINTIC_WALK;
      break;

    case BUTTON_DOWN:
      display_menu = QUINTIC_WALK;
      break;

    case BUTTON_LEFT:
      display_menu = SELECT_POSE;
      break;
    }
    break;

  case WIFI:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = SELECT_POSE;
      break;

    case BUTTON_DOWN:
      display_menu = MATCH;
      break;

    case BUTTON_RIGHT:
      display_menu = WIFI_ALTAIR;
      break;
    }
    break;

  case WIFI_ALTAIR:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = WIFI_AUTO;
      break;

    case BUTTON_DOWN:
      display_menu = WIFI_KRI;
      break;

    case BUTTON_LEFT:
      display_menu = WIFI;
      break;
    }
    break;

  case WIFI_KRI:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = WIFI_ALTAIR;
      break;

    case BUTTON_DOWN:
      display_menu = WIFI_AUTO;
      break;

    case BUTTON_LEFT:
      display_menu = WIFI;
      break;
    }
    break;

  case WIFI_AUTO:
    /* process the button input */
    switch (button_action)
    {
    case BUTTON_UP:
      display_menu = WIFI_KRI;
      break;

    case BUTTON_DOWN:
      display_menu = WIFI_ALTAIR;
      break;

    case BUTTON_LEFT:
      display_menu = WIFI;
      break;
    }
    break;

  }
  button_action = BUTTON_NONE;

  /* refresh the display as required */
  if (refresh_display)
  {
    /* clear the previous display */
    display.clearDisplay();

    switch (display_menu)
    {
    case MATCH:
      display.setCursor(0, 0);
      display.print("______MAIN MENU______");
      display.setCursor(0, 12);
      display.print(">> [Match Gameplay]");
      display.setCursor(0, 24);
      display.print(" Odometry Position");
      display.setCursor(0, 36);
      display.print(" Select Pose");
      display.setCursor(0, 48);
      display.print(" Wifi");
      break;

    case REGIONAL:
      display.setCursor(0, 0);
      display.print("___MATCH GAMEPLAY____");
      display.setCursor(0, 12);
      display.print(">> [Regional KRI]");
      display.setCursor(0, 24);
      display.print(" National KRI");
      break;

    case REGIONAL_PLAY:
      display.setCursor(0, 0);
      display.print("____REGIONAL KRI_____");
      display.setTextSize(2);
      switch (button_msg)
      {
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

      // Draw button visualization
      drawButtonVisualization();
      break;

    case NASIONAL:
      display.setCursor(0, 0);
      display.print("___MATCH GAMEPLAY____");
      display.setCursor(0, 12);
      display.print(" Regional KRI");
      display.setCursor(0, 24);
      display.print(">> [National KRI]");
      break;

    case NASIONAL_PLAY:
      display.setCursor(0, 0);
      display.print("____NATIONAL KRI_____");
      display.setTextSize(2);
      switch (button_msg)
      {
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

      // Draw button visualization
      drawButtonVisualization();
      break;

    case ODOMETRY:
      display.setCursor(0, 0);
      display.print("___ODOMETRY MAP___");

      // Draw the field squares
      int squares[12][4] = {

          // Button data is 40 + selected_square

          {10, 10, 10, 10},  // 0: Top Left
          {106, 10, 10, 10}, // 1: Top Right
          {30, 14, 10, 10},  // 2: Left Top
          {40, 27, 10, 10},  // 3: Left Middle
          {30, 40, 10, 10},  // 4: Left Bottom
          {88, 14, 10, 10},  // 5: Right Top
          {78, 27, 10, 10},  // 6: Right Middle
          {88, 40, 10, 10},  // 7: Right Bottom
          {10, 54, 10, 10},  // 8: Bottom Left
          {106, 54, 10, 10}, // 9: Bottom Right

          // Keeper positions
          {5, 27, 10, 10},   // 10: Left Keeper
          {111, 27, 10, 10}, // 11: Right Keeper
      };

      // Draw field boundary
      display.drawRect(0, 8, 128, 56, WHITE);

      // Draw all squares
      for (int i = 0; i < 12; i++)
      {
        if (i == selected_square)
        {
          // Fill selected square
          display.fillRect(squares[i][0], squares[i][1], squares[i][2], squares[i][3], WHITE);
        }
        else
        {
          // Draw outline for unselected squares
          display.drawRect(squares[i][0], squares[i][1], squares[i][2], squares[i][3], WHITE);
        }
      }

      // Show selection info
      display.setCursor(0, 0);
      display.print("Pos: ");
      display.print(selected_square);
      display.print(" (");
      display.print(40 + selected_square);
      display.print(")");
      break;

    case SELECT_POSE:
      display.setCursor(0, 0);
      display.print("______MAIN MENU______");
      display.setCursor(0, 12);
      display.print(" Match Gameplay");
      display.setCursor(0, 24);
      display.print(" Odometry Position");
      display.setCursor(0, 36);
      display.print(">> [Select Pose]");
      display.setCursor(0, 48);
      display.print(" Wifi");
      break;

    case QUINTIC_WALK:
      display.setCursor(0, 0);
      display.print("_____SELECT POSE_____");
      display.setCursor(0, 12);
      display.print(">> [Quintic Walk]");
      display.setCursor(0, 24);
      display.print(" Kick");
      break;

    case KICK:
      display.setCursor(0, 0);
      display.print("_____SELECT POSE_____");
      display.setCursor(0, 12);
      display.print(" Quintic Walk");
      display.setCursor(0, 24);
      display.print(">> [Kick]");
      break;

    case WIFI:
      display.setCursor(0, 0);
      display.print("______MAIN MENU______");
      display.setCursor(0, 12);
      display.print(" Match Gameplay");
      display.setCursor(0, 24);
      display.print(" Odometry Position");
      display.setCursor(0, 36);
      display.print(" Select Pose");
      display.setCursor(0, 48);
      display.print(">> [Wifi]");
      break;

    case WIFI_ALTAIR:
      display.setCursor(0, 0);
      display.print("________WIFI_________");
      display.setCursor(0, 12);
      display.print(">> [SSID: altairjuara]");
      display.setCursor(0, 24);
      display.print(" Use KRI field Wifi");
      display.setCursor(0, 36);
      display.print(" Use Automatic");
      break;

    case WIFI_KRI:
      display.setCursor(0, 0);
      display.print("________WIFI_________");
      display.setCursor(0, 12);
      display.print(" SSID: altairjuara");
      display.setCursor(0, 24);
      display.print(">> [Use KRI field Wifi]");
      display.setCursor(0, 36);
      display.print(" Use Automatic");
      break;

    case WIFI_AUTO:
      display.setCursor(0, 0);
      display.print("________WIFI_________");
      display.setCursor(0, 12);
      display.print(" SSID: altairjuara");
      display.setCursor(0, 24);
      display.print(" Use KRI field Wifi");
      display.setCursor(0, 36);
      display.print(">> [Use Automatic]");
      break;

    }

    /* display the new one */
    display.display();
    refresh_display = false;
  }
}

void ButtonActionControl::actionController()
{
  //  if(reset) {
  //    button_msg = 0;
  //    Serial.print(button_msg);
  //    reset = false;
  //    return;
  //  }
  switch (display_menu)
  {
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

void ButtonActionControl::send()
{
  Serial.print('B');
  Serial.print(button_msg);

  if (reset)
  {
    button_msg = 0;
    reset = false;
  }
}

void ButtonActionControl::buttonService()
{
  /* combine! */
  this->buttonEvent();
  this->displayEvent();
  this->actionController();
  this->send();
}

// Add this helper function to draw the button visualization
void ButtonActionControl::drawButtonVisualization()
{
  // Draw 4 squares at the bottom with A, B, C, D labels and numbers
  // Starting positions for the 4 boxes
  const int boxSize = 12;
  const int yPos = 48;
  const int spacing = 35;

  for (int i = 0; i < 4; i++)
  {
    int xPos = 5 + (i * spacing);
    // Draw box
    display.drawRect(xPos, yPos, boxSize, boxSize, WHITE);

    // Draw label (A, B, C, D)
    display.setCursor(xPos + 4, yPos + 2);
    display.print(char('A' + i));

    display.setCursor(xPos + 4, yPos + boxSize + 2);
  }
}
