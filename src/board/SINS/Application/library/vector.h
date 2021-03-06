/*
 * vector.h
 *
 *  Created on: Jul 6, 2019
 *      Author: michael
 */

#ifndef VECTOR_H_
#define VECTOR_H_

/*
 * Vector minus vector
 */
void vmv(const float a[3], const float b[3], float amb[3]);

/*
 * Matrix multiplied by vector
 */
void mxv(const float m[3][3], const float v[3], float mv[3]);

#endif /* VECTOR_H_ */
