#include <Sprite.h>  // Sprite before Matrix
#include <Matrix.h>

//                   DIN, CLK, LOAD, #chips
Matrix myMatrix = Matrix(11, 13, 1);

void setup() {
  myMatrix.clear();
}

void loop() {
  byte x, y;

  // light one LED at a time, scanning left to right
  // and top to bottom... useful for testing the matrix
  for (y=0; y<8; y++) {
    for (x=0; x<8; x++) {
      myMatrix.write(x, y, HIGH);
      delay(100);
      myMatrix.write(x, y, LOW);
    }
  }
}
