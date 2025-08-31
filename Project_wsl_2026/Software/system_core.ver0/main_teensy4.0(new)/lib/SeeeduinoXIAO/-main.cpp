#include <Arduino.h>
#include "MySettings.h"
#include "LINEXIAO.h"
#include "IRXIAO.h"

/*ChartoInt*/

int ChartoInt(char *p, int start_index, int end_index)
{
    char Char_p[end_index - start_index + 2];                     // +1(数) +1('\0')
    memcpy(Char_p, &p[start_index], end_index - start_index + 1); // p からコピー
    Char_p[end_index - start_index + 1] = '\0';                   // 終端追加
    return atoi(Char_p);                                          // 文字列を数値に変換
}

/*LINEXIAO*/

int LINEedge = -1;
bool LINEright = false;
bool LINEleft = false;
bool LINEback = false;
bool LINEexist = false;

void LINEXIAObegin()
{
    LINEXIAO_SERIAL.begin(LINEXIAO_SERIALSPEED);
}

void LINEXIAOget()
{
    while (LINEXIAO_SERIAL.available())
    {
        char buf[20];                                                             // 多めにとる、受信したデータを格納する変数
        int buf_long = LINEXIAO_SERIAL.readBytesUntil('a', buf, sizeof(buf) - 1); // データを読み取る('a'まで読み取り、bufに、最大sizeof(buf) - 1バイトのデータまで読み取る)
        // ↑読み取ったブッファ量をbuf_longに記録
        // ↑'a'は読まないので7文字受信
        buf[buf_long] = '\0'; // 終端記号を追加(シリアルプリント時などに'\0'がないと永遠に読み込まれるらしい)

        // 受信したデータが6文字なら正常なデータである
        if (buf_long == 6)
        {
            if (buf[0] == '-')
            {
                LINEedge = -1; // IRボールが存在しないときは-1を格納
            }
            else
            {
                LINEedge = ChartoInt(buf, 0, 2);
            }
            // SIDELINEの状態を取得
            LINEright = ChartoInt(buf, 3, 3);
            LINEleft = ChartoInt(buf, 4, 4);
            LINEback = ChartoInt(buf, 5, 5);
        }
    }

    if (LINEright == 1 || LINEleft == 1 || LINEback == 1 || LINEedge != -1)
    {
        LINEexist = true; // LINEが存在する
    }
    else
    {
        LINEexist = false; // LINEが存在しない
    }
}

/*IRXIAO*/

int IRedge = -1;
int IRdist = -1;
double IRball_p = 0.0;
double IRball_q = 0.0;
bool IRexist = false;

void IRXIAObegin()
{
    IRXIAO_SERIAL.begin(IRXIAO_SERIALSPEED);
}

void IRXIAOget()
{
    while (IRXIAO_SERIAL.available())
    {
        char buf[20];                                                           // 多めにとる、受信したデータを格納する変数
        int buf_long = IRXIAO_SERIAL.readBytesUntil('a', buf, sizeof(buf) - 1); // データを読み取る('a'まで読み取り、bufに、最大sizeof(buf) - 1バイトのデータまで読み取る)
        // ↑読み取ったブッファ量をbuf_longに記録
        // ↑'a'は読まないので7文字受信
        buf[buf_long] = '\0'; // 終端記号を追加(シリアルプリント時などに'\0'がないと永遠に読み込まれるらしい)

        // 受信したデータが6文字なら正常なデータである
        if (buf_long == 6)
        {
            if (buf[0] == '-')
            {
                IRedge = -1; // IRボールが存在しないときは-1を格納
                IRdist = -1; // IRボールが存在しないときは-1を格納

                IRball_p = 0.0; // IRボールが存在しないときは0を格納
                IRball_q = 0.0; // IRボールが存在しないときは0を格納
            }
            else
            {
                IRedge = ChartoInt(buf, 0, 2); // 0番目から2番目までの数をint型で読み取る
                IRedge = (IRedge + 360) % 360; // 調整

                IRdist = ChartoInt(buf, 3, 5); // 3番目から6番目までの数をint型で読み取る
                if (IRedge < 90 && IRedge > 270)
                    IRdist /= 2; // 調整
                else
                    IRdist /= 2; // 調整
            }
        }
    }

    if (IRedge != -1)
    {
        IRexist = true; // IRボールが存在する
    }
    else
    {
        IRexist = false; // IRボールが存在しない
    }
}
