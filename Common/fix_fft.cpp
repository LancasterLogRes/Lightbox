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

#define LOG2_N_WAVE 11       /* log2(N_WAVE) */
#define N_WAVE      (1 << LOG2_N_WAVE)     /* full length of Sinewave[] */
#define WAVE_BITS   (N_WAVE * 2 - 1)
// Generate with:
// perl -e '$n = 2048/4; for ($i = 0; $i < $n; ++$i) { print int(sin($i*3.1415926535898/2.0/$n) * 32767).($i % 16 != 15 ? ", " : ",\n"); } print "\n";'
int SinTab[N_WAVE] = {
	0, 100, 201, 301, 402, 502, 603, 703, 804, 904, 1005, 1105, 1206, 1306, 1406, 1507,
	1607, 1708, 1808, 1908, 2009, 2109, 2209, 2310, 2410, 2510, 2610, 2711, 2811, 2911, 3011, 3111,
	3211, 3311, 3411, 3511, 3611, 3711, 3811, 3911, 4011, 4110, 4210, 4310, 4409, 4509, 4608, 4708,
	4807, 4907, 5006, 5106, 5205, 5304, 5403, 5502, 5601, 5700, 5799, 5898, 5997, 6096, 6195, 6293,
	6392, 6491, 6589, 6688, 6786, 6884, 6982, 7081, 7179, 7277, 7375, 7473, 7571, 7668, 7766, 7864,
	7961, 8059, 8156, 8253, 8351, 8448, 8545, 8642, 8739, 8836, 8932, 9029, 9126, 9222, 9319, 9415,
	9511, 9607, 9703, 9799, 9895, 9991, 10087, 10182, 10278, 10373, 10469, 10564, 10659, 10754, 10849, 10944,
	11038, 11133, 11227, 11322, 11416, 11510, 11604, 11698, 11792, 11886, 11980, 12073, 12166, 12260, 12353, 12446,
	12539, 12632, 12724, 12817, 12909, 13002, 13094, 13186, 13278, 13370, 13462, 13553, 13645, 13736, 13827, 13918,
	14009, 14100, 14191, 14281, 14372, 14462, 14552, 14642, 14732, 14822, 14911, 15001, 15090, 15179, 15268, 15357,
	15446, 15534, 15623, 15711, 15799, 15887, 15975, 16063, 16150, 16238, 16325, 16412, 16499, 16586, 16672, 16759,
	16845, 16931, 17017, 17103, 17189, 17274, 17360, 17445, 17530, 17615, 17699, 17784, 17868, 17952, 18036, 18120,
	18204, 18287, 18371, 18454, 18537, 18620, 18702, 18785, 18867, 18949, 19031, 19113, 19194, 19276, 19357, 19438,
	19519, 19599, 19680, 19760, 19840, 19920, 20000, 20079, 20159, 20238, 20317, 20396, 20474, 20553, 20631, 20709,
	20787, 20864, 20942, 21019, 21096, 21173, 21249, 21326, 21402, 21478, 21554, 21629, 21705, 21780, 21855, 21930,
	22004, 22079, 22153, 22227, 22301, 22374, 22448, 22521, 22594, 22666, 22739, 22811, 22883, 22955, 23027, 23098,
	23169, 23240, 23311, 23382, 23452, 23522, 23592, 23661, 23731, 23800, 23869, 23938, 24006, 24075, 24143, 24211,
	24278, 24346, 24413, 24480, 24546, 24613, 24679, 24745, 24811, 24877, 24942, 25007, 25072, 25136, 25201, 25265,
	25329, 25392, 25456, 25519, 25582, 25645, 25707, 25769, 25831, 25893, 25954, 26016, 26077, 26137, 26198, 26258,
	26318, 26378, 26437, 26497, 26556, 26615, 26673, 26731, 26789, 26847, 26905, 26962, 27019, 27076, 27132, 27188,
	27244, 27300, 27355, 27411, 27466, 27520, 27575, 27629, 27683, 27736, 27790, 27843, 27896, 27948, 28001, 28053,
	28105, 28156, 28208, 28259, 28309, 28360, 28410, 28460, 28510, 28559, 28608, 28657, 28706, 28754, 28802, 28850,
	28897, 28945, 28992, 29038, 29085, 29131, 29177, 29222, 29268, 29313, 29358, 29402, 29446, 29490, 29534, 29577,
	29621, 29663, 29706, 29748, 29790, 29832, 29873, 29915, 29955, 29996, 30036, 30076, 30116, 30156, 30195, 30234,
	30272, 30311, 30349, 30386, 30424, 30461, 30498, 30535, 30571, 30607, 30643, 30678, 30713, 30748, 30783, 30817,
	30851, 30885, 30918, 30951, 30984, 31017, 31049, 31081, 31113, 31144, 31175, 31206, 31236, 31267, 31297, 31326,
	31356, 31385, 31413, 31442, 31470, 31498, 31525, 31553, 31580, 31606, 31633, 31659, 31684, 31710, 31735, 31760,
	31785, 31809, 31833, 31856, 31880, 31903, 31926, 31948, 31970, 31992, 32014, 32035, 32056, 32077, 32097, 32117,
	32137, 32156, 32176, 32194, 32213, 32231, 32249, 32267, 32284, 32301, 32318, 32334, 32350, 32366, 32382, 32397,
	32412, 32426, 32441, 32455, 32468, 32482, 32495, 32508, 32520, 32532, 32544, 32556, 32567, 32578, 32588, 32599,
	32609, 32618, 32628, 32637, 32646, 32654, 32662, 32670, 32678, 32685, 32692, 32699, 32705, 32711, 32717, 32722,
	32727, 32732, 32736, 32740, 32744, 32748, 32751, 32754, 32757, 32759, 32761, 32763, 32764, 32765, 32766, 32766
};

inline int Sin(uint16_t d)
{
	if (d < N_WAVE / 4)
		return SinTab[d];
	if (d < N_WAVE / 2)
		return SinTab[N_WAVE / 2 - 1 - d];
	if (d < N_WAVE * 3 / 4)
		return -SinTab[d - N_WAVE / 2];
	return -SinTab[N_WAVE - 1 - d];
}

inline int Cos(uint16_t d)
{
	if (d < N_WAVE / 4)
		return SinTab[N_WAVE / 4 - 1 - d];
	if (d < N_WAVE / 2)
		return -SinTab[d - N_WAVE / 4];
	if (d < N_WAVE * 3 / 4)
		return -SinTab[N_WAVE * 3 / 4 - 1 - d];
	return SinTab[d - N_WAVE * 3 / 4];
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

			wr =  Cos(j & WAVE_BITS);
			wi = -Sin(j & WAVE_BITS);

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
