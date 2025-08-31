#include <Arduino.h>
#include "MyLibrary.h"

/*モータ制御について*/
#define MOVEMOTORMAX 80 // 移動で使う最大出力
#define GYROMOTORMAX 18 // pid制御で使う最大出力

// rfモータのは1chに接続
// lfモータのは2chに接続
// rbモータのは3chに接続
// lbモータのは4chに接続
void Motor4pidControl(double speed_move, int edge_move, double speed_gyro, int edge_gyro, int goaledge_gyro); // ロボットを指定した角度に移動させる関数
void Motor4pidTurn(double speed_gyro, int edge_gyro, int goaledge_gyro);                                      // ロボットをその場でpidだけさせる関数
void Motor4Stop();                                                                                            // ロボットを停止させる関数

/*LINEについて*/
#define LINEESCAPE_TIME 100 // ラインに触れてどのくらいラインから離れる動きをするか

long now_LINEtime = 0;  // 今の時間を格納する変数
long old_LINEtime = 0;  // 昔の時間を格納する変数
int Final_LINEedge = 0; // 最終的なラインの角度を格納する変数

/*回り込みについて*/
#define IRFRONT_EDGE_MIN 13      // 前にボールがあるとする角度min
#define IRFRONT_EDGE_MAX 13      // 前にボールがあるとする角度max
#define IRFOLLOW_EDGE_PLUS 4     // ボール方向に行かせる角度+a
#define IRNEARFRONT_EDGE_PLUS 30 // 前付近にボールがあるとする角度+a

#define IRFRONT_DIST 110 // 前のどの方向に進もうとするか

bool CannotAround_error = false; // 接線算出に失敗したときに1に9なる
int Cal_AroundIRedge;            // 計算しだした回り込む角度

//  edge : IRボールの角度, dist : IRボールとの距離, R : ボールの半径（大きめに指定しよう）
void IRaroundmove_Calculate(int edge, double dist, int *Cal_around_edge);
// edge : IRボールの角度, dist : IRボールとの距離, q : 前のどの方向に進もうとするか
void IRfrontmove_Calculate(int edge, double dist, double q, int *Cal_around_edge); // 前付近にある時の角度を計算する関数

/*OLEDについて*/
int select_num = 0; // 何を選ぶかを格納する変数

void setup()
{
    /*いろいろ初期設定*/
    BNO055begin();
    DSR1202begin();
    IRXIAObegin();
    LINEXIAObegin();
    SSD1306begin();
    SSD1306clear();
    SWbegin();
    PIDv2begin();
    Kickerbegin();

    /*Camの設定*/
    OpenMVbegin();

    /*PCとのシリアル通信*/
    Serial.begin(9600);

    /*LINEの停止時間*/
    SSD1306clear();
    SSD1306write(1, 0, 0, "Wait a moment", true);
    while (millis() <= LINEESCAPE_TIME)
    {
        // タイマーがLINEESCAPE_TIME超えるまで待つ。(誤作動を防ぐ)
    }
}

/*メインループ*/

void loop()
{
    /*データを取得*/
    LINEXIAOget(); // LINEXIAOからデータを取得
    IRXIAOget();   // IRXIAOからデータを取得
    BNO055get();   // BNO055からデータを取得
    TactSWget();   // SWのデータを取得
    OpenMVget();   // Camのデータ取得

    /*SWの設定*/
    if (LeftSW)
    {
        select_num++;
        select_num = (select_num + 2) % 2; // 0~1に収める
    }
    else if (RightSW)
    {
        select_num--;
        select_num = (select_num + 2) % 2; // 0~1に収める
    }

    /*キッカーを動かす*/
    KickerPlay(EnterSW); // 決定ボタンが押されたら蹴る

    /*モータを動かす*/
    now_LINEtime = millis() - old_LINEtime; // 時間を更新
    if (select_num == 0)                    // トグルスイッチがoffならば
    {
        Motor4Stop(); // ストップする
    }
    else if (LINEexist || now_LINEtime <= LINEESCAPE_TIME) // LINEがあるまたはタイマーが指定時間内ならば
    {
        if (LINEedge != -1)
        {
            Final_LINEedge = LINEedge; // 円形のラインが反応
        }
        else if (LINEleft)
        {
            Final_LINEedge = 90; // 左のラインが反応
        }
        else if (LINEright)
        {
            Final_LINEedge = 270; // 右のラインが反応
        }
        else if (LINEback)
        {
            Final_LINEedge = 180; // 後ろのラインが反応
        }
        Motor4pidControl(MOVEMOTORMAX, Final_LINEedge + 180, GYROMOTORMAX, BNO055deg, 0); // LINEとは逆方向に移動させる
        if (LINEexist)                                                                    // LINEがあるときは更新
        {
            old_LINEtime = millis(); // 昔の時間を更新
        }
    }
    else if (IRexist) // IRボールがあるならば
    {
        Serial.print(digitalRead(6));

        int MotorPower = MOVEMOTORMAX;
        if (IRedge <= IRFRONT_EDGE_MIN || IRedge >= 360 - IRFRONT_EDGE_MAX) // 前にボールがある
        {
            KickerPlay((IRdist <= 145));

            Serial.print("Front : ");
            Cal_AroundIRedge = 0; // 0度を代入

            // MotorPower = 0;
        }
        else if (IRedge <= IRFRONT_EDGE_MIN + IRFOLLOW_EDGE_PLUS || IRedge >= 360 - (IRFRONT_EDGE_MAX + IRFOLLOW_EDGE_PLUS)) // かなり前にボールがある
        {
            Serial.print("Follow : ");
            if (IRdist <= 170)
            {
                if (IRedge < 180)
                {
                    MotorPower /= 2;
                    Cal_AroundIRedge = 90;
                }
                else
                {
                    MotorPower /= 2;
                    Cal_AroundIRedge = 270;
                }
            }
            else
            {
                Cal_AroundIRedge = IRedge; // ボールの方向を代入
            }
        }
        else if (IRedge <= IRFRONT_EDGE_MIN + IRFOLLOW_EDGE_PLUS + IRNEARFRONT_EDGE_PLUS || IRedge >= 360 - (IRFRONT_EDGE_MAX + IRFOLLOW_EDGE_PLUS + IRNEARFRONT_EDGE_PLUS)) // 前付近にボールがある
        {
            Serial.print("NearFront : ");
            IRfrontmove_Calculate(IRedge, IRdist, IRFRONT_DIST, &Cal_AroundIRedge); // 回り込む角度を計算

            // MotorPower /= 2;
        }
        else // それ以外
        {
            Serial.print("Around : ");
            IRaroundmove_Calculate(IRedge, IRdist, &Cal_AroundIRedge); // 回り込む角度を計算

            // MotorPower = 0;
        }
        Motor4pidControl(MotorPower, Cal_AroundIRedge, GYROMOTORMAX, BNO055deg, 0); // LINEとは逆方向に移動させる
        Serial.println(Cal_AroundIRedge);                                           // 角度をプリント
    }
    else // IRボールがないならば
    {
        Motor4pidTurn(GYROMOTORMAX, BNO055deg, 0); // pidだけする
        Serial.println("Noball : -1");
    }

    /*LCDに表示*/
    SSD1306clear();
    SSD1306Bitmap(0, 0, LINEIRSWChecker_bitmap, 128, 64, false); // LINEIRSW Checkerのビットマップを表示
    SSD1306write(1, 68, 44, String(LINEedge), false);            // LINEの情報を表示
    SSD1306write(1, 68, 53, String(LINEleft) + String(LINEright) + String(LINEback) + "0", false);
    SSD1306line(111, 48, 111 - sin(radians(LINEedge)) * 15, 48 - cos(radians(LINEedge)) * 15, false);
    SSD1306write(1, 4, 44, String(IRedge), false); // IRボールの情報を表示
    SSD1306write(1, 4, 53, String(IRdist), false);
    SSD1306line(47, 47, 47 - sin(radians(IRedge)) * 15, 48 - cos(radians(IRedge)) * 15, false);
    SSD1306write(1, 79, 21, String(BNO055deg), false); // BNO055の情報を表示
    SSD1306line(116, 20, 116 - sin(radians(BNO055deg)) * 10, 20 - cos(radians(BNO055deg)) * 10, false);
    SSD1306write(1, 48, 21, String(analogRead(6)), false);                                                                                                              // 可変抵抗の値を表示
    SSD1306write(1, 4, 21, String(LeftSW_true) + String(RightSW_true) + String(EnterSW_true) + String(BNO055resetSW) + String(CamToggle) + String(MotorToggle), false); // SWの情報を表示
    SSD1306show();

    delay(50); // 50ms待機する
}

/*関数*/

/*　回り込むための角度を計算する　*/
// 角度の算出方法について : https://www.desmos.com/calculator/c0nzjtscrs?lang=ja
// ロボットは原点にして障害物（円）との接線をIRedge[rad] ± asin(ballR / IRdist)で求める方式
void IRaroundmove_Calculate(int edge, double dist, int *Cal_around_edge)
{
    if (dist <= 170)
    {
        if (edge > 180) // 左のボールがある
        {
            *Cal_around_edge = (edge - 52 + 360) % 360;
        }
        else // 右にボールがある
        {
            *Cal_around_edge = (edge + 52 + 360) % 360;
        }
    }
    else
    {
        *Cal_around_edge = edge;
    }
}

/*　前付近時の回り込むための角度を計算する　*/
void IRfrontmove_Calculate(int edge, double dist, double q, int *Cal_around_edge)
{
    edge = (edge + 360 - 270) % 360; // x軸正の方向を0度とする

    double Delta_x = dist * cos(radians(edge));     // x方向の変化量を求める
    double Delta_y = dist * sin(radians(edge)) - q; // y方向の変化量を求める

    *Cal_around_edge = int(degrees(atan2(Delta_y, Delta_x))); // 角度を求める
    *Cal_around_edge = (*Cal_around_edge + 360) % 360;        // 一応丸める
    *Cal_around_edge = (*Cal_around_edge + 360 - 90) % 360;   // 前を0度とする
}

/*　ロボットを指定した角度に移動させる関数　*/
void Motor4pidControl(double speed_move, int edge_move, double speed_gyro, int edge_gyro, int goaledge_gyro)
{
    edge_move = (edge_move + 360) % 360; // 一応丸め込む
    edge_gyro = (edge_gyro + 360) % 360; // 一応丸め込む

    double PIDpower = PIDv2(edge_gyro, goaledge_gyro, speed_gyro); // PIDの出力を格納

    double ControlmotorPower[4] = {0.0, 0.0, 0.0, 0.0};                           // モータの出力値を格納する配列 {rf, lf, rb, lb}
    ControlmotorPower[0] = -sin(radians(edge_move + 45)) * speed_move + PIDpower; // rfモータの出力値
    ControlmotorPower[1] = cos(radians(edge_move + 45)) * speed_move + PIDpower;  // lfモータ2の出力値
    ControlmotorPower[2] = -cos(radians(edge_move + 45)) * speed_move + PIDpower; // rbモータ3の出力値
    ControlmotorPower[3] = sin(radians(edge_move + 45)) * speed_move + PIDpower;  // lbモータ4の出力値

    DSR1202move(ControlmotorPower[0],
                ControlmotorPower[1],
                ControlmotorPower[2],
                ControlmotorPower[3]); // モータを制御する
}

/*　ロボットをその場でpidだけさせる関数　*/
void Motor4pidTurn(double speed_gyro, int edge_gyro, int goaledge_gyro)
{
    edge_gyro = (edge_gyro + 360) % 360; // 一応丸め込む

    double PIDpower = PIDv2(edge_gyro, goaledge_gyro, speed_gyro); // PIDの出力を格納

    double ControlmotorPower[4] = {0.0, 0.0, 0.0, 0.0}; // モータの出力値を格納する配列 {rf, lf, rb, lb}
    ControlmotorPower[0] = PIDpower;                    // rfモータの出力値
    ControlmotorPower[1] = PIDpower;                    // lfモータ2の出力値
    ControlmotorPower[2] = PIDpower;                    // rbモータ3の出力値
    ControlmotorPower[3] = PIDpower;                    // lbモータ4の出力値

    DSR1202move(ControlmotorPower[0],
                ControlmotorPower[1],
                ControlmotorPower[2],
                ControlmotorPower[3]); // モータを制御する
}

/*　ロボットを停止させる関数　*/
void Motor4Stop()
{
    DSR1202move(0, 0, 0, 0); // モータを停止する
}