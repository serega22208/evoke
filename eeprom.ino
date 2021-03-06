/**Fu
**/
void read_large_eeprom(int address, unsigned long first, unsigned long bytes) {
    byte value;
    unsigned long add;
    byte Error_eeprom=0; //show if somewhere error happened
    for(add = first; add < bytes; add++) {
        if (Error_eeprom!=0){
            Serial.write("ERRoR");
            break;
    }

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
    if(Wire.available()) {
        value = Wire.read();
        //The packet format is as follows:
        //@@Header(0x06,0x85)|SizeofPayload|Payload|CRC16(XModem)@@
        byte eeprom_packet[5];
        eeprom_packet[0]=6;
        eeprom_packet[1]=133;
        eeprom_packet[2]=8;
        eeprom_packet[3]=value;
        eeprom_packet[4]=calcrc((char*)eeprom_packet, 4);
        Serial.print(eeprom_packet, 5);
    }


    for(i=1;i<=5;i++){
    //checking that data transfed and crc16 is correct,
    //before transfer next byte
    //5 times try to transfer and wait datacheck good answer('OK\n')
    //or ter
    //if no - mark error flag
        Serial.write(eeprom_packet, 5);
        delay(5);
        if  (Serial.findUntil("OK","\n")) {
            //Serial.findUntil has 1 sec timeout, after it done
            break;
            }
        if (i>=5){
            byte Error_eeprom=1;
            break;
        }
    }
}
