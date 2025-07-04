#include <SoftwareSerial.h> //6.10.0
#include <TinyGPS++.h>

SoftwareSerial LoRaSerial(26, 27); // RX, TX
int END = 0;
char in ;
String command = "" ;
String LoRaCommand = "" ;
long range = 3;
int Send = 0;
int n = 0;
int SendCount = 0;
int Press = 0 ;
int GPSInRange = 0 ;
int UseGPS = 1;

String RouteID = "2F";
String StopID[4] = {"001", "003", "123", "321"};
int LEDPIN = 12, BUTTON = 22;

TinyGPSPlus gps;//add
static const double IVEST_LAT = 22.391006 , IVEST_LON = 114.197873; //add

void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}



void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial2.begin(9600, SERIAL_8N1, 17, 16); //add

  pinMode(LEDPIN, OUTPUT);
  //pinMode(BUTTON, INPUT_PULLDOWN);
  pinMode(BUTTON, INPUT);
  digitalWrite(LEDPIN, HIGH);
  delay(1000);
  digitalWrite(LEDPIN, LOW);
  delay(4000);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Native USB only
  //  }
  Serial.println("System Boot");

  // set the data rate for the SoftwareSerial port
  LoRaSerial.begin(9600);
  LoRaSerial.println("LoRa Boot");
}

void loop() // run over and over
{
  unsigned long distancetoIVEST = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), IVEST_LAT, IVEST_LON);



  //Serial.print(digitalRead(BUTTON));
  //----------------------------------------------------------------------------
  if (LoRaSerial.available()) {
    in = LoRaSerial.read();
    LoRaCommand = String(LoRaCommand) + char(in);


    if (char(in) == char('\n')  )
    {
      END = 1;
      Serial.print("LoRa input:");
      Serial.println(LoRaCommand);
    }
  }

  if (Serial.available()) {
    in = Serial.read();
    command = String(command) + char(in);
    //Serial.print("input");
    //Serial.println(command);

    if (char(in) == char('\n')  )
    {
      END = 1;
      Serial.print("Command: ");
      Serial.println(String(command));
    }
  }

  if (END == 1) {
    Serial.println(String("Command:") + command);
    END = 0;

    //Serial.print(command.indexOf("TEST"));
    if (command.indexOf("Press") > -1) {
      digitalWrite(LEDPIN, 0);
      Serial.println(digitalRead(BUTTON));
      Serial.print("Pressed!!");
      if (Send == 0) {
        Send = 1 ;
      }
    }

    if (command.indexOf("HELP") > -1) {

      Serial.println(StopID[n] + RouteID + String("done-ack"));
      Serial.println("TEST ID SetID SetRoute Range Press OFF");
    }

    if (command.indexOf("ID") > -1) {
      Serial.print(StopID[n]);
      Serial.print(" ");
      Serial.println(RouteID);
    }

    if (command.indexOf("SetID") > -1) {
      Serial.println("Set ID mode");
      StopID[n] = "";
      while (1) {
        if (Serial.available()) {
          in = Serial.read();
          command = String(command) + char(in);
          //Serial.print("input");
          //Serial.println(command);

          if (char(in) == char('\n')  )
          {
            break;
          }

          if (isDigit(in)) {
            StopID[n] = StopID[n] + char(in);
            Serial.print(StopID[n]);
            Serial.print("------");
            command = "";
          }

        }
      }
    }

    if (command.indexOf("SetRoute") > -1) {
      Serial.println("Set Route mode");
      RouteID = "";
      while (1) {
        if (Serial.available()) {
          in = Serial.read();
          command = String(command) + char(in);
          //Serial.print("input");
          //Serial.println(command);

          if (char(in) == char('\n')  )
          {
            break;
          }

          if (isAlphaNumeric(in)) {
            RouteID = RouteID + char(in);
            Serial.print(RouteID);
            Serial.print("------");
            command = "";
          }

        }
      }
    }


    if (command.indexOf("Range") > -1) {
      while (Serial2.available()) {
        gps.encode(Serial2.read());
      }
      Serial.print(gps.location.lat(), 6);
      Serial.print("        ");
      Serial.println(gps.location.lng(), 6);

      Serial.println("distance:");
      printInt(distancetoIVEST, gps.location.isValid(), 9);
    }


    if (command.indexOf("TEST") > -1) {
      Serial.println("It work");
      LoRaSerial.println("It work");
      digitalWrite(LEDPIN, 1);
      delay(200);
      digitalWrite(LEDPIN, 0);
      delay(200);
      digitalWrite(LEDPIN, 1);
      delay(200);
      digitalWrite(LEDPIN, 0);
      delay(200);
      LoRaSerial.print("LoRa Send success");
      while (Serial2.available()) {
        char GPS = Serial2.read();
        Serial.write(GPS);
      }

      Serial.println("distance:");
      printInt(distancetoIVEST, gps.location.isValid(), 9);

      Serial.print(gps.location.lat(), 6);
      Serial.print("        ");
      Serial.println(gps.location.lng(), 6);

    }

    if (command.indexOf("Reboot") > -1) {
      Serial.println("Rebooting...");
      ESP.restart();
    }


    //-----------------------------------------------------------------------------------------command group end

    if (LoRaCommand.indexOf( StopID[n] + String("call") + RouteID) > -1) { //<stationID>call<RouteID> 006call2F
      Serial.println("ID matched !!!!!!!");
      digitalWrite(LEDPIN, 1);
      Serial.println("distance:");
      printInt(distancetoIVEST, gps.location.isValid(), 9);

      Serial.print(gps.location.lat(), 6);
      Serial.print("        ");
      Serial.println(gps.location.lng(), 6);
    }

    if (LoRaCommand.indexOf( StopID[n] + RouteID + String("done-ack")) > -1) {  //<stationID><RouteID>done-ack
      digitalWrite(LEDPIN, 0);
      Serial.println("Ack done");
      Send = 0;
      //GPSInRange = 0;

      n = n + 1;

      if (n > 4) {
        n = 0;
      }
    }

    if (Send == 2 && (LoRaCommand.indexOf( StopID[n] + RouteID + String("done-ack")) < 0) ) {
      Send = 1 ;
      Serial.println("ReSend");
    }

    if (command.indexOf("OFF") > -1) {
      digitalWrite(LEDPIN, 0);
      LoRaSerial.print(RouteID);
      LoRaSerial.print("done");
    }

    command = String("");
    LoRaCommand = String("");

  }
  //---------------------------------------------------------------------------------------------------


  LoRaSerial.print(command);


  if (digitalRead(BUTTON)) {
    digitalWrite(LEDPIN, 0);
    Serial.println(digitalRead(BUTTON));

    if (Press == 0) {
      Send = 1 ;
      Press = 1;
    }
  }

  if (!digitalRead(BUTTON) && Press == 1) {
    Press = 0;
  }

  if (distancetoIVEST < range && UseGPS == 1) {
    Serial.println("In Range ");
    UseGPS = 0;
    if (GPSInRange == 0) {
      Send = 1 ;
      GPSInRange = 1;
    }
  }

  if (Send == 1) {
    Serial.print("Send");
    LoRaSerial.print(RouteID);
    LoRaSerial.print("done");
    LoRaSerial.print(StopID[n]);
    Serial.println("Target RX ack:" + StopID[n] + String("call") + RouteID);

    SendCount = SendCount + 1;

    Send = 2; //call check

    if (SendCount > 10) {
      Send = 0;
      SendCount = 0;
    }



  }

}
//---------------------------------------------------------------------
