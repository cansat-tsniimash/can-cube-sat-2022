/*
 * lds.h
 *
 *  Created on: 16 мар. 2019 г.
 *      Author: sereshotes
 */

#ifndef LDS_H_
#define LDS_H_

#include <math.h>

#define LDS_COUNT 3
#define LDS_DIM 3

static const float __lds_array_inv[LDS_DIM][LDS_COUNT] = {
        {1.00000, 0.00000, 0.00000 },
        {-1.00000, 0.75249, 0.75249 },
        {0.00000, -1.46190, 1.46190 }
};
static const float __lds_array_str[LDS_COUNT][LDS_DIM] = {
        {1.00000, 0.00000, 0.00000 },
        {0.66446, 0.66446, -0.34202 },
        {0.66446, 0.66446, 0.34202 }
};
__attribute__((unused))
static void lds_find(float x[LDS_DIM], const float b[LDS_COUNT]) {
    for (int i = 0; i < LDS_DIM; i++) {
        x[i] = 0;
        for (int j = 0; j < LDS_COUNT; j++) {
            x[i] += __lds_array_inv[i][j] * b[j];
        }
    }
}

//Возвращает стандартное отклонение
__attribute__((unused))
static float lds_get_error(const float x[LDS_DIM], const float b[LDS_COUNT]) {
    int sumsq = 0;
    for (int i = 0; i < LDS_COUNT; i++) {
        float b0i = 0;
        for (int j = 0; j < LDS_DIM; j++) {
            b0i += __lds_array_str[i][j] * x[j];
        }
        float d = b0i - b[i];
        sumsq += d * d;
    }
    return sqrt(sumsq);
}
__attribute__((unused))
static void dekart_to_euler(const float x[3], float sph[3]) {
    sph[0] = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    sph[1] = acos(x[2] / sph[0]);
    sph[2] = atan(x[1] / x[0]);
}
__attribute__((unused))
static float degrees(float a) {
    return a * 180 / M_PI;
}


#endif /* LDS_H_ */
