/* fix_fft.c - Fixed-point in-place Fast Fourier Transform  */
/*
  All data are fixed-point int16_t integers, in which -32768
  to +32768 represent -1.0 to +1.0 respectively. Integer
  arithmetic is used for speed, instead of the more natural
  floating-point.

  For the forward FFT (time -> freq), fixed scaling is
  performed to prevent arithmetic overflow, and to map a 0dB
  sine/cosine wave (i.e. amplitude = 32767) to two -6dB freq
  coefficients. The return value is always 0.

  For the inverse FFT (freq -> time), fixed scaling cannot be
  done, as two 0dB coefficients would sum to a peak amplitude
  of 64K, overflowing the 32k range of the fixed-point integers.
  Thus, the fix_fft() routine performs variable scaling, and
  returns a value which is the number of bits LEFT by which
  the output must be shifted to get the actual amplitude
  (i.e. if fix_fft() returns 3, each value of fr[] and fi[]
  must be multiplied by 8 (2**3) for proper scaling.
  Clearly, this cannot be done within fixed-point int16_t
  integers. In practice, if the result is to be used as a
  filter, the scale_shift can usually be ignored, as the
  result will be approximately correctly normalized as is.

  Written by:  Tom Roberts  11/8/89
  Made portable:  Malcolm Slaney 12/15/94 malcolm@interval.com
  Enhanced:  Dimitrios P. Bouras  14 Jun 2006 dbouras@ieee.org
*/

/*
  Henceforth "int16_t" implies 16-bit word. If this is not
  the case in your architecture, please replace "int16_t"
  with a type definition which *is* a 16-bit word.
*/

/* MODS by l3v3rz for ATMEL 128*/

#include <cstdint>

#define N_WAVE      256     /* full length of Sinewave[] */
#define LOG2_N_WAVE 8       /* log2(N_WAVE) */

int SinTab[] = {
		0,
	  804,  1608,  2410,  3212,  4011,  4808,  5602,  6393,
	 7179,  7962,  8739,  9512, 10278, 11039, 11793, 12539,
	13279, 14010, 14732, 15446, 16151, 16846, 17530, 18204,
	18868, 19519, 20159, 20787, 21403, 22005, 22594, 23170,
	23732, 24279, 24812, 25329, 25832, 26319, 26790, 27245,
	27684, 28106, 28511, 28898, 29269, 29621, 29956, 30273,
	30572, 30852, 31114, 31357, 31581, 31786, 31972, 32138,
	32285, 32413, 32521, 32610, 32679, 32728, 32758};

inline int Sin(uint8_t d)
{
	if (d<64)
		return SinTab[d];
	if (d<128)
		return SinTab[127-d];
	if (d<192)
		return -SinTab[d-128];
	return -SinTab[255-d];
}

inline int Cos(uint8_t d)
{
	if (d<64)
		return SinTab[63-d];
	if (d<128)
		return -SinTab[d-64];
	if (d<192)
		return -SinTab[191-d];
	return SinTab[d-192];
}

/*
  FIX_MPY() - fixed-point multiplication & scaling.
  Substitute inline assembly for hardware-specific
  optimization suited to a particluar DSP processor.
  Scaling ensures that result remains 16-bit.
*/
inline int16_t FIX_MPY(int16_t a, int16_t b)
{
	/* shift right one less bit (i.e. 15-1) */
	int32_t c = ((int32_t)a * (int32_t)b) >> 14;
	/* last bit shifted out = rounding-bit */
	b = c & 0x01;
	/* last shift + rounding bit */
	a = (c >> 1) + b;
	return a;
}

/*
  fix_fft() - perform forward/inverse fast Fourier transform.
  fr[n],fi[n] are real and imaginary arrays, both INPUT AND
  RESULT (in-place FFT), with 0 <= n < 2**m; set inverse to
  0 for forward transform (FFT), or 1 for iFFT.
*/
int fix_fft(int16_t fr[], int16_t fi[], int16_t m, int16_t inverse)
{
	long mr, nn, i, j, l, k, istep, n, scale, shift;
	int16_t qr, qi, tr, ti, wr, wi;

	n = 1 << m;

	/* max FFT size = N_WAVE */
	if (n > N_WAVE)
		return -1;

	mr = 0;
	nn = n - 1;
	scale = 0;

	/* decimation in time - re-order data */
	for (m=1; m<=nn; ++m) {
		l = n;
		do {
			l >>= 1;
		} while (mr+l > nn);
		mr = (mr & (l-1)) + l;

		if (mr <= m)
			continue;
		tr = fr[m];
		fr[m] = fr[mr];
		fr[mr] = tr;
		ti = fi[m];
		fi[m] = fi[mr];
		fi[mr] = ti;
	}

	l = 1;
	k = LOG2_N_WAVE-1;
	while (l < n) {
		if (inverse) {
			/* variable scaling, depending upon data */
			shift = 0;
			for (i=0; i<n; ++i) {
				j = fr[i];
				if (j < 0)
					j = -j;
				m = fi[i];
				if (m < 0)
					m = -m;
				if (j > 16383 || m > 16383) {
					shift = 1;
					break;
				}
			}
			if (shift)
				++scale;
		} else {
			/*
			  fixed scaling, for proper normalization --
			  there will be log2(n) passes, so this results
			  in an overall factor of 1/n, distributed to
			  maximize arithmetic accuracy.
			*/
			shift = 1;
		}
		/*
		  it may not be obvious, but the shift will be
		  performed on each data point exactly once,
		  during this pass.
		*/
		istep = l << 1;
		for (m=0; m<l; ++m) {
			j = m << k;
			/* 0 <= j < N_WAVE/2 */
			//wr =  Sinewave[j+N_WAVE/4];
			//wi = -Sinewave[j];

			wr =  Cos((uint8_t)j);
			wi = -Sin((uint8_t)j);

			if (inverse)
				wi = -wi;
			if (shift) {
				wr >>= 1;
				wi >>= 1;
			}
			for (i=m; i<n; i+=istep) {
				j = i + l;
				tr = FIX_MPY(wr,fr[j]) - FIX_MPY(wi,fi[j]);
				ti = FIX_MPY(wr,fi[j]) + FIX_MPY(wi,fr[j]);
				qr = fr[i];
				qi = fi[i];
				if (shift) {
					qr >>= 1;
					qi >>= 1;
				}
				fr[j] = qr - tr;
				fi[j] = qi - ti;
				fr[i] = qr + tr;
				fi[i] = qi + ti;
			}
		}
		--k;
		l = istep;
	}
	return scale;
}

/*
  fix_fftr() - forward/inverse FFT on array of real numbers.
  Real FFT/iFFT using half-size complex FFT by distributing
  even/odd samples into real/imaginary arrays respectively.
  In order to save data space (i.e. to avoid two arrays, one
  for real, one for imaginary samples), we proceed in the
  following two steps: a) samples are rearranged in the real
  array so that all even samples are in places 0-(N/2-1) and
  all imaginary samples in places (N/2)-(N-1), and b) fix_fft
  is called with fr and fi pointing to index 0 and index N/2
  respectively in the original array. The above guarantees
  that fix_fft "sees" consecutive real samples as alternating
  real and imaginary samples in the complex array.
*/
int fix_fftr(int16_t f[], int m, int inverse)
{
	int i, N = 1<<(m-1), scale = 0;
	int16_t tt, *fr=f, *fi=&f[N];

	if (inverse)
		scale = fix_fft(fi, fr, m-1, inverse);
	for (i=1; i<N; i+=2) {
		tt = f[N+i-1];
		f[N+i-1] = f[i];
		f[i] = tt;
	}
	if (! inverse)
		scale = fix_fft(fi, fr, m-1, inverse);
	return scale;
}
