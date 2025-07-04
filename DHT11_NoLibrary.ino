#define DHT11_PIN   12

unsigned long TON_TIME = 0;
unsigned long TOFF_TIME = 0;

unsigned char data_byte[5];
unsigned int data_packet[40];
unsigned char bit_data = 0;
unsigned char checksum_byte = 0;
int bit_counter = 0;

void setup() {
  Serial.begin(115200);
  //---------------------------------- power the dht11
  pinMode(14, OUTPUT);

  digitalWrite(14, HIGH);
  //----------------------------------
  Serial.println("DHT11 Humidity & Temperature Sensor (Raw) \n");
  delay(1000);//Wait Sensor
}

void loop() {
  pinMode(DHT11_PIN, OUTPUT);
  digitalWrite(DHT11_PIN, LOW);
  delay(18);
  digitalWrite(DHT11_PIN, HIGH);
  pinMode(DHT11_PIN, INPUT_PULLUP);

  TOFF_TIME = pulseIn(DHT11_PIN, LOW);
  if (TOFF_TIME <= 84 && TOFF_TIME >= 76) {
    while (1) {
      TON_TIME = pulseIn(DHT11_PIN, HIGH);
      if (TON_TIME <= 28 && TON_TIME >= 20) {
        bit_data = 0;
      }
      else if (TON_TIME <= 74 && TON_TIME >= 65) {
        bit_data = 1;
      }
      else if (bit_counter == 40) {
        break;
      }
      data_byte[bit_counter / 8] |= bit_data << (7 - (bit_counter % 8));
      data_packet[bit_counter] = bit_data;
      bit_counter++;

    }

  }
  checksum_byte = data_byte[0] + data_byte[1] + data_byte[2] + data_byte[3];


  Serial.print("byte dump: " + String(data_byte[0]) + " " + String(data_byte[1]) + " " + String(data_byte[2]) + " " + String(data_byte[3]) + " " + String(data_byte[4]) + " " + String(data_byte[5]));
  Serial.println(" Humidity | .Humidity (0) | Temperature | . Temperature | Checksum | Total bit length (40)");

  if (checksum_byte == data_byte[4] && checksum_byte != 0) {

    Serial.print("Checksum Byte: ");
    for (int c = 0; c <= 7; c++) {
      Serial.print(data_packet[c + 32]);
    }
    Serial.print("\t");
    Serial.print("CHECKSUM_Pass");
    Serial.println("");

  }

  else {
    Serial.print("Checksum Byte: ");
    for (int c = 0; c <= 7; c++) {
      Serial.print(data_packet[c + 32]);
    }
    Serial.print("\t");
    Serial.print("CHECKSUM_Fail");
    Serial.println("");
  }


  Serial.print("Humidity: ");
  for (int c = 0; c <= 7; c++) {
    Serial.print(data_packet[c]);
  }
  Serial.print("\t");
  for (int c = 0; c <= 7; c++) {
    Serial.print(data_packet[c + 8]);
  }
  Serial.print("\t");
  Serial.print(data_byte[0]);
  Serial.print("%");
  Serial.print("\t");

  Serial.print("Temperature: ");
  for (int c = 0; c <= 7; c++) {
    Serial.print(data_packet[c + 16]);
  }
  Serial.print("\t");
  for (int c = 0; c <= 7; c++) {
    Serial.print(data_packet[c + 24]);
  }
  Serial.print("\t");
  Serial.print(String(data_byte[2]) + "." +  String(data_byte[3]));
  Serial.print("C");
  Serial.println("");
  Serial.println("-------------------------------------------");



  bit_counter = 0;
  data_byte[0] = data_byte[1] = data_byte[2] = data_byte[3] = data_byte[4] = 0;
  delay(200);
}
