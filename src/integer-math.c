#include "integer.h"
#include "util.h"

// Internal inlines definitions

static inline kk_vi_t add_borrowed_kkbigint_to_borrowed_kkbigint(kk_bi_t bigint_a, kk_bi_t bigint_b);
static inline kk_vi_t add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint(kk_bi_t bigint_larger, kk_bi_t bigint_smaller);
static inline kk_vi_t add_borrowed_kkbigint_to_kksmallint(kk_bi_t bigint_a, kk_si_t varint_b);
static inline kk_vi_t add_kksmallint_to_kksmallint(kk_si_t varint_a, kk_si_t varint_b);

// Functions

kk_vi_t _add_borrowed_kkvarint_to_borrowed_kkvarint_slow(kk_vi_t varint_a, kk_vi_t varint_b)
{
  if (KK_VARINT_IS_BIGINT(varint_a & varint_b))
    return add_borrowed_kkbigint_to_borrowed_kkbigint(kkvarint_as_kkbigint(varint_a), kkvarint_as_kkbigint(varint_b));

  if (KK_VARINT_IS_BIGINT(varint_a))
    return add_borrowed_kkbigint_to_kksmallint(kkvarint_as_kkbigint(varint_a), kkvarint_as_kksmallint(varint_b));

  if (KK_VARINT_IS_BIGINT(varint_b))
    return add_borrowed_kkbigint_to_kksmallint(kkvarint_as_kkbigint(varint_b), kkvarint_as_kksmallint(varint_a));

  return add_kksmallint_to_kksmallint(kkvarint_as_kksmallint(varint_a), kkvarint_as_kksmallint(varint_b));
}

// Internal inlines

static inline kk_vi_t add_borrowed_kkbigint_to_borrowed_kkbigint(kk_bi_t bigint_a, kk_bi_t bigint_b)
{
  if (KK_BI_GET_FULLP_LENGTH(bigint_a) > KK_BI_GET_FULLP_LENGTH(bigint_b))
    return add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint(bigint_a, bigint_b);
  else
    return add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint(bigint_b, bigint_a);
}

static inline kk_vi_t add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_optiona(kk_bi_t bigint_larger, kk_bi_t bigint_smaller)
{
  kk_bi_length_t larger_length = KK_BI_GET_FULLP_LENGTH(bigint_larger);
  kk_bi_length_t smaller_length = KK_BI_GET_FULLP_LENGTH(bigint_smaller);
  kk_bi_fullp_arr_t data_larger = KK_BI_GET_FULLP_ARRAY(bigint_larger);
  kk_bi_fullp_arr_t data_smaller = KK_BI_GET_FULLP_ARRAY(bigint_smaller);

  kk_bi_fullpart_t carry = 0;
  kk_bi_fullpart_t a = 0;
  kk_bi_fullpart_t b = 0;
  for (size_t i = 0; i < smaller_length; i++)
  {
    a = data_smaller[i];
    b = data_larger[i];
    carry = a + b + carry;
    data_larger[i] = carry;

    // TODO: improve
    carry = (((a | b) & ~carry) | (a & b)) >> (KK_BI_FULLPART_BITS - 1);
  }

  if (carry)
  {
    if (smaller_length == larger_length)
    {
      bigint_larger = kkbigint_resize(bigint_larger, larger_length + 1);
      data_larger = KK_BI_GET_FULLP_ARRAY(bigint_larger);
    }
    data_larger[smaller_length] = carry;
  }

  free_kkbigint(bigint_smaller);

  return kkbigint_as_kkvarint(bigint_larger);
}

static inline kk_vi_t add_borrowed_larger_kkbigint_to_borrowed_smaller_kkbigint_optionb(kk_bi_t bigint_larger, kk_bi_t bigint_smaller)
{
  kk_bi_length_t larger_length = KK_BI_GET_FULLP_LENGTH(bigint_larger);
  kk_bi_length_t smaller_length = KK_BI_GET_FULLP_LENGTH(bigint_smaller);
  kk_bi_fullp_arr_t data_larger = KK_BI_GET_FULLP_ARRAY(bigint_larger);
  kk_bi_fullp_arr_t data_smaller = KK_BI_GET_FULLP_ARRAY(bigint_smaller);

  kk_bi_fullpart_t carry = 0;
  kk_bi_fullpart_t a = 0;
  kk_bi_fullpart_t b = 0;
  for (size_t i = 0; i < smaller_length; i++)
  {
    a = data_smaller[i];
    b = data_larger[i];
    carry = a + b + carry;
    data_larger[i] = carry;

    // TODO: improve
    carry = (((a | b) & ~carry) | (a & b)) >> (KK_BI_FULLPART_BITS - 1);
  }

  if (carry)
  {
    if (smaller_length == larger_length)
    {
      bigint_larger = kkbigint_resize(bigint_larger, larger_length + 1);
      data_larger = KK_BI_GET_FULLP_ARRAY(bigint_larger);
    }
    data_larger[smaller_length] = carry;
  }

  free_kkbigint(bigint_smaller);

  return kkbigint_as_kkvarint(bigint_larger);
}

static inline kk_vi_t add_borrowed_kkbigint_to_kksmallint(kk_bi_t bigint_a, kk_si_t varint_b)
{
  printf("add_borrowed_kkbigint_to_borrowed_kkvarint\n");
  free_kkbigint(bigint_a);

  // ignore unused
  (void)varint_b;
  return 0;
}

static inline kk_vi_t add_kksmallint_to_kksmallint(kk_si_t varint_a, kk_si_t varint_b)
{
  printf("add_kksmallint_to_kksmallint\n");
  // ignore unused
  (void)varint_a;
  (void)varint_b;
  return 0;
}
