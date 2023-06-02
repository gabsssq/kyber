#include "kyber512_kem.hpp"
#include "kyber768_kem.hpp"
#include "utils.hpp"
#include <cassert>
#include <fstream>

// Test functional correctness of Kyber PQC suite implementation
namespace test_kyber {

using namespace std::literals;
namespace utils = kyber_utils;
namespace kyber512 = kyber512_kem;
namespace kyber768 = kyber768_kem;

// Test if
//
// - Is Kyber512 KEM implemented correctly ?
// - Is it conformant with the specification ?
//
// using Known Answer Tests.
inline void
test_kyber512_kem_kat()
{
  const std::string kat_file = "./kats/kyber512.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string d;

    if (!std::getline(file, d).eof()) {
      std::string z;
      std::string pk;
      std::string sk;
      std::string m;
      std::string ct;
      std::string ss;

      std::getline(file, z);
      std::getline(file, pk);
      std::getline(file, sk);
      std::getline(file, m);
      std::getline(file, ct);
      std::getline(file, ss);

      auto _d = std::string_view(d);
      auto __d = _d.substr(_d.find("="sv) + 2, _d.size());
      auto ___d = utils::from_hex<32>(__d);

      auto _z = std::string_view(z);
      auto __z = _z.substr(_z.find("="sv) + 2, _z.size());
      auto ___z = utils::from_hex<32>(__z);

      auto _pk = std::string_view(pk);
      auto __pk = _pk.substr(_pk.find("="sv) + 2, _pk.size());
      auto ___pk = utils::from_hex<kyber512::PKEY_LEN>(__pk);

      auto _sk = std::string_view(sk);
      auto __sk = _sk.substr(_sk.find("="sv) + 2, _sk.size());
      auto ___sk = utils::from_hex<kyber512::SKEY_LEN>(__sk);

      auto _m = std::string_view(m);
      auto __m = _m.substr(_m.find("="sv) + 2, _m.size());
      auto ___m = utils::from_hex<32>(__m);

      auto _ct = std::string_view(ct);
      auto __ct = _ct.substr(_ct.find("="sv) + 2, _ct.size());
      auto ___ct = utils::from_hex<kyber512::CIPHER_LEN>(__ct);

      auto _ss = std::string_view(ss);
      auto __ss = _ss.substr(_ss.find("="sv) + 2, _ss.size());
      auto ___ss = utils::from_hex<32>(__ss);

      std::array<uint8_t, kyber512::PKEY_LEN> pkey{};
      std::array<uint8_t, kyber512::SKEY_LEN> skey{};
      std::array<uint8_t, kyber512::CIPHER_LEN> ctxt{};
      std::array<uint8_t, 32> shrd_sec0{};
      std::array<uint8_t, 32> shrd_sec1{};

      kyber512::keygen(___d.data(), ___z.data(), pkey.data(), skey.data());
      auto skdf = kyber512::encapsulate(___m.data(), pkey.data(), ctxt.data());
      auto rkdf = kyber512::decapsulate(skey.data(), ctxt.data());

      skdf.read(shrd_sec0.data(), shrd_sec0.size());
      rkdf.read(shrd_sec1.data(), shrd_sec1.size());

      assert(std::ranges::equal(___pk, pkey));
      assert(std::ranges::equal(___sk, skey));
      assert(std::ranges::equal(___ct, ctxt));
      assert(std::ranges::equal(___ss, shrd_sec0));
      assert(std::ranges::equal(shrd_sec0, shrd_sec1));

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

// Test if
//
// - Is Kyber768 KEM implemented correctly ?
// - Is it conformant with the specification ?
//
// using Known Answer Tests.
inline void
test_kyber768_kem_kat()
{
  const std::string kat_file = "./kats/kyber768.kat";
  std::fstream file(kat_file);

  while (true) {
    std::string d;

    if (!std::getline(file, d).eof()) {
      std::string z;
      std::string pk;
      std::string sk;
      std::string m;
      std::string ct;
      std::string ss;

      std::getline(file, z);
      std::getline(file, pk);
      std::getline(file, sk);
      std::getline(file, m);
      std::getline(file, ct);
      std::getline(file, ss);

      auto _d = std::string_view(d);
      auto __d = _d.substr(_d.find("="sv) + 2, _d.size());
      auto ___d = utils::from_hex<32>(__d);

      auto _z = std::string_view(z);
      auto __z = _z.substr(_z.find("="sv) + 2, _z.size());
      auto ___z = utils::from_hex<32>(__z);

      auto _pk = std::string_view(pk);
      auto __pk = _pk.substr(_pk.find("="sv) + 2, _pk.size());
      auto ___pk = utils::from_hex<kyber768::PKEY_LEN>(__pk);

      auto _sk = std::string_view(sk);
      auto __sk = _sk.substr(_sk.find("="sv) + 2, _sk.size());
      auto ___sk = utils::from_hex<kyber768::SKEY_LEN>(__sk);

      auto _m = std::string_view(m);
      auto __m = _m.substr(_m.find("="sv) + 2, _m.size());
      auto ___m = utils::from_hex<32>(__m);

      auto _ct = std::string_view(ct);
      auto __ct = _ct.substr(_ct.find("="sv) + 2, _ct.size());
      auto ___ct = utils::from_hex<kyber768::CIPHER_LEN>(__ct);

      auto _ss = std::string_view(ss);
      auto __ss = _ss.substr(_ss.find("="sv) + 2, _ss.size());
      auto ___ss = utils::from_hex<32>(__ss);

      std::array<uint8_t, kyber768::PKEY_LEN> pkey{};
      std::array<uint8_t, kyber768::SKEY_LEN> skey{};
      std::array<uint8_t, kyber768::CIPHER_LEN> ctxt{};
      std::array<uint8_t, 32> shrd_sec0{};
      std::array<uint8_t, 32> shrd_sec1{};

      kyber768::keygen(___d.data(), ___z.data(), pkey.data(), skey.data());
      auto skdf = kyber768::encapsulate(___m.data(), pkey.data(), ctxt.data());
      auto rkdf = kyber768::decapsulate(skey.data(), ctxt.data());

      skdf.read(shrd_sec0.data(), shrd_sec0.size());
      rkdf.read(shrd_sec1.data(), shrd_sec1.size());

      assert(std::ranges::equal(___pk, pkey));
      assert(std::ranges::equal(___sk, skey));
      assert(std::ranges::equal(___ct, ctxt));
      assert(std::ranges::equal(___ss, shrd_sec0));
      assert(std::ranges::equal(shrd_sec0, shrd_sec1));

      std::string empty_line;
      std::getline(file, empty_line);
    } else {
      break;
    }
  }

  file.close();
}

}
