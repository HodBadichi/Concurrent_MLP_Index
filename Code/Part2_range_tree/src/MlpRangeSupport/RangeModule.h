//
// Created by eaad on 9/9/23.
//

#ifndef ORIGINWITHRANGE_RANGEMODULE_H
#define ORIGINWITHRANGE_RANGEMODULE_H

#include <stdint.h>
#include <assert.h>
#include "common.h"

#define SHIFT_RIGHT_BYTES(x, B) (x) >>= (8 * (B))
#define SHIFT_LEFT_BYTES(x, B) (x) <<= (8 * (B))
#define SHIFT_RIGHT_BYTES_EVAL(x, B) ((x) >> (8 * (B)))
#define SHIFT_LEFT_BYTES_EVAL(x, B) ((x) << (8 * (B)))

/*
 * We count bytes from left (msB) to right.
 * like that: uint64_t x = 0x0011223344556677
 */
#define BYTE1(x) (((x) >> 56) & 0xff)
#define BYTE2(x) (((x) >> 48) & 0xff)
#define BYTE3(x) (((x) >> 40) & 0xff)
#define BYTE4(x) (((x) >> 32) & 0xff)
#define BYTE5(x) (((x) >> 24) & 0xff)
#define BYTE6(x) (((x) >> 16) & 0xff)
#define BYTE7(x) (((x) >> 8) & 0xff)
#define BYTE8(x) (((x) >> 0) & 0xff)

enum handle_range_ret_vals {
    ALREADY_PERFECT_RANGE,
    LEFTOVER1_UPDATED,
    LEFTOVER2_UPDATED,
    BOTH_LEFTOVERS_UPDATED
};

uint8_t get_byte(uint64_t x, uint8_t b);
int handle_range(uint64_t *x, uint64_t *y, uint64_t *lf1, uint64_t *lf2, uint8_t *sb);
uint64_t get_prefix(uint64_t x, uint8_t split_point);

#endif //ORIGINWITHRANGE_RANGEMODULE_H
