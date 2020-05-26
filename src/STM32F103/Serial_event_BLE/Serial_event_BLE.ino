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
#define STX1 0xFD
#define STX2 0xFE
#define ETX1 0xCA
#define ACK '*'
#define NAK '!'
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete
bool STX_COME = false;
String inputString1 = "";     // a String to hold incoming data
bool stringComplete1 = false; // whether the string is complete
bool STX_COME1 = false;
bool STX_COME2 = false;

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
  Serial2.begin(9600);
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
  serialEvent1();
  //
  blink(blinkFlag);
  //
  blinkON(stringComplete && inputString.substring(0, 1).equals(CMD_BLINK_ON));
  blinkOFF(stringComplete && inputString.equals(CMD_BLINK_OFF));
  info(stringComplete && inputString.equals(CMD_INFO));
  //
  blinkON(STX_COME1 && stringComplete1 && inputString1.substring(0, 1).equals(CMD_BLINK_ON));
  blinkOFF(STX_COME1 && stringComplete1 && inputString1.equals(CMD_BLINK_OFF));
  info1(STX_COME2 && stringComplete1 && inputString1.equals(CMD_INFO));
  //
  ClearSerialEvent(stringComplete || stringComplete1);
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
void info1(bool flag)
{
  if (flag)
  {
    Serial2.print(DEVICE_NAME);
    Serial2.println();
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
    
  }
}
//
void serialEvent1()
{
  if (Serial2.available())
  {

    // get the new byte:
    char inChar = (char)Serial2.read();
    Serial.println(inChar);
    
    // add it to the inputString:
    if (STX_COME1 || STX_COME2)
    {
      if (inChar == ETX1)
      {
        stringComplete1 = true;
        Serial.println(inputString1);
        Serial.println("ETX1 come.");
        return;
      }
      if (inChar != STX1 && inChar != STX2 && inChar != '\n' && inChar != '\r' && inChar != ETX1)
      {
        inputString1 += inChar;
      }
      return;
    }

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == STX1)
    {
      STX_COME1 = true;
      stringComplete1 = false;
      inputString1 = "";
      Serial.println("STX1 come.");
      return;
    }

    if (inChar == STX2)
    {
      STX_COME2 = true;
      stringComplete1 = false;
      inputString1 = "";
      Serial.println("STX2 come.");
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
    STX_COME1 = false;
    STX_COME2 = false;
    stringComplete1 = false;
    inputString1 = "";
  }
}

#pragma endregion
