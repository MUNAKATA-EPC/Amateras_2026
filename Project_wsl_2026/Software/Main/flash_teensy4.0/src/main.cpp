#include <Arduino.h>
#include <SPIMemory.h>

const uint8_t CS_PIN = 36;
SPIFlash flash(CS_PIN);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  if (flash.begin())
  {
    Serial.println("flashmemory is detected");
  }
  else
  {
    Serial.println("flashmemory is not detected");
    while (true)
      ;
  }

  Serial.print("flashmemory ID:");
  Serial.println(flash.getUniqueID());
  Serial.print("capacity:");
  Serial.print(flash.getCapacity() / 1024);
  Serial.println("[KB]");

  uint32_t addr = 0x000000;
  String write_data = "this is the first data";
  String read_data;

  Serial.println("erasing sector...");
  if (flash.eraseSector(addr))
  {
    Serial.println("erase successful");
  }

  Serial.println("writing data...");
  if (flash.writeStr(addr, write_data))
  {
    Serial.println("write successful");
  }

  Serial.println("reading data...");
  if (flash.readStr(addr, read_data))
  {
    Serial.print("data read:");
    Serial.println(read_data);
  }
}

void loop()
{
}