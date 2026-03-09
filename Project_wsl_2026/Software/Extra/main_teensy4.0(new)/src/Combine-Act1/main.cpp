#include <Arduino.h>
#include "MyLibrary.h"

#define MOVEMOTORPOWER 80 // 移動で使う最大出力
#define GYROMOTORPOWER 20 // pid制御で使う最大出力

/*IRについて*/
// 距離について
#define IRDISTFARVAL 500 // IRdistがどのくらいの時に遠いと認識させるか
// ボールが近い時
#define MAXAROUNDEDGE 90 // 最大回り込み角度（ボールがより近い）
#define MINAROUNDEDGE 45 // 最小回り込み角度（ボールがより遠い）
// ボールが前付近にある時
#define MAXAROUNDEDGE_FRONT 30 // 最大回り込み角度（ボールがより近い）
#define MINAROUNDEDGE_FRONT 0  // 最小回り込み角度（ボールがより遠い）
// 寸法・角度について
#define ROBOT_R 3          // ロボットの半径を定義
#define LINE_R 2           // 円形ライン基盤の半径を定義
#define IRBALL_R 1         // IRボールの半径を定義
#define IRFRONT_VAL 18     // IRボールの角度が±どのくらいの時にボールが前にあるとするか定義
#define IRNEARFRONT_VAL 28 // IRボールの角度が±どのくらいの時にボールが前付近にあるとするか定義

double IRballmoveAroundedge = 0.0;                                                   // 回り込むための角度を格納する変数
double IRball_Calculate(double IRedge_move, double R, double r, double p, double q); // 回り込むための角度を計算する関数

/*LINEについて*/
// タイマー
#define LINETIME 60 // 踏んだら何ms離れる動きをするか

int old_LINEedge = -1;           // 昔のラインの方向を保存する変数
long now_LINEtime, old_LINEtime; // 今と昔のラインで使うタイマー

/*モータ制御について*/
// rfモータのは1chに接続
// lfモータのは2chに接続
// rbモータのは3chに接続
// lbモータのは4chに接続
void Motor4pidControl(double speed_move, double edge_move, double speed_gyro, double edge_gyro, double goaledge_gyro); // ロボットを指定した角度に移動させる関数
void Motor4Stop();                                                                                                     // ロボットを停止させる関数

void setup()
{
    BNO055begin();
    DSR1202begin();
    IRXIAObegin();
    LINEXIAObegin();
    SSD1306begin();
    SSD1306clear();
    SWbegin();

    /*PIDの設定*/
    PIDv2begin();

    /*タイマーの初期化*/
    old_LINEtime = millis();
}

/*メインループ*/

void loop()
{
    /*LINEXIAOからデータを取得*/
    LINEXIAOget();

    /*IRXIAOからデータを取得*/
    IRXIAOget();

    /*BNO055からデータを取得*/
    BNO055get();

    /*SWのデータを取得*/
    TactSWget();

    /*LCDに表示*/
    SSD1306clear();
    // LINEIRSW Checkerのビットマップを表示
    SSD1306Bitmap(0, 0, LINEIRSWChecker_bitmap, 128, 64, false);
    // LINEの情報を表示
    SSD1306write(1, 68, 44, String(LINEedge), false);
    SSD1306write(1, 68, 53, String(LINEleft) + String(LINEright) + String(LINEback) + "0", false);
    SSD1306line(111, 48, 111 - sin(radians(LINEedge)) * 15, 48 - cos(radians(LINEedge)) * 15, false);
    // IRボールの情報を表示
    SSD1306write(1, 4, 44, String(IRedge), false);
    SSD1306write(1, 4, 53, String(IRdist), false);
    SSD1306line(47, 47, 47 - sin(radians(IRedge)) * 15, 48 - cos(radians(IRedge)) * 15, false);
    // BNO055の情報を表示
    SSD1306write(1, 79, 21, String(BNO055deg), false);
    SSD1306line(116, 20, 116 - sin(radians(BNO055deg)) * 10, 20 - cos(radians(BNO055deg)) * 10, false);
    // 可変抵抗の値を表示
    SSD1306write(1, 48, 21, String(analogRead(6)), false);
    // SWの情報を表示
    SSD1306write(1, 4, 21, String(LeftSW_true) + String(RightSW_true) + String(EnterSW_true) + String(BNO055resetSW) + String(CamToggle) + String(MotorToggle), false);
    SSD1306show();

    /*シリアル*/
    Serial.println("( " + String(IRball_p) + " , " + String(IRball_q) + " )");

    /*モーター制御*/
    now_LINEtime = millis() - old_LINEtime; // タイマーを更新

    if (now_LINEtime <= LINETIME)
    {
        Motor4pidControl(MOVEMOTORPOWER, old_LINEedge + 180, GYROMOTORPOWER, BNO055deg, 0); // LINEとは逆方向に移動させる
    }
    else if (LINEexist)
    {
        int Final_LINEedge = 0; // 最終的なラインの角度を格納する変数
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
        Motor4pidControl(MOVEMOTORPOWER, Final_LINEedge + 180, GYROMOTORPOWER, BNO055deg, 0); // LINEとは逆方向に移動させる
        old_LINEedge = Final_LINEedge;                                                        // 昔の角度を更新
        old_LINEtime = millis();                                                              // 昔の時間を更新
    }
    else
    {
        if (IRexist)
        {
            if (IRedge <= IRFRONT_VAL || IRedge >= 360 - IRFRONT_VAL)
            {
                Motor4pidControl(MOVEMOTORPOWER, 0, GYROMOTORPOWER, BNO055deg, 0); // まっすぐ進む
            }
            else if (IRedge <= IRNEARFRONT_VAL || IRedge >= 360 - IRNEARFRONT_VAL)
            {
                // 前にボールがあるとき
                int IRballaddedge = map(IRdist, 0, 1023, MAXAROUNDEDGE_FRONT, MINAROUNDEDGE_FRONT);
                IRballmoveAroundedge = (IRedge > 180) ? IRedge - IRballaddedge : IRedge + IRballaddedge; // 少しずれた角度になるように計算させる

                Motor4pidControl(MOVEMOTORPOWER, IRballmoveAroundedge, GYROMOTORPOWER, BNO055deg, 0); // 回り込む方向に移動
            }
            else
            {
                if (IRdist > IRDISTFARVAL)
                {
                    Motor4pidControl(MOVEMOTORPOWER, IRedge, GYROMOTORPOWER, BNO055deg, 0); // ボールの方向に移動
                }
                else
                {
                    int IRballaddedge = map(IRdist, 0, IRDISTFARVAL, MAXAROUNDEDGE, MINAROUNDEDGE);
                    IRballmoveAroundedge = (IRedge > 180) ? IRedge - IRballaddedge : IRedge + IRballaddedge; // 回り込む角度を計算させる

                    Motor4pidControl(MOVEMOTORPOWER, IRballmoveAroundedge, GYROMOTORPOWER, BNO055deg, 0); // 回り込む方向に移動
                }
            }
        }
        else
        {
            Motor4pidControl(0, 0, GYROMOTORPOWER, BNO055deg, 0); // IRボールが存在しない場合はPID制御のみ
        }
    }

    delay(50); // 50ms待機する
}

/*関数*/

/*　回り込むための角度を計算する　*/
// R: ロボットの半径, r: IRボールの半径, p: IRボールのX座標, q: IRボールのY座標
// 角度の算出方法について : https://www.desmos.com/calculator/dip4ot39as?lang=ja

long double A = 0.0;       // 接線を出すための係数を格納するための変数
long double B = 0.0;       // 接線を出すための係数を格納するための変数
long double Delta_X = 0.0; // Xの変化量を格納するための変数
long double Delta_Y = 0.0; // Yの変化量を格納するための変数

double IRball_Calculate(double IRedge_move, double R, double r, double p, double q)
{
    double pp = p * p;                 // pの二乗
    double qq = q * q;                 // qの二乗
    double rr = r + R;                 // rとRの和
    double rr2 = rr * rr;              // rとRの和の二乗
    double denom = pp + qq;            // pの二乗とqの二乗の和
    double root = sqrt(pp + qq - rr2); // pの二乗とqの二乗の和からrとRの和の二乗を引いたものの平方根

    if (IRedge_move < 180)
    {
        /* 左にボールがあるときの接点 */
        A = (rr * p + q * root) / denom;
        B = (rr * q - p * root) / denom;
        // 原点を中心とする半径Rの円の接点座標 => (R * A, R * B)
        // (p,q)を中心とする半径rの円の接点座標 => (p - r * A, q - r * B)
    }
    else
    {
        /* 右にボールがあるときの接点 */
        A = (rr * p - q * root) / denom;
        B = (rr * q + p * root) / denom;
        // 原点を中心とする半径Rの円の接点座標 => (R * A, R * B)
        // (p,q)を中心とする半径rの円の接点座標 => (p - r * A, q - r * B)
    }

    double return_edge; // 計算した角度を格納する変数

    if (IRedge_move == -1)
    {
        return_edge = -1; // ボールがないときは-1を格納
    }
    else
    {
        Delta_X = (p - r * A) - R * A; // Xの変化量を計算
        Delta_Y = (q - r * B) - R * B; // Yの変化量を計算

        return_edge = atan2(Delta_X, Delta_Y); // 回り込むための角度を計算
        return_edge = degrees(return_edge);    // ラジアンから度に変換

        if (return_edge < 0)
            return_edge += 360.0;
        if (return_edge >= 360.0)
            return_edge -= 360.0;

        if (IRedge != -1 && (IRedge_move < 90 || IRedge_move > 270))
            return_edge += 180.0; // 回り込むための角度を調整
        if (return_edge >= 360.0)
            return_edge -= 360.0; // 回り込むための角度を調整
    }

    if (pp + qq - rr2 < 0)
        return_edge = -1; // ボールが存在しないときは-1を格納

    return return_edge; // 回り込むための角度を返す
}

/*　ロボットを指定した角度に移動させる関数　*/
void Motor4pidControl(double speed_move, double edge_move, double speed_gyro, double edge_gyro, double goaledge_gyro)
{
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

void Motor4Stop()
{
    DSR1202move(0, 0, 0, 0); // モータを停止する
}