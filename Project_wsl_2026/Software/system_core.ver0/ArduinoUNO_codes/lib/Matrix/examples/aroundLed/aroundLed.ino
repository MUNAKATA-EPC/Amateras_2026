//ドットLEDが動く（全画面走査）

#include <Sprite.h>  // Sprite before Matrix
#include <Matrix.h>

const int numLeds = 1;
const int dtime=50;
//                   DIN, CLK, LOAD, #chips
Matrix myMatrix = Matrix(11, 13, 1, numLeds);

void setup() {
  myMatrix.clear();
}

void loop() {
  byte x=0, y=0;

  // light one LED at a time, scanning left to right
  // and top to bottom... useful for testing the matrix
  y=0;
    for (x=0; x<8; x++) {
      myMatrix.write(x, y, HIGH);
      delay(dtime);
      myMatrix.write(x, y, LOW);
    }
 
 x=7;
    for (y=1; y<8; y++) {
      myMatrix.write(x, y, HIGH);
      delay(dtime);
      myMatrix.write(x, y, LOW);
    }
    
  y=7;
    for (x=6; x>0; x--) {
      myMatrix.write(x, y, HIGH);
      delay(dtime);
      myMatrix.write(x, y, LOW);
    }

 x=0;
   for (y=7; y>0; y--) {
      myMatrix.write(x, y, HIGH);
      delay(dtime);
      myMatrix.write(x, y, LOW);
    }

}
