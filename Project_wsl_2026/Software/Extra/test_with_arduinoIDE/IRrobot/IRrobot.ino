#include <IRremote.h>
#include <RPomniDirect.h>
#include <RPlib.h>

RPomniDirect omniBot(1.0f, 1.0f, 1.0f, 20.0f); 

int RECV_PIN = A5;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup(){
  irrecv.enableIRIn();
}

int x=0,y=0;
int s=80;
int r=0;

void loop(){
  if (irrecv.decode(&results)){
    irrecv.resume();
  }

  switch (results.value){
  case 0xFF30CF:
    r=0;
    x=-s;
    y=s;
    break;
  case 0xFF18E7:
    r=0;
    y=s;
    break;
  case 0xFF7A85:
    r=0;
    x=s;
    y=s;
    break;
  case 0xFF10EF:
    r=0;
    x=-s;
    break;
  case 0xFF38C7:
    r=0;
    x=0;
    y=0;
    break;
  case 0xFF5AA5:
    r=0;
    x=s;
    break;
  case 0xFF42BD:
    r=0;
    x=-s;
    y=-s;
    break;
  case 0xFF4AB5:
    r=0;
    y=-s;
    break;
  case 0xFF52AD:
    r=0;
    x=s;
    y=-s;
    break;

  case 0xFF9867:
    r=-s;
    x=0;
    y=0;
    break;
  case 0xFFB04F:
    r=s;
    x=0;
    y=0;
    break;

  default:
    break;
  }

  omniBot.move(-x,y,r);
}




