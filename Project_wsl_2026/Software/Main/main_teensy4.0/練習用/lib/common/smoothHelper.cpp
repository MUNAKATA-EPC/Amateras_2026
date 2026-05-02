#include "smoothHelper.hpp"

//// パブリック関数 ////

// ラグランジュ補間の公式（イキり）
/*
      n-1       n-1
f(x) = Σ {yi *   Π   (x-xj) / (xi - xj)}
      i=0     j=0,j≠i
*/
float lagrangeShifter(int n, float pos[][2], float x)
{
    float y = 0.0f;

    for (int i = 0; i < n; i++)
    {
        float mother = 1.0f;
        float son = 1.0f;

        for (int j = 0; j < n; j++)
        {
            if (i == j)
                continue; // 同じ点はスキップ

            // 分子の累積： (x - x_j)
            son *= (x - pos[j][0]);

            // 分母の累積： (x_i - x_j)
            mother *= (pos[i][0] - pos[j][0]);
        }

        // y_i * (分子 / 分母) を加算
        y += (float)pos[i][1] * (son / mother);
    }

    return y;
}

float smoothShifter(float x1, float x2, float y1, float y2, float input_x)
{
    float x = fabsf(input_x);
    if (0 <= x && x <= x1)
    {
        return y1;
    }
    else if (x1 < x && x < x2)
    {

        float p = x2 - x1;
        float q = y2 - y1;

        return pow(x - x1, 3) * (-2 * q / (p * p * p)) + pow(x - x1, 2) * (3 * q / (p * p)) + y1; // 3次関数
        // return pow(x - x1, 4) * (-q / (p * p * p * p)) + pow(x - x1, 2) * (2 * q / (p * p)) + y1; // 4次関数
    }
    else
    {
        return y2;
    }
}