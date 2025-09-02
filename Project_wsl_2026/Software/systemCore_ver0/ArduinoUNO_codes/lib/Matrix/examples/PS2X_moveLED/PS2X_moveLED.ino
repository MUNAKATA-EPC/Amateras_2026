//ゲームパッドでLEDを操縦してみよう
//
//
#include <PS2X_lib.h>  
#include <Sprite.h>  
#include <Matrix.h>

PS2X ps2x; 
Matrix myMatrix = Matrix(11, 13, 1);

int x=0,y=0;

void setup(){
  ps2x.config_gamepad(13,11,10,12, true, true);  

}

void loop(){

  myMatrix.write(constrain(x,0,7), constrain(y,0,7), HIGH);
  delay(50);
 // myMatrix.write(constrain(x,0,7), constrain(y,0,7), LOW);

  ps2x.read_gamepad();  

  if(ps2x.Button(PSB_PAD_UP))y++;    
  if(ps2x.Button(PSB_PAD_RIGHT))x++;   
  if(ps2x.Button(PSB_PAD_LEFT))x--;    
  if(ps2x.Button(PSB_PAD_DOWN))y--;
  
}

