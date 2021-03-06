/*
 * lds.h
 *
 *  Created on: 16 мар. 2019 г.
 *      Author: sereshotes
 */

#ifndef LDS_H_
#define LDS_H_

#include <math.h>
#include <sins_config.h>
#include "matrix.h"

#define LDS_COUNT ITS_SINS_LDS_COUNT
#define LDS_DIM 3

int lds_find_bound(float x[LDS_DIM], const float b[LDS_COUNT], float bound);

int lds_get_ginversed(const float b[LDS_COUNT], float bound, Matrixf *out);

void lds_solution(const float b[LDS_COUNT], const Matrixf *ginv, float x[3]);

float lds_calc_error(const float b[LDS_COUNT], float bound, const Matrixf *ginv, const float x[3]) ;

static const float __lds_number = 1.23793 * 3.16228;

static const float __lds_array_inv[LDS_DIM][LDS_COUNT] = {
        {0.21795, 0.21795, 0.12782, -0.04809, -0.04809, -0.21795, -0.21795, -0.12782, 0.04809, 0.04809, },
        {0.04809, 0.04809, -0.12782, -0.21795, -0.21795, -0.04809, -0.04809, 0.12782, 0.21795, 0.21795, },
        {-0.36548, 0.36548, 0.00000, 0.36548, -0.36548, -0.36548, 0.36548, 0.00000, 0.36548, -0.36548, },
        };
static const float __lds_array_str[LDS_COUNT][LDS_DIM] = {
        {0.93969, 0.00000, -0.34202, },
        {0.93969, 0.00000, 0.34202, },
        {0.70711, -0.70711, 0.00000, },
        {-0.00000, -0.93969, 0.34202, },
        {-0.00000, -0.93969, -0.34202, },
        {-0.93969, 0.00000, -0.34202, },
        {-0.93969, 0.00000, 0.34202, },
        {-0.70711, 0.70711, 0.00000, },
        {0.00000, 0.93969, 0.34202, },
        {0.00000, 0.93969, -0.34202, },
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
    float sumsq = 0;
    for (int i = 0; i < LDS_COUNT; i++) {
        float b0i = 0;
        for (int j = 0; j < LDS_DIM; j++) {
            b0i += __lds_array_str[i][j] * x[j];
        }
        float d = b0i - b[i];
        sumsq += d * d;
    }
    float sumb = 0;
    for (int i = 0; i < LDS_COUNT; i++) {
        sumb += b[i] * b[i];
    }

    return sqrt(sumsq / sumb);
}
__attribute__((unused))
static void dekart_to_euler(const float x[3], float sph[3]) {
    sph[0] = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    sph[1] = acos(x[2] / sph[0]);
    sph[2] = atan2(x[1], x[0]);
}
__attribute__((unused))
static void euler_to_dekart(float x[3], const float sph[3]) {
    x[0] = sph[0] * sin(sph[1]) * cos(sph[2]);
    x[1] = sph[0] * sin(sph[1]) * sin(sph[2]);
    x[2] = sph[0] * cos(sph[1]);
}
__attribute__((unused))
static float degrees(float a) {
    return a * 180 / M_PI;
}


#endif /* LDS_H_ */
