#include "gaussianShifter.hpp"

void gaussianShifter(float *data, int size, int radius, float sigma)
{
    float *temp = new float[size];
    float *kernel = new float[2 * radius + 1];

    // カーネル作成
    for (int k = -radius; k <= radius; k++)
    {
        kernel[k + radius] = exp(-(k * k) / (2.0f * sigma * sigma));
    }

    for (int i = 0; i < size; i++)
    {
        float sum_weight = 0.0f;
        float sum_value = 0.0f;

        for (int k = -radius; k <= radius; k++)
        {
            int idx = i + k;
            if (idx < 0 || idx >= size)
                continue;

            float weight = kernel[k + radius];
            sum_weight += weight;
            sum_value += data[idx] * weight;
        }

        temp[i] = sum_value / sum_weight;
    }

    for (int i = 0; i < size; i++)
        data[i] = temp[i];

    delete[] kernel;
    delete[] temp;
}