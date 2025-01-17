#include "kem.hpp"
#include "utils.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <vector>

// Benchmarking IND-CCA2-secure Kyber KEM key generation algorithm
template<size_t k, size_t eta1>
void
bench_keygen(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = kyber_utils::get_kem_public_key_len<k>();
  constexpr size_t sklen = kyber_utils::get_kem_secret_key_len<k>();

  std::vector<uint8_t> d(slen);
  std::vector<uint8_t> z(slen);
  std::vector<uint8_t> pkey(pklen);
  std::vector<uint8_t> skey(sklen);

  auto _d = std::span<uint8_t, slen>(d);
  auto _z = std::span<uint8_t, slen>(z);
  auto _pkey = std::span<uint8_t, pklen>(pkey);
  auto _skey = std::span<uint8_t, sklen>(skey);

  prng::prng_t prng;
  prng.read(_d);
  prng.read(_z);

  for (auto _ : state) {
    kem::keygen<k, eta1>(_d, _z, _pkey, _skey);

    benchmark::DoNotOptimize(_d);
    benchmark::DoNotOptimize(_z);
    benchmark::DoNotOptimize(_pkey);
    benchmark::DoNotOptimize(_skey);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

// Benchmarking IND-CCA2-secure Kyber KEM encapsulation algorithm
template<size_t k, size_t eta1, size_t eta2, size_t du, size_t dv>
void
bench_encapsulate(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = kyber_utils::get_kem_public_key_len<k>();
  constexpr size_t sklen = kyber_utils::get_kem_secret_key_len<k>();
  constexpr size_t ctlen = kyber_utils::get_kem_cipher_len<k, du, dv>();
  constexpr size_t klen = 32;

  std::vector<uint8_t> d(slen);
  std::vector<uint8_t> z(slen);
  std::vector<uint8_t> m(slen);
  std::vector<uint8_t> pkey(pklen);
  std::vector<uint8_t> skey(sklen);
  std::vector<uint8_t> cipher(ctlen);
  std::vector<uint8_t> sender_key(klen);

  auto _d = std::span<uint8_t, slen>(d);
  auto _z = std::span<uint8_t, slen>(z);
  auto _m = std::span<uint8_t, slen>(m);
  auto _pkey = std::span<uint8_t, pklen>(pkey);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _cipher = std::span<uint8_t, ctlen>(cipher);
  auto _sender_key = std::span<uint8_t, klen>(sender_key);

  prng::prng_t prng;
  prng.read(_d);
  prng.read(_z);

  kem::keygen<k, eta1>(_d, _z, _pkey, _skey);

  prng.read(_m);

  for (auto _ : state) {
    auto skdf = kem::encapsulate<k, eta1, eta2, du, dv>(_m, _pkey, _cipher);
    benchmark::DoNotOptimize(skdf);
    skdf.squeeze(_sender_key);

    benchmark::DoNotOptimize(_m);
    benchmark::DoNotOptimize(_pkey);
    benchmark::DoNotOptimize(_cipher);
    benchmark::DoNotOptimize(_sender_key);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
}

// Benchmarking IND-CCA2-secure Kyber KEM decapsulation algorithm
template<size_t k, size_t eta1, size_t eta2, size_t du, size_t dv>
void
bench_decapsulate(benchmark::State& state)
{
  constexpr size_t slen = 32;
  constexpr size_t pklen = kyber_utils::get_kem_public_key_len<k>();
  constexpr size_t sklen = kyber_utils::get_kem_secret_key_len<k>();
  constexpr size_t ctlen = kyber_utils::get_kem_cipher_len<k, du, dv>();
  constexpr size_t klen = 32;

  std::vector<uint8_t> d(slen);
  std::vector<uint8_t> z(slen);
  std::vector<uint8_t> m(slen);
  std::vector<uint8_t> pkey(pklen);
  std::vector<uint8_t> skey(sklen);
  std::vector<uint8_t> cipher(ctlen);
  std::vector<uint8_t> sender_key(klen);
  std::vector<uint8_t> receiver_key(klen);

  auto _d = std::span<uint8_t, slen>(d);
  auto _z = std::span<uint8_t, slen>(z);
  auto _m = std::span<uint8_t, slen>(m);
  auto _pkey = std::span<uint8_t, pklen>(pkey);
  auto _skey = std::span<uint8_t, sklen>(skey);
  auto _cipher = std::span<uint8_t, ctlen>(cipher);
  auto _sender_key = std::span<uint8_t, klen>(sender_key);
  auto _receiver_key = std::span<uint8_t, klen>(receiver_key);

  prng::prng_t prng;
  prng.read(_d);
  prng.read(_z);

  kem::keygen<k, eta1>(_d, _z, _pkey, _skey);

  prng.read(_m);

  auto skdf = kem::encapsulate<k, eta1, eta2, du, dv>(_m, _pkey, _cipher);
  skdf.squeeze(_sender_key);

  for (auto _ : state) {
    auto rkdf = kem::decapsulate<k, eta1, eta2, du, dv>(_skey, _cipher);
    benchmark::DoNotOptimize(rkdf);
    rkdf.squeeze(_receiver_key);

    benchmark::DoNotOptimize(_skey);
    benchmark::DoNotOptimize(_cipher);
    benchmark::DoNotOptimize(_receiver_key);
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations());
  assert(std::ranges::equal(_sender_key, _receiver_key));
}

// Register for benchmarking IND-CCA2-secure Kyber Key Encapsulation Mechanism

// Kyber512
BENCHMARK(bench_keygen<2, 3>)->Name("kyber512/keygen");
BENCHMARK(bench_encapsulate<2, 3, 2, 10, 4>)->Name("kyber512/encap");
BENCHMARK(bench_decapsulate<2, 3, 2, 10, 4>)->Name("kyber512/decap");

// Kyber768
BENCHMARK(bench_keygen<3, 2>)->Name("kyber768/keygen");
BENCHMARK(bench_encapsulate<3, 2, 2, 10, 4>)->Name("kyber768/encap");
BENCHMARK(bench_decapsulate<3, 2, 2, 10, 4>)->Name("kyber768/decap");

// Kyber1024
BENCHMARK(bench_keygen<4, 2>)->Name("kyber1024/keygen");
BENCHMARK(bench_encapsulate<4, 2, 2, 11, 5>)->Name("kyber1024/encap");
BENCHMARK(bench_decapsulate<4, 2, 2, 11, 5>)->Name("kyber1024/decap");
