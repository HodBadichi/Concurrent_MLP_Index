//
// Created by eaad on 9/9/23.
//

#include "RangeModule.h"

uint8_t get_byte(uint64_t x, uint8_t b)
{
    assert(1<=b && b<=8);
    switch (b) {
        case 1:
            return BYTE1(x);
        case 2:
            return BYTE2(x);
        case 3:
            return BYTE3(x);
        case 4:
            return BYTE4(x);
        case 5:
            return BYTE5(x);
        case 6:
            return BYTE6(x);
        case 7:
            return BYTE7(x);
        case 8:
            return BYTE8(x);
        default:
            assert(0);
    }
    return 0;
}


/*
 * For two 64b numbers that forms a range (low, high),
 * we would like to get the index of the first byte
 * that after it the two numbers split.
 * Some examples:
 * 	0x0011223344556677, 0x1122334455667788
 * 		split at "byte" 0,
 * 	0x1122334455667788, 0x1122334455667789
 * 		split at byte 7
 * 	0xa5a5a5a5a5a5a5a5, 0xa5a5a5a5a5a5a5a5
 * 		split at byte 8 (they are the same)
 */
uint8_t get_split_byte_index(uint64_t x, uint64_t y)
{
    uint8_t split = 0;
    uint8_t xB, yB;
    xB = get_byte(x, split + 1);
    yB = get_byte(y, split + 1);
    while (xB == yB) {
        split++;
        if (unlikely(split == 8))
            break;
        xB = get_byte(x, split + 1);
        yB = get_byte(y, split + 1);
    }
    return split;
}

/*
 * Given a 64b key and a splitting point,
 * we are interested of the prefix that is defined by
 * that splitting point, since that prefix node in the
 * trie is where we store the existence of the range.
 *
 * For example, for the key 0x0011223344556677 and split
 * point = 3, the prefix will be 0x001122.
 */
uint64_t get_prefix(uint64_t x, uint8_t split_point)
{
    if (unlikely(split_point == 8))
        return x;

    uint64_t ret_val = 0;
    for (uint8_t i = 1; i <= split_point; i++)
        ret_val = (ret_val << 8) | get_byte(x, i);
    return ret_val;
}

/*
 * For a 64b number, and a byte index that after it
 * this number splits (with respect to other number)
 * we are interested to see if all bits after the byte index
 * are zero.
 * This means that the subtree with the prefix[:split_index]
 * has the minimal key it could hold.
 *
 * An example, for 0x0011223344556677 and split_index = 4
 * we split the number to:
 * 	prefix: 0x00112233
 * 	tail:	0x44556677
 * and since tail!=0, the minimal possible key for prefix
 * does not exist.
 *
 * Another example, f0r 0x0011223300000000 and split_index = 0,
 * the minimal key in the subtree of 0x00112233 does exist.
 *
 *
 * Note, a different way to look at this operation is to ask
 * if x is "perfect lower bound" for the prefix.
 *
 */
uint8_t is_minmal_key_for_subtree_exist(uint64_t x, uint8_t b)
{
    while (b < 8)
    {
        uint8_t B = get_byte(x, b+1);
        if (x != 0)
            return 0;
        b++;
    }
    return 1;
}

uint8_t convert_to_perfect_lower_bound(uint64_t *x, uint8_t b)
{
    uint64_t val = *x;

    /*
     * if b == 7, then the range differs only in the last byte (where the less
     * significant bits are). This case handles specially (normal
     * insertions.
     * if b==8, keys are the same, so we won't get to here.
     */
    if (b == 7 || b == 8)
        return 0;

    /*
     * Clear all right b-1 bytes;
     */
    SHIFT_RIGHT_BYTES(val, 8 - b - 1);

    if (SHIFT_LEFT_BYTES_EVAL(val, 8 - b - 1) == *x)
        return 0; //on this case, x was already perfect lower bound

    /*
     * Increment by 1
     */
    val++;

    /*
     * Return b-1 bytes to left;
     */
    SHIFT_LEFT_BYTES(val, 8 - b - 1);

    if (val == *x)
        return 0;
    *x = val;
    return 1;
}

/*
 * Similar to the above, we want to know for 64b key and
 * splitting point if the maximal key in that subtree
 * exists
 *
 * Note, a different way to look at this operation is to ask
 * if x is "perfect upper bound" for the prefix.
 */
uint8_t is_maximal_key_for_subtree_exist(uint64_t x, uint8_t b)
{
    while (b < 7)
    {
        uint8_t B = get_byte(x, b+1);
        if (x != 0xff)
            return 0;
        b++;
    }
    return 1;
}

uint8_t convert_to_perfect_upper_bound(uint64_t *x, uint8_t b)
{
    uint64_t val = *x;

    /*
     * if b == 7, then the range differs only in the last byte (where the less
     * significant bits are). This case handles specially (normal
     * insertions.
     * if b==8, keys are the same, so we won't get to here.
     */
    if (b == 7 || b == 8)
        return 0;

    /*
     * Clear all right b-1 bytes;
     */
    SHIFT_RIGHT_BYTES(val, 8 - b - 1);

    if (SHIFT_LEFT_BYTES_EVAL(val + 1, 8 - b - 1) - 1 == *x)
        return 0; //on this case, x was already perfect lower bound

    /*
     * Return b-1 bytes to left;
     */
    SHIFT_LEFT_BYTES(val, 8 - b - 1);

    /*
     * Decrement by 1
     */
    val--;


    if (val == *x)
        return 0;
    *x = val;
    return 1;
}

/*
 * For given two 64b numbers (x,y) that form a range,
 * we define a "perfect range" to be a range such that
 * after the spltting point of {x,y}, if we look on the prefix
 * that is defined by that splitting point, then the subtree
 * defined by that prefix conssists both of its minimal
 * key and its maximal key.
 *
 * Therfore, if a range is perfect, its subtree is complete
 * (contain all possible keys).
 */
uint8_t is_range_perfect(uint64_t x, uint64_t y)
{
    uint8_t split = get_split_byte_index(x, y);
    if (is_minmal_key_for_subtree_exist(x, split) &&
        is_maximal_key_for_subtree_exist(y, split))
        return 1;
    return 0;
}

/* For a given 64b number, and a splitting point we want to convert it
 * to the closest number that is a perfect lower bound with respect to
 * the splitting point.
 *
 * For example, 0x0011223333333333 and prefix 4
 * will be converted to 0x0011223400000000
 *
 * The function receives:
 * 	@uint64_t *x: a pointer to a key
 * 	@uin8_t split: the splitting point
 * 	@uint64_t *leftover: a pointer to two 64b numbers
 *
 * Then, the function updates x to the perfect lower bound,
 * and if x was indeed changed (i.e. was not already perfect lower bound),
 * the function also updates the leftover array to hold:
 * 	(old_x, new_x-1)
 *
 * The return value is 0 if leftovers was not set and 1 if it was set.
 */
uint8_t handle_lower_bound(uint64_t *x, uint8_t split, uint64_t *lf)
{
    lf[0] = *x;
    return 0;
}
/*
 * From here we start dealing with the real thing.
 *
 * For a given range (x,y), we would like to transform it
 * to the largest perfect range possible, and to two optional
 * ranges that are not perfect (that will be handled recursively).
 *
 * For example: for
 * 	0x0011223333333333,
 * 	0x0011229999999999
 *
 * we would like to get the range
 * 	0x0011223400000000,
 * 	0x00112298ffffffff
 * so we can store it directly at the prefix 0x001122 child bitmap,
 *
 * and we want to handle recursively the ranges
 * 	0x0011223333333333,
 * 	0x00112233ffffffff
 * and:
 * 	0x0011229900000000,
 * 	0x0011229999999999
 *
 * We will handle the "leftovers" intervals in the same approach.
 * Note that if we got to a splitting point = 7, i.e. the only different
 * byte is the last byte, then we have two options,
 * 	- if the range is perfect (like in the range (0x0, 0xff)
 * 	we can still hold the same logic.
 * 	- o.w. the range length is less than 256 so we insert all leaves just normally.
 *
 * ANOTHER NOTE: do we really need the 8 level in the tree? can't we just do the same-
 * store in the bitmap and if we see 1 in bitmap of level 7, then the child exist?
 * 	(if the leaves were storing data then of course we need them, but if we
 * 	just want to indicate existence i think we can do that)
 */

/*
 * The following function receives
 * @uint64_t *x, @uint6_t *y, @uint64_t *leftover1, @uint64_t *leftover2
 *
 * where leftover1/2 each have space for two uint64_t.
 *
 * Now, what the function does is to update x,y to the best possible perfect range,
 * and accordingly update leftover1 and leftover2 to the relevant leftovers.
 *
 * It returns value signifies:
 * 	if (x,y) were already perfect,
 * 	if leftover1 was updated,
 * 	if leftover2 was updated,
 * 	if both leftovers were updated.
 *
 *
 * For example, for
 * 	0x0011223333333333,
 * 	0x0011229999999999
 *
 * We will return
 * 	x = 0x0011223400000000,
 * 	y = 0x00112298ffffffff,
 * 	leftover1 = (0x0011223333333333, 0x00112233ffffffff),
 * 	leftover2 = (0x0011229900000000, 0x0011229999999999)
 */

int handle_range(uint64_t *x, uint64_t *y, uint64_t *lf1, uint64_t *lf2, uint8_t *sb)
{
    *sb = get_split_byte_index(*x, *y);

    lf1[0] = *x; //store original x as leftover1 lower bound
    lf2[1] = *y; //store original y as leftover2 upper bound

    uint8_t is_lf1_updated = convert_to_perfect_lower_bound(x, *sb);
    uint8_t is_lf2_updated = convert_to_perfect_upper_bound(y, *sb);

    lf1[1] = *x - 1; //store leftover1 upper bound
    lf2[0] = *y + 1; //store leftover2 lower bound

    if (is_lf1_updated && is_lf2_updated)
        return  BOTH_LEFTOVERS_UPDATED;
    else if (is_lf1_updated)
        return LEFTOVER1_UPDATED;
    else if (is_lf2_updated)
        return LEFTOVER2_UPDATED;
    return ALREADY_PERFECT_RANGE;
}
