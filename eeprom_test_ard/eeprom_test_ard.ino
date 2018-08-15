#include <Wire.h>


#define ADDRESS 0x50

//Crc 16 library (XModem)
//Crc16 crc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("CRC-16 bit test program");
  Serial.println("=======================");
  Wire.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long num_bytes = 0;
  int dev_type = 0;
  read_large_eeprom(ADDRESS, 0, 10);
}





void read_large_eeprom(int address, unsigned long first, unsigned long bytes) {
  byte value;
  unsigned long add;
  byte Error_eeprom = 0; //show if somewhere error happened
  for (add = first; add < bytes; add++) {
    if (Error_eeprom != 0) {
      Serial.write("ERRoR");
      break;
    }
//Serial.write(address);
    Wire.beginTransmission(address);
    ////starting transmitting over i2c with current adress
    ////После него последовательность байтов для передачи ставится
    ////в очередь с помощью функции write(),
    ////и их передача с помощью вызова endTransmission().
    Wire.write(byte(add >> 8)); //MSB
    Wire.write(byte(add & 0xFF));  //LSB
    Wire.endTransmission();//
    //Wire.beginTransmission(address);
    Wire.requestFrom(address, 1);
    // Используется мастером для запроса байтов от ведомого устройства.
    // Эти байты могут быть получены с помощью методов available() и read()
    // Wire.requestFrom(address, quantity) address: 7-битный адрес устройства,
    // у которого запрашиваются байты; quantity:
    // количество запрашиваемых байтов;
    delay(1);
    //read 1 byte from i2c eeprom and encode it to send over rs-232
    if (Wire.available()) {
      value = Wire.read();
      int i=0;
      //  The packet format is as follows:
      //  @@Header(0x06,0x85)|SizeofPayload|Payload|CRC16(XModem)@@

      byte eeprom_packet[5];
      eeprom_packet[0] = 6;
      eeprom_packet[1] = 133;
      eeprom_packet[2] = 8;
      eeprom_packet[3] = 130; //value;
      eeprom_packet[4] = calcrc((char*)eeprom_packet, 4);
      byte data[] = "61338130";
      unsigned short value = calcrc((char*)data, 8);
      unsigned short vvvvv = calcrc((char*)eeprom_packet, 4);
      Serial.print("crc DEFAULT = 0x");
      Serial.println(value, HEX);

    //  for (i = 0; i <= 4; i++) {
    //      Serial.print(eeprom_packet[i]);
    //
    //  }
      Serial.print(eeprom_packet[0]);
      Serial.print(eeprom_packet[1]);
      Serial.print(eeprom_packet[2]);
      Serial.print(eeprom_packet[3]);
      Serial.print("crc = 0x");
      Serial.println(vvvvv, HEX);


    for (i = 1; i <= 5; i++) {
    char response[] = "O""K";
      //checking that data transfed and crc16 is correct,
      //before transfer next byte
      //5 times try to transfer and wait datacheck good answer('OK\n')
      //or ter
      //if no - mark error flag
      delay(5);
      if  (Serial.find(response)) {
        //Serial.findUntil has 1 sec timeout, after it done
        break;
      }
      if (i >= 5) {
        byte Error_eeprom = 1;
        break;
      }
    }
    }
  }
}

//Check routine taken from
//http://web.mit.edu/6.115/www/miscfiles/amulet/amulet-help/xmodem.htm
int calcrc(char *ptr, int count)
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}
