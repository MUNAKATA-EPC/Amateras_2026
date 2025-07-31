#include <Wire.h>

void setup() {
    Wire.begin(0x54);  // Pixy V1のI2Cアドレス (0x54)
    Wire.onRequest(requestEvent);
    Serial.begin(115200);
    Serial.println("Arduino Nano (Pixy Emulator) Ready");
}

void loop() {
    delay(2000);  // 2秒ごとに送信
    Serial.println("Sending data...");
}

void requestEvent() {
    // データの送信（Pixy V1の形式に合わせる）
    // 0xAA55（同期ワード）+ シグネチャ番号 + 座標 (X, Y) + 幅, 高さ + チェックサム
    uint8_t pixyData[] = {
        0xAA, 0x55,       // 同期ワード
        0x01,             // シグネチャ番号
        0x02, 0x03,       // X, Y座標 (例: X=0x0203, Y=0x0405)
        0x04, 0x05,       // 幅, 高さ
        0x06              // チェックサム（仮）
    };

    // チェックサムの計算（ここでは単純な例として足し算を使っています）
    uint8_t checksum = 0;
    for (int i = 0; i < sizeof(pixyData) - 1; i++) {
        checksum += pixyData[i];
    }
    pixyData[sizeof(pixyData) - 1] = checksum;  // チェックサムをデータに追加

    // データ送信
    Wire.write(pixyData, sizeof(pixyData));
    Serial.println("Data sent successfully!");

    // 送信データをシリアルモニタに表示
    Serial.print("Sending Data: ");
    for (int i = 0; i < sizeof(pixyData); i++) {
        Serial.print("0x");
        Serial.print(pixyData[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}


