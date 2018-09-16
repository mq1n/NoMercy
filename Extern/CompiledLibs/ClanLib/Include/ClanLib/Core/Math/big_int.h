/*
**  ClanLib SDK
**  Copyright (c) 1997-2016 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Mark Page
**    Michael J. Fromberger
*/

// This class is based on the original MPI library (not NSS, because of license restrictions) with some modifications.
// Some ideas and algorithms are from NSS (Netscape Security Suite). Where they have been used, the function contains a reference note
//
// Note, since September 2011, I believe the MPI homepage is now: http://spinning-yarns.org/michael/mpi/
// Note, since 2013, the MPI page no longer exists, however the internet archive has the details here: http://web.archive.org/web/20100426001741/http://spinning-yarns.org/michael/mpi/README
// The license is as follows
// This software was written by Michael J. Fromberger,
//   http://www.dartmouth.edu/~sting/
//
// See the MPI home page at
//   http://www.dartmouth.edu/~sting/mpi/
//
// This software is in the public domain.  It is entirely free, and you
// may use it and/or redistribute it for whatever purpose you choose;
// however, as free software, it is provided without warranty of any
// kind, not even the implied warranty of merchantability or fitness for
// a particular purpose.


#pragma once

#include "../System/cl_platform.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class BigInt_Impl;

	/// \brief Big Integer class
	class BigInt
	{
	public:
		/// \brief Constructs a big integer (initialised to zero)
		BigInt();

		/// \brief Constructs a big integer (initialised to value)
		explicit BigInt(uint32_t value);

		/// \brief Constructs a big integer (initialised to value)
		explicit BigInt(int32_t value);

		/// \brief Constructs a big integer (initialised to value)
		explicit BigInt(uint64_t value);

		/// \brief Constructs a big integer (initialised to value)
		explicit BigInt(int64_t value);

		/// \brief Copy constructor
		BigInt(const BigInt &other);

		/// \brief Destructor
		~BigInt();

		BigInt &operator=(const BigInt& other);

		void read_unsigned_octets(const unsigned char *input_str, unsigned int input_length);

		void zero();

		bool make_prime(unsigned int num_bits);

		/// \brief  Compare a <=> 0.  Returns <0 if a<0, 0 if a=0, >0 if a>0.
		int cmp_z() const;

		void set_bit(unsigned int bit_number, unsigned int value);

		int significant_bits() const;

		void sieve(const uint32_t *primes, unsigned int num_primes, std::vector<unsigned char> &sieve);

		/// \brief  Compute c = a (mod d).  Result will always be 0 <= c < d
		uint32_t mod_d(uint32_t d) const;

		/// \brief  Compute the quotient q = a / d and remainder r = a mod d, for a single digit d.  Respects the sign of its divisor (single digits are unsigned anyway).
		void div_d(uint32_t d, BigInt *q, uint32_t *r) const;

		/// \brief  Using w as a witness, try pseudo-primality testing based on Fermat's little theorem. 
		///
		/// If a is prime, and (w, a) = 1, then w^a == w (mod a).
		/// So, we compute z = w^a (mod a) and compare z to w; if they are
		/// equal, the test passes and we return true.  Otherwise, we return false.
		bool fermat(uint32_t w) const;

		/// \brief  Performs nt iteration of the Miller-Rabin probabilistic primality test on a.
		///
		/// Returns true if the tests pass, false if one fails.
		/// If false is returned, the number is definitely composite.  If true
		bool pprime(int nt) const;

		/// \brief Sets a value
		void set(int32_t d);
		void set(uint32_t d);
		void set(uint64_t d);
		void set(int64_t d);

		/// \brief Gets a value.
		///
		/// Throws exception if number exceeds datatype bounds
		void get(uint32_t &d);
		void get(uint64_t &d);
		void get(int64_t &d);
		void get(int32_t &d);

		/// \brief  Compute c = (a ** b) mod m.
		///
		/// Uses a standard square-and-multiply
		/// method with modular reductions at each step. (This is basically the
		/// same code as expt(), except for the addition of the reductions)
		///
		/// The modular reductions are done using Barrett's algorithm (see reduce() for details)
		void exptmod(const BigInt *b, const BigInt *m, BigInt *c) const;

		/// \brief  Compute c = a (mod m).  Result will always be 0 <= c < m.
		void mod(const BigInt *m, BigInt *c) const;

		/// \brief  Compute q = a / b and r = a mod b.
		///
		/// Input parameters may be re-used
		/// as output parameters.  If q or r is NULL, that portion of the
		/// computation will be discarded (although it will still be computed)
		void div(const BigInt &b, BigInt *q, BigInt *r) const;
		void div(uint32_t d, BigInt *q, BigInt *r) const;

		/// \brief Compute result = this + b.
		BigInt operator + (const BigInt& b);
		BigInt operator + (uint32_t d);

		/// \brief Compute this += b.
		BigInt operator += (const BigInt& b);
		BigInt operator += (uint32_t d);

		/// \brief Compute result = this - b.
		BigInt operator - (const BigInt& b);
		BigInt operator - (uint32_t d);

		/// \brief Compute this -= b.
		BigInt operator -= (const BigInt& b);
		BigInt operator -= (uint32_t d);

		/// \brief Compute result = this * b.
		BigInt operator * (const BigInt& b);
		BigInt operator * (uint32_t d);

		/// \brief Compute this *= b.
		BigInt operator *= (const BigInt& b);
		BigInt operator *= (uint32_t d);

		/// \brief Compute result = this / b.
		BigInt operator / (const BigInt& b);
		BigInt operator / (uint32_t d);

		/// \brief Compute this /= b.
		BigInt operator /= (const BigInt& b);
		BigInt operator /= (uint32_t d);

		/// \brief Compute result = this % b.
		BigInt operator % (const BigInt& b);
		BigInt operator % (uint32_t d);

		/// \brief Compute this %= b.
		BigInt operator %= (const BigInt& b);
		BigInt operator %= (uint32_t d);

		int cmp(const BigInt *b) const;

		/// \brief  Compare a <=> d.  Returns <0 if a<d, 0 if a=d, >0 if a>d
		int cmp_d(uint32_t d) const;

		/// \brief  Compute b = -a.  'a' and 'b' may be identical.
		void neg(BigInt *b) const;

		unsigned int trailing_zeros() const;

		void sqrmod(const BigInt *m, BigInt *c) const;
		void sqr(BigInt *b) const;

		/// \brief Assigns a random value to a.
		///
		/// This value is generated using the
		/// standard C library's rand() function, so it should not be used for
		/// cryptographic purposes, but it should be fine for primality testing,
		/// since all we really care about there is reasonable statistical
		/// properties.
		/// As many digits as a currently has are filled with random digits.
		void random();

		/// \brief Exchange mp1 and mp2 without allocating any intermediate memory
		///
		/// (well, unless you count the stack space needed for this call and the
		/// locals it creates...).  This cannot fail.
		void exch(BigInt *mp2);

		/// \brief  Compute c = a^-1 (mod m), if there is an inverse for a (mod m).
		///
		/// This is equivalent to the question of whether (a, m) = 1.  If not,
		/// false is returned, and there is no inverse.
		bool invmod(const BigInt *m, BigInt *c) const;

		/// \brief  Compute g = (a, b) and values x and y satisfying Bezout's identity
		///
		/// (that is, ax + by = g).  This uses the extended binary GCD algorithm
		/// based on the Stein algorithm used for mp_gcd()
		void xgcd(const BigInt *b, BigInt *g, BigInt *x, BigInt *y) const;

		/// \brief   Compute b = |a|.  'a' and 'b' may be identical.
		void abs(BigInt *b) const;

		///  \brief  Returns a true if number is even
		bool is_even() const;

		///  \brief  Returns a true if number is odd
		bool is_odd() const;

		/// \brief  Compute c = a / 2, disregarding the remainder.
		void div_2(BigInt *c) const;

		void to_unsigned_octets(unsigned char *output_str, unsigned int output_length) const;

		int unsigned_octet_size() const;

	private:
		std::unique_ptr<BigInt_Impl> impl;
	};

	/// \}
}
