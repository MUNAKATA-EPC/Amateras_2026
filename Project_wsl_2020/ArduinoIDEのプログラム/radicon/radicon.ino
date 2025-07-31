//D6 -> left_button , D7 -> right_button
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#define BNO055_SAMPLERATE_DELAY_MS 10
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);  // BNO用のI2Cを指定
int raw_deg;
int reset_deg;
int deg;
void bno_init();
int bno_process();

#include <PS2X_lib.h>
PS2X ps2x;
float rx, ry, r_hue = 0.0, lx, ly, l_hue = 0.0, power, add = 0.0;

void setup() {
  Wire.begin();
  pinMode(6, INPUT);
  ps2x.config_gamepad(2, 3, 5, 4, true, true);  //コントローラーを使用するおまじない
  Serial.begin(19200);
  bno_init();
}

void loop() {
  int gyro = bno_process();

  ps2x.read_gamepad();

  lx = ps2x.Analog(PSS_LX) - 128;
  ly = -ps2x.Analog(PSS_LY) + 128;
  l_hue = atan2(ly, lx);
  if (l_hue < 0) l_hue = 2 * PI + l_hue;
  l_hue += 0.25 * PI;
  if (l_hue > 2 * PI) l_hue = l_hue - 2 * PI;
  if (lx > -10 && lx < 10 && ly > -10 && ly < 10) l_hue = -1;

  rx = ps2x.Analog(PSS_RX) - 128;
  ry = -ps2x.Analog(PSS_RY) + 128;
  r_hue = atan2(ry, rx) * 180 / PI;
  if (r_hue < 0) r_hue = 360 + r_hue;
  r_hue -= 90;
  if (r_hue < 0) r_hue = 360 + r_hue;
  r_hue = 360 - r_hue;
  if (rx > -10 && rx < 10 && ry > -10 && ry < 10) r_hue = -1;

  if (gyro > 7 && gyro < 180) {
    add = -10.0;
  } else if (gyro < 353 && gyro >= 180) {
    add = 10.0;
  } else {
    add = 0;
  }
  /*
  Serial.print(r_hue);
  Serial.print("  ");
  Serial.println(gyro);
 */
  power = 1.0;

  if (l_hue == -1) {
    if (add == 0) {
      control_brake('1');
      control_brake('2');
      control_brake('3');
      control_brake('4');
    } else {
      control_motor('1', -add * power);
      control_motor('2', add * power);
      control_motor('3', -add * power);
      control_motor('4', add * power);
    }
  } else {
    control_motor('1', int(-77 * power * sin(l_hue) - add * power));
    control_motor('2', int(77 * power * cos(l_hue) + add * power));
    control_motor('3', int(77 * power * cos(l_hue)) - add * power);
    control_motor('4', int(-77 * power * sin(l_hue)) + add * power);
  }

  delay(10);
}


//関数一覧


void bno_init() {
  delay(1000);

  if (!bno.begin()) {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }

  bno.setExtCrystalUse(true);
}

int bno_process() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  //raw_deg = euler.x();
  deg = (int)(euler.x() - reset_deg + 360) % 360;

  if (r_hue != -1) {
    reset_deg = r_hue;
  } else {
    reset_deg = 0;
  }

  return deg;
}


// I2Cアドレス変更コマンド
void change_i2c_address(char n) {
  Serial.print("A:");
  Serial.print(n);
  Serial.print("\r\n");
}

// シリアル通信速度コマンド
void change_serial_speed(char n) {
  Serial.print("S:");
  Serial.print(n);
  Serial.print("\r\n");
}

// モータ制御コマンド
void control_motor(char ch, int speed) {
  char direction;
  if (speed < 0) {
    direction = 'R';
    speed = -speed;
  } else {
    direction = 'F';
  }
  Serial.print(ch);
  Serial.print(direction);
  if (speed < 10) {
    Serial.print("00");
  } else if (speed < 100) {
    Serial.print("0");
  }
  Serial.print(speed);
  Serial.print("\r\n");
}

//ブレーキ
void control_brake(char ch) {
  Serial.print(ch);
  Serial.print('R');
  Serial.print("000");
  Serial.print("\r\n");
}