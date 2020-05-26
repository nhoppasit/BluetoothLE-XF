/*
26-05-2020
*/
#define DEVICE_NAME "MCU"
#define _DEBUG_SERIAL_ 0

//
#define CMD_INFO "?"          // !!! CAREFUL! CMD NEEDS TO ADD isCommand()
#define CMD_BLINK_OFF "b"     // !!! CAREFUL! CMD NEEDS TO ADD isCommand()
#define CMD_BLINK_ON "B"      // !!! CAREFUL! CMD NEEDS TO ADD isCommand()
//
#define STX ':'
#define ETX '\n'
#define ACK '*'
#define NAK '!'
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete
bool STX_COME = false;

// Unlocked flag
bool IsUnlocked = true;
//
unsigned long t0Blink = 0;
bool blinkState = 0;
bool blinkFlag = false;
int blinkTime = 500;
//
/*
-----------------------------------------------------------------------------
SETUP
-----------------------------------------------------------------------------
*/
void setup()
{

  //
  Serial.begin(9600);
  delay(250);
  Serial1.begin(9600);
  delay(250);

  Serial.print(DEVICE_NAME);
  Serial.println(" START.");

  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, 1); // LED OFF
  t0Blink = millis();
} // SETUP END.

void loop()
{
  serialEvent();
  update_input();
  blink(blinkFlag);
  blinkON(IsUnlocked && stringComplete && inputString.substring(0, 1).equals(CMD_BLINK_ON));
  blinkOFF(IsUnlocked && stringComplete && inputString.equals(CMD_BLINK_OFF));
  info(IsUnlocked && stringComplete && inputString.equals(CMD_INFO));
  //
  ClearSerialEvent(stringComplete);
} // LOOP END.
//
#pragma region BLINK
/*
-----------------------------------------------------------------------------
BLINK CONTROL
-----------------------------------------------------------------------------
*/
void blinkON(bool flag)
{
  if (flag)
  {
    blinkTime = inputString.substring(1).toInt();
    if (blinkTime <= 0)
    {
      blinkTime = 500;
    }
    if (10e3 < blinkTime)
    {
      blinkTime = 10e3;
    }
    blinkFlag = true;
    Serial.println(ACK);
    Serial.print("BLINK ON with time of ");
    Serial.print(blinkTime);
    Serial.println(" ms.");
  }
}
void blinkOFF(bool flag)
{
  if (flag)
  {
    digitalWrite(PC13, 1);
    blinkFlag = false;
    Serial.println(ACK);
    Serial.println("Blink OFF.");
  }
}
//
void blink(bool flag)
{
  if (flag)
  {
    if (blinkTime == 0)
      blinkTime = 500;
    if (blinkTime < (millis() - t0Blink))
    {
      blinkState = !blinkState;
      if (blinkState)
      {
        digitalWrite(PC13, 1);
        //Serial.print("OFF");
      }
      else
      {
        digitalWrite(PC13, 0);
        //Serial2.print("ON");
      }
      t0Blink = millis();
    }
  }
}
#pragma endregion
//
#pragma region SERIAL PORT / UART
/*
-----------------------------------------------------------------------------
ECHO DEVICE INFO
-----------------------------------------------------------------------------
*/
void info(bool flag)
{
  if (flag)
  {
    Serial.print(DEVICE_NAME);
    Serial.println();
  }
} // INFO END.

/*
------------------------------------------------------------------------
 Serial event
------------------------------------------------------------------------
*/
void serialEvent()
{
  if (Serial.available())
  {

    // get the new byte:
    char inChar = (char)Serial.read();
#if _DEBUG_SERIAL_
    Serial.println(inChar);
#endif

    _OPT_SHOW_INPUT_ = false;
    _OPT_SHOW_PWM_ = false;
    stop_all_motor();

    // add it to the inputString:
    if (STX_COME)
    {
      if (inChar == ETX)
      {
        stringComplete = true;
#if _DEBUG_SERIAL_
        Serial.println("ETX come.");
#endif
        return;
      }
      if (inChar != STX && inChar != '\r' && inChar != ETX)
      {
        inputString += inChar;
      }
      return;
    }

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == STX)
    {
      STX_COME = true;
      stringComplete = false;
      inputString = "";
#if _DEBUG_SERIAL_
      Serial.println("STX come.");
#endif
      return;
    }

    if (!STX_COME && IsUnlocked && inChar != ETX && inChar != '\r')
    {
      IsUnlocked = false;
      blinkFlag = false;
      blinkOFF(true);
      Serial.print(NAK);
      Serial.println("CL");
      return;
    }
  }
}
//
void ClearSerialEvent(bool flag)
{
  if (flag)
  {
#if _DEBUG_SERIAL_
    Serial.println("Clear serial event.");
#endif
    STX_COME = false;
    stringComplete = false;
    inputString = "";
  }
}

#pragma endregion
