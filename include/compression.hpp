#pragma once
#include "field.hpp"
#include "ntt.hpp"
#include "params.hpp"
#include <span>

// IND-CPA-secure Public Key Encryption Scheme Utilities
namespace kyber_utils {

// Given an element x ∈ Z_q | q = 3329, this routine compresses it by discarding
// some low-order bits, computing y ∈ [0, 2^d) | d < round(log2(q))
//
// See top of page 5 of Kyber specification
// https://pq-crystals.org/kyber/data/kyber-specification-round3-20210804.pdf
template<size_t d>
static inline field::zq_t
compress(const field::zq_t x)
  requires(kyber_params::check_d(d))
{
  constexpr uint16_t t0 = 1u << d;
  constexpr uint32_t t1 = static_cast<uint32_t>(field::Q >> 1);

  const uint32_t t2 = x.to_canonical() << d;
  const uint32_t t3 = t2 + t1;
  const uint16_t t4 = static_cast<uint16_t>(t3 / field::Q);
  const uint16_t t5 = t4 & (t0 - 1);

  return field::zq_t::from_canonical(t5);
}

// Given an element x ∈ [0, 2^d) | d < round(log2(q)), this routine decompresses
// it back to y ∈ Z_q | q = 3329
//
// This routine recovers the compressed element with error probability as
// defined in eq. 2 of Kyber specification.
//
// See top of page 5 of Kyber specification
// https://pq-crystals.org/kyber/data/kyber-specification-round3-20210804.pdf
template<size_t d>
static inline field::zq_t
decompress(const field::zq_t x)
  requires(kyber_params::check_d(d))
{
  constexpr uint32_t t0 = 1u << d;
  constexpr uint32_t t1 = t0 >> 1;

  const uint32_t t2 = static_cast<uint32_t>(field::Q * x.to_canonical());
  const uint32_t t3 = t2 + t1;
  const uint16_t t4 = static_cast<uint16_t>(t3 >> d);

  return field::zq_t::from_canonical(t4);
}

// Utility function to compress each of 256 coefficients of a degree-255
// polynomial s.t. input polynomial is mutated.
template<size_t d>
static inline void
poly_compress(std::span<field::zq_t, ntt::N> poly)
  requires(kyber_params::check_d(d))
{
  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] = compress<d>(poly[i]);
  }
}

// Utility function to decompress each of 256 coefficients of a degree-255
// polynomial s.t. input polynomial is mutated.
template<size_t d>
static inline void
poly_decompress(std::span<field::zq_t, ntt::N> poly)
  requires(kyber_params::check_d(d))
{
  for (size_t i = 0; i < poly.size(); i++) {
    poly[i] = decompress<d>(poly[i]);
  }
}

}
