#include <Arduino.h>
#include "MyLibrary.h"

/*モータ制御について*/
#define MOVEMOTORMAX 80 // 移動で使う最大出力
#define GYROMOTORMAX 20 // pid制御で使う最大出力

// rfモータのは1chに接続
// lfモータのは2chに接続
// rbモータのは3chに接続
// lbモータのは4chに接続
void Motor4pidControl(double speed_move, int edge_move, double speed_gyro, int edge_gyro, int goaledge_gyro); // ロボットを指定した角度に移動させる関数
void Motor4pidTurn(double speed_gyro, int edge_gyro, int goaledge_gyro);                                      // ロボットをその場でpidだけさせる関数
void Motor4Stop();                                                                                            // ロボットを停止させる関数

/*回り込みについて*/
#define ROBOT_R 5          // ロボットの半径を指定
#define ALPHA_R 5          // ロボットの半径からどのくらい足した位置で回り込むか指定
#define IRFRONT_EDGE 15    // 前にボールがあるとする角度
#define IRNEARFRONT_EDGE 0 // 前付近にボールがあるとする角度

double A, B;                // 傾きと切片　(右にボールがあるときの関数)
double TouchPoint_right[2]; // 接点座標1
double C, D;                // 傾きと切片　(左にボールがあるときの関数)
double TouchPoint_left[2];  // 接点座標1

// 処理を軽くするために、傾きと切片そして座標はどちらか一方しか計算しない
//  robotR: ロボットの半径, plusR: robotR + plusR = 回り込みの半径 , p: IRボールのX座標 , q: IRボールのY座標
int IRmove_Calculate(int edge, double robotR, double plusR, double p, double q); // 回り込みの角度を算出する関数

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

    /*PCとのシリアル通信*/
    Serial.begin(9600);
}

/*メインループ*/

void loop()
{
    /*データを取得*/
    LINEXIAOget(); // LINEXIAOからデータを取得
    IRXIAOget();   // IRXIAOからデータを取得
    BNO055get();   // BNO055からデータを取得
    TactSWget();   // SWのデータを取得

    /*モータを動かす*/
    if (IRexist && !MotorToggle) // IRボールがあるならば
    {
        if (IRedge <= 1 || IRedge >= 359) // 前にボールがある
        {
            Motor4pidControl(MOVEMOTORMAX, 0, GYROMOTORMAX, BNO055deg, 0); // 前進する

            Serial.println("IRedge : " + String(IRedge) + " IRdist : " + String(IRdist) + " p : " + String(IRball_p) + " q : " + String(IRball_q) + " -front> " + String(0)); // シリアルプリント
        }
        else if (IRedge <= 1 || IRedge >= 359) // 前付近にボールがある
        {
            Motor4pidControl(MOVEMOTORMAX, IRedge, GYROMOTORMAX, BNO055deg, 0); // ボールのほうへ行く

            Serial.println("IRedge : " + String(IRedge) + " IRdist : " + String(IRdist) + " p : " + String(IRball_p) + " q : " + String(IRball_q) + " -near> " + String(IRedge)); // シリアルプリント
        }
        else // それ以外
        {
            int IRaround_edge = IRmove_Calculate(IRedge, ROBOT_R, ALPHA_R, IRball_p, IRball_q); // 回り込む角度を計算

            Motor4pidControl(MOVEMOTORMAX, IRaround_edge, GYROMOTORMAX, BNO055deg, 0); // ボールのほうへ行く

            Serial.println("IRedge : " + String(IRedge) + " IRdist : " + String(IRdist) + " p : " + String(IRball_p) + " q : " + String(IRball_q) + " -around> " + String(IRaround_edge)); // シリアルプリント
        }
    }
    else // IRボールがないならば
    {
        Motor4pidTurn(GYROMOTORMAX, BNO055deg, 0); // pidだけする

        Serial.println("IRedge : " + String(IRedge) + " IRdist : " + String(IRdist) + " p : " + String(IRball_p) + " q : " + String(IRball_q) + " -Nothing> " + String(-1)); // シリアルプリント
    }

    /*LCDに表示*/
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

    SSD1306clear();
    if (select_num == 0)
    {
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
    }
    else if (select_num == 1)
    {
        SSD1306write(1, 0, 0, "true:" + String(IRedge), true);                                                                        // IRボールの情報を表示
        SSD1306Circle(64, 32, ROBOT_R, false);                                                                                        // ロボットの円を描く
        SSD1306Circle(64, 32, ROBOT_R + ALPHA_R, false);                                                                              // 回り込みの円を描く
        SSD1306line(64, 32, 64 - sin(radians(IRedge)) * (ROBOT_R + ALPHA_R), 32 - cos(radians(IRedge)) * (ROBOT_R + ALPHA_R), false); // 回り込む角度を描く
        SSD1306Circle(IRball_p + 64, -IRball_q + 32, 1, false);                                                                       // IRボールを描く
        SSD1306functionline(64, 32, A, B, false);                                                                                     // 接線を描く
        SSD1306functionline(64, 32, C, D, false);                                                                                     // 接線を描く
    }
    SSD1306show();

    delay(50); // 50ms待機する
}

/*関数*/

/*　回り込むための角度を計算する　*/
// 角度の算出方法について : https://www.desmos.com/calculator/wuttwtmaiw?lang=ja
int IRmove_Calculate(int edge, double robotR, double plusR, double p, double q)
{
    edge = (edge + 360) % 360; // 一応丸め込む

    int Cal_edge; // 計算した角度を格納する変数

    double R = robotR + plusR; // 回り込みの半径を求める
    double p2 = p * p;
    double q2 = q * q;
    double R2 = R * R;
    double denom = R2 - p2;

    double Q = p2 + q2 - R2; // 判別式

    if (Q >= 0 && fabs(denom) > 1e-6) // 接線が存在し、かつ除算エラー防止
    {
        double sqrtQ = sqrt(Q);

        // 傾きの算出を精度の高い順序で
        A = (-p * q + sqrtQ) / denom;
        B = q - A * p;
        C = (-p * q - sqrtQ) / denom;
        D = q - C * p;

        // 接点の座標を求める
        double A2 = A * A;
        double C2 = C * C;

        double denomA = A2 + 1.0;
        double denomC = C2 + 1.0;

        // 接点を計算（より安定化）
        TouchPoint_right[0] = -(A * B) / denomA;
        TouchPoint_right[1] = B / denomA;

        TouchPoint_left[0] = -(C * D) / denomC;
        TouchPoint_left[1] = D / denomC;

        // ボールが右にある時
        if (edge > 180)
        {
            double dx = p - TouchPoint_right[0];
            double dy = q - TouchPoint_right[1];

            Cal_edge = int(degrees(atan2(dy, dx)) + 360) % 360;
            Cal_edge = (Cal_edge + 270) % 360;
        }
        else // ボールが左にある時
        {
            double dx = p - TouchPoint_left[0];
            double dy = q - TouchPoint_left[1];

            Cal_edge = int(degrees(atan2(dy, dx)) + 360) % 360;
            Cal_edge = (Cal_edge + 270) % 360;
        }
    }
    else // 接線が存在しない時
    {
        Cal_edge = (edge + 180) % 360; // ボールに近づきすぎなので離れるようにする
        Serial.print("error  ");
    }

    return Cal_edge; // 計算した角度を返す
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