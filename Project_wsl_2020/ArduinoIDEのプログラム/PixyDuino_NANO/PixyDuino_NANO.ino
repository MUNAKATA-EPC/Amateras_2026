#include <Wire.h>

#define PIXY_I2C_ADDR 0x54  // PixyのI2Cアドレス

// Pixyから送信されるデータ形式に基づいたダミーデータ（最初の1つのオブジェクトの情報）
uint8_t pixyData[] = {
  0xAA, 0x55,  // 同期ワード
  0x00, 0x00,  // チェックサム（後で計算）
  0x01,        // シグネチャ番号
  0x00, 0x50,  // X座標（80）
  0x00, 0x32,  // Y座標（50）
  0x00, 0x28,  // 幅（40）
  0x00, 0x14   // 高さ（20）
};

void setup() {
  Wire.begin(PIXY_I2C_ADDR);  // I2Cスレーブとして開始
  Wire.onRequest(sendData);   // マスターからのリクエスト時にデータを送る
}

void loop() {
  // 何もしない（I2Cのリクエストを待つ）
}

// I2Cリクエスト時に呼ばれる関数
void sendData() {
  // チェックサムを計算
  uint16_t checksum = calculateChecksum(pixyData + 4, 8);  // シグネチャ番号以降を計算
  pixyData[2] = (checksum >> 8) & 0xFF;  // 上位バイト
  pixyData[3] = checksum & 0xFF;         // 下位バイト

  // データ送信
  Wire.write(pixyData, sizeof(pixyData));
}

// チェックサム計算関数
uint16_t calculateChecksum(uint8_t* data, size_t len) {
  uint16_t checksum = 0;
  for (size_t i = 0; i < len; i++) {
    checksum += data[i];
  }
  return checksum;
}




