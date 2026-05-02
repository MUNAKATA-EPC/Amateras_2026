#include <Arduino.h>
#include <SPIMemory.h>

const uint8_t CS_PIN = 7;
SPIFlash flash(CS_PIN);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  if (flash.begin())
  {
    Serial.println("Flash Memory System Ready");
  }
  else
  {
    Serial.println("Flash memory is not detected!");
    while (true)
      ;
  }

  Serial.print("Flash memory ID: ");
  uint64_t id = flash.getUniqueID();
  Serial.print((uint32_t)(id >> 32), HEX);
  Serial.println((uint32_t)(id & 0xFFFFFFFF), HEX);

  Serial.println("Commands: 'read' or 'send'");
}

void loop()
{
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "read")
    {
      Serial.println("\n[READ MODE]");

      Serial.print("Enter Address (decimal): ");
      while (!Serial.available())
        ;
      uint32_t addr = Serial.parseInt();
      Serial.println(addr);

      Serial.print("Enter Length (bytes): ");
      while (!Serial.available())
        ;
      int len = Serial.parseInt();
      Serial.println(len);

      String read_data;
      if (flash.readStr(addr, read_data))
      {
        Serial.print("Data at ");
        Serial.print(addr);
        Serial.print(": ");
        Serial.println(read_data);
      }
      else
      {
        Serial.println("Read failed.");
      }
      Serial.println("\nNext command ('read' or 'send')?");
    }

    else if (command == "send")
    {
      Serial.println("\n[WRITE MODE]");

      Serial.print("Enter Address (decimal): ");
      while (!Serial.available())
        ;
      uint32_t addr = Serial.parseInt();
      Serial.println(addr);

      Serial.println("Erasing sector...");
      flash.eraseSector(addr);

      Serial.print("Enter Data to Write: ");
      while (Serial.available() == 0)
        ;
      delay(100);
      String write_data = Serial.readStringUntil('\n');
      write_data.trim();
      Serial.println(write_data);

      if (flash.writeStr(addr, write_data))
      {
        Serial.println("Write successful!");
      }
      else
      {
        Serial.println("Write failed.");
      }
      Serial.println("\nNext command ('read' or 'send')?");
    }
  }
}