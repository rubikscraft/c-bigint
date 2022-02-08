#include "integer.h"
#include "util.h"

// Internal inlines definitions

static inline kk_vi_t add_borrowed_kkbigint_to_borrowed_kkbigint(kk_bi_t bigint_a, kk_bi_t bigint_b);
static inline kk_vi_t add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_option(
    kk_bi_t bigint_larger,
    kk_bi_t bigint_smaller,
    kk_bi_length_t larger_length,
    kk_bi_smallp_arr_t data_larger,
    kk_bi_length_t smaller_length,
    kk_bi_smallp_arr_t data_smaller);
static inline kk_vi_t add_borrowed_kkbigint_to_kksmallint(kk_bi_t bigint_a, kk_si_t varint_b);
static inline kk_vi_t add_kksmallint_to_kksmallint(kk_si_t varint_a, kk_si_t varint_b);

// Functions

kk_vi_t _add_borrowed_kkvarint_to_borrowed_kkvarint_slow(kk_vi_t varint_a, kk_vi_t varint_b)
{
  if (KK_VI_IS_BI(varint_a & varint_b))
    return add_borrowed_kkbigint_to_borrowed_kkbigint(kkvarint_as_kkbigint(varint_a), kkvarint_as_kkbigint(varint_b));

  if (KK_VI_IS_BI(varint_a))
    return add_borrowed_kkbigint_to_kksmallint(kkvarint_as_kkbigint(varint_a), kkvarint_as_kksmallint(varint_b));

  if (KK_VI_IS_BI(varint_b))
    return add_borrowed_kkbigint_to_kksmallint(kkvarint_as_kkbigint(varint_b), kkvarint_as_kksmallint(varint_a));

  return add_kksmallint_to_kksmallint(kkvarint_as_kksmallint(varint_a), kkvarint_as_kksmallint(varint_b));
}

// Internal inlines

static inline kk_vi_t add_kksmallint_to_kksmallint(kk_si_t varint_a, kk_si_t varint_b)
{
  return kkbigint_as_kkvarint(create_kkbigint(varint_a + varint_b));
}

static inline kk_vi_t add_borrowed_kkbigint_to_borrowed_kkbigint(kk_bi_t bigint_a, kk_bi_t bigint_b)
{
  kk_bi_length_t a_length = KK_BI_GET_SMALLP_LENGTH(bigint_a);
  kk_bi_length_t b_length = KK_BI_GET_SMALLP_LENGTH(bigint_b);
  kk_bi_smallp_arr_t a_data = KK_BI_GET_SMALLP_ARRAY(bigint_a);
  kk_bi_smallp_arr_t b_data = KK_BI_GET_SMALLP_ARRAY(bigint_b);

  if (a_length > b_length)
    return add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_option(bigint_a, bigint_b, a_length, a_data, b_length, b_data);
  else
    return add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_option(bigint_b, bigint_a, b_length, b_data, a_length, a_data);
}

static inline kk_vi_t add_borrowed_kkbigint_to_kksmallint(kk_bi_t bigint_a, kk_si_t varint_b)
{
  kk_bi_length_t big_length = KK_BI_GET_SMALLP_LENGTH(bigint_a);
  kk_bi_smallp_arr_t big_data = KK_BI_GET_SMALLP_ARRAY(bigint_a);
  kk_bi_length_t small_length = sizeof(kk_si_t);
  kk_bi_smallp_arr_t small_data = (kk_bi_smallp_arr_t)&varint_b;

  return add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_option(bigint_a, NULL, big_length, big_data, small_length, small_data);
}

static inline kk_vi_t add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_option(
    kk_bi_t bigint_larger,
    kk_bi_t bigint_smaller,
    kk_bi_length_t larger_length,
    kk_bi_smallp_arr_t data_larger,
    kk_bi_length_t smaller_length,
    kk_bi_smallp_arr_t data_smaller)
{
  // printf("add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_optionb\n");

  kk_bi_fullpart_t carry = 0;

  kk_bi_smallpart_t a = 0;
  kk_bi_smallpart_t b = 0;

  for (size_t i = 0; i < smaller_length; i++)
  {
    a = data_smaller[i];
    b = data_larger[i];
    carry = (kk_bi_fullpart_t)a + (kk_bi_fullpart_t)b + carry;
    data_larger[i] = (kk_bi_smallpart_t)carry;

    carry >>= KK_BI_SMALLPART_BITS;
  }

  if (carry)
  {
    if (smaller_length == larger_length)
    {
      bigint_larger = kkbigint_resize(bigint_larger, KK_BI_SMALLP_TO_FULLP_LEN(larger_length) + 1);
      data_larger = KK_BI_GET_SMALLP_ARRAY(bigint_larger);
    }
    data_larger[smaller_length] = (kk_bi_smallpart_t)carry;
  }

  free_kkbigint(bigint_smaller);

  return kkbigint_as_kkvarint(bigint_larger);
}
