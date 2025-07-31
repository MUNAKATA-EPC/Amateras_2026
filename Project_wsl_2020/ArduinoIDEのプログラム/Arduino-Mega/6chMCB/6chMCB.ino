// 通信速度：6chMCBの出荷時初期値に合わせる
#define MCB_BAUDRATE 19200

void setup() {
  Serial.begin(MCB_BAUDRATE);  // 6chMCBとシリアル通信開始
}

void loop() {
  Move_motor(100, 50, 0, 100, 0, 0);
}

// ============================
// モーター制御関数
// ============================
// 各値は 0〜100 の出力（正転）
// 例：Something(100, 50, 0, 100, 0, 0);
void Move_motor(int m1, int m2, int m3, int m4, int m5, int m6) {
  String cmd = "";
  cmd += makeMotorCmd(1, m1);
  cmd += makeMotorCmd(2, m2);
  cmd += makeMotorCmd(3, m3);
  cmd += makeMotorCmd(4, m4);
  cmd += makeMotorCmd(5, m5);
  cmd += makeMotorCmd(6, m6);
  sendMotorCommand(cmd);
}

// ============================
// モーター個別コマンド生成（正転）
// ============================
String makeMotorCmd(int ch, int power) {
  power = constrain(power, 0, 100);  // 安全：0〜100に制限
  char buf[10];
  sprintf(buf, "%dF%03d", ch, power);  // 例：1F100（1ch 正転 100%）
  return String(buf);
}

// ============================
// シリアル送信関数
// ============================
void sendMotorCommand(String cmd) {
  Serial.print(cmd);
  Serial.print("\r\n");  // CRLF終端（0x0D, 0x0A）
}

