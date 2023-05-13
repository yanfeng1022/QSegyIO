#ifndef SGYIO_H
#define SGYIO_H

#define LOCALFORMAT 0x00000009

const int SEGY_TEXT_HEADER_SIZE = 3200;
const int SEGY_TRACE_HEADER_SIZE = 240;

//#include "gendefs.h"
#include <string>
#include <map>
#include <tuple>

class QFile;
class QString;

//ascii code to ebcdic code
static const unsigned char a2e[256] = {
	0,  1,  2,  3,  55, 45, 46, 47, 22, 5,  37, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
	64, 79, 127,123,91, 108,80, 125,77, 93, 92, 78, 107,96, 75, 97,
	240,241,242,243,244,245,246,247,248,249,122,94, 76, 126,110,111,
	124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
	215,216,217,226,227,228,229,230,231,232,233,74, 224,90, 95, 109,
	121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
	151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,7,
	32, 33, 34, 35, 36, 21, 6,  23, 40, 41, 42, 43, 44, 9,  10, 27,
	48, 49, 26, 51, 52, 53, 54, 8,  56, 57, 58, 59, 4,  20, 62, 225,
	65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
	88, 89, 98, 99, 100,101,102,103,104,105,112,113,114,115,116,117,
	118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
	159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
	184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
	220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255
};


//ebcdic code ascii code to
static const unsigned char e2a[256] = {
	0,  1,  2,  3,  156,9,  134,127,151,141,142, 11,12, 13, 14, 15,
	16, 17, 18, 19, 157,133,8,  135,24, 25, 146,143,28, 29, 30, 31,
	128,129,130,131,132,10, 23, 27, 136,137,138,139,140,5,  6,  7,
	144,145,22, 147,148,149,150,4,  152,153,154,155,20, 21, 158,26,
	32, 160,161,162,163,164,165,166,167,168,91, 46, 60, 40, 43, 33,
	38, 169,170,171,172,173,174,175,176,177,93, 36, 42, 41, 59, 94,
	45, 47, 178,179,180,181,182,183,184,185,124,44, 37, 95, 62, 63,
	186,187,188,189,190,191,192,193,194,96, 58, 35, 64, 39, 61, 34,
	195,97, 98, 99, 100,101,102,103,104,105,196,197,198,199,200,201,
	202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
	209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
	216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
	123,65, 66, 67, 68, 69, 70, 71, 72, 73, 232,233,234,235,236,237,
	125,74, 75, 76, 77, 78, 79, 80, 81, 82, 238,239,240,241,242,243,
	92, 159,83, 84, 85, 86, 87, 88, 89, 90, 244,245,246,247,248,249,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 250,251,252,253,254,255
};

static int encode(char* dst,
	const char* src,
	const unsigned char* conv,
	size_t n) {
	for (size_t i = 0; i < n; ++i)
		dst[i] = (char)conv[(unsigned char)src[i]];

	return 0;
}

class RollHeader
{
public:
	RollHeader();
	~RollHeader();

	void setAsciiHeader(const std::string& asciiHeader);
	std::string getAsciiHeader();

	char textualHd[SEGY_TEXT_HEADER_SIZE];//textual header(3200 bytes)

	/* bhed - binary header */

	int jobid;	/* job identification number */

	int lino;	/* line number (only one line per reel) */

	int reno;	/* reel number */

	short ntrpr;	/* number of data traces per record */

	short nart;	/* number of auxiliary traces per record */

	unsigned short hdt; /* sample interval in micro secs for this reel */

	unsigned short dto; /* same for original field recording */

	unsigned short hns; /* number of samples per trace for this reel */

	unsigned short nso; /* same for original field recording */

	short format;	/* data sample format code:
					1 = floating point, 4 byte (32 bits)
					2 = fixed point, 4 byte (32 bits)
					3 = fixed point, 2 byte (16 bits)
					4 = fixed point w/gain code, 4 byte (32 bits)
					5 = IEEE floating point, 4 byte (32 bits)
					8 = two's complement integer, 1 byte (8 bits)
					*/

	short fold;	/* CDP fold expected per CDP ensemble */

	short tsort;	/* trace sorting code: 
					1 = as recorded (no sorting)
					2 = CDP ensemble
					3 = single fold continuous profile
					4 = horizontally stacked */

	short vscode;	/* vertical sum code:
					1 = no sum
					2 = two sum ...
					N = N sum (N = 32,767) */

	short hsfs;	/* sweep frequency at start */

	short hsfe;	/* sweep frequency at end */

	short hslen;	/* sweep length (ms) */

	short hstyp;	/* sweep type code:
					1 = linear
					2 = parabolic
					3 = exponential
					4 = other */

	short schn;	/* trace number of sweep channel */

	short hstas;	/* sweep trace taper length at start if
					tapered (the taper starts at zero time
					and is effective for this length) */

	short hstae;	/* sweep trace taper length at end (the ending
					taper starts at sweep length minus the taper
					length at end) */

	short htatyp;	/* sweep trace taper type code:
					1 = linear
					2 = cos-squared
					3 = other */

	short hcorr;	/* correlated data traces code:
					1 = no
					2 = yes */

	short bgrcv;	/* binary gain recovered code:
					1 = yes
					2 = no */

	short rcvm;	/* amplitude recovery method code:
				1 = none
				2 = spherical divergence
				3 = AGC
				4 = other */

	short mfeet;	/* measurement system code:
					1 = meters
					2 = feet */

	short polyt;	/* impulse signal polarity code:
					1 = increase in pressure or upward
					geophone case movement gives
					negative number on tape
					2 = increase in pressure or upward
					geophone case movement gives
					positive number on tape */

	short vpol;	/* vibratory polarity code:
				code	seismic signal lags pilot by
				1	337.5 to  22.5 degrees
				2	 22.5 to  67.5 degrees
				3	 67.5 to 112.5 degrees
				4	112.5 to 157.5 degrees
				5	157.5 to 202.5 degrees
				6	202.5 to 247.5 degrees
				7	247.5 to 292.5 degrees
				8	293.5 to 337.5 degrees */

	short hunass[170];	/* unassigned */

};


class TraceHeader
{	/* segy - trace identification header */
public:
	TraceHeader();
	~TraceHeader();


	int tracl;	/* 0 Trace sequence number within line
				--numbers continue to increase if the
				same line continues across multiple
				SEG Y files.
			 */

	int tracr;	/* 4 Trace sequence number within SEG Y file
				---each file starts with trace sequence
				one
			 */

	int fldr;	/* 8 Original field record number */

	int tracf;	/* 12 Trace number within original field record */

	int ep;		/* 16 energy source point number
				---Used when more than one record occurs
				at the same effective surface location.
			 */

	int cdp;	/* 20 Ensemble number (i.e. CDP, CMP, CRP,...) */

	int cdpt;	/* trace number within the ensemble
				---each ensemble starts with trace number one.
			 */

	short trid;	/* trace identification code:
				-1 = Other
				0 = Unknown
				1 = Seismic data
				2 = Dead
				3 = Dummy
				4 = Time break
				5 = Uphole
				6 = Sweep
				7 = Timing
				8 = Water break
				9 = Near-field gun signature
				10 = Far-field gun signature
				11 = Seismic pressure sensor
				12 = Multicomponent seismic sensor
				- Vertical component
				13 = Multicomponent seismic sensor
				- Cross-line component 
				14 = Multicomponent seismic sensor
				- in-line component 
				15 = Rotated multicomponent seismic sensor
				- Vertical component
				16 = Rotated multicomponent seismic sensor
				- Transverse component
				17 = Rotated multicomponent seismic sensor
				- Radial component
				18 = Vibrator reaction mass
				19 = Vibrator baseplate
				20 = Vibrator estimated ground force
				21 = Vibrator reference
				22 = Time-velocity pairs
				23 ... N = optional use 
				(maximum N = 32,767)

				Following are CWP id flags:

				109 = autocorrelation
				110 = Fourier transformed - no packing
				xr[0],xi[0], ..., xr[N-1],xi[N-1]
				111 = Fourier transformed - unpacked Nyquist
				xr[0],xi[0],...,xr[N/2],xi[N/2]
				112 = Fourier transformed - packed Nyquist
				even N:
				xr[0],xr[N/2],xr[1],xi[1], ...,
				xr[N/2 -1],xi[N/2 -1]
				(note the exceptional second entry)
				odd N:
				xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
				xr[(N-1)/2 -1],xi[(N-1)/2 -1],xi[(N-1)/2]
				(note the exceptional second & last entries)
				113 = Complex signal in the time domain
				xr[0],xi[0], ..., xr[N-1],xi[N-1]
				114 = Fourier transformed - amplitude/phase
				a[0],p[0], ..., a[N-1],p[N-1]
				115 = Complex time signal - amplitude/phase
				a[0],p[0], ..., a[N-1],p[N-1]
				116 = Real part of complex trace from 0 to Nyquist
				117 = Imag part of complex trace from 0 to Nyquist
				118 = Amplitude of complex trace from 0 to Nyquist
				119 = Phase of complex trace from 0 to Nyquist
				121 = Wavenumber time domain (k-t)
				122 = Wavenumber frequency (k-omega)
				123 = Envelope of the complex time trace
				124 = Phase of the complex time trace
				125 = Frequency of the complex time trace
				130 = Depth-Range (z-x) traces
				201 = Seismic data packed to bytes (by supack1)
				202 = Seismic data packed to 2 bytes (by supack2)
				*/

	short nvs;	/* Number of vertically summed traces yielding
				this trace. (1 is one trace, 
				2 is two summed traces, etc.)
			 */

	short nhs;	/* Number of horizontally summed traces yielding
				this trace. (1 is one trace
				2 is two summed traces, etc.)
			 */

	short duse;	/* Data use:
				1 = Production
				2 = Test
			 */

	int offset;	/* Distance from the center of the source point 
				to the center of the receiver group 
				(negative if opposite to direction in which 
				the line was shot).
			 */

	int gelev;	/* Receiver group elevation from sea level
				(all elevations above the Vertical datum are 
				positive and below are negative).
			 */

	int selev;	/* Surface elevation at source. */

	int sdepth;	/* Source depth below surface (a positive number). */

	int gdel;	/* Datum elevation at receiver group. */

	int sdel;	/* Datum elevation at source. */

	int swdep;	/* Water depth at source. */

	int gwdep;	/* Water depth at receiver group. */

	short scalel;	/* Scalar to be applied to the previous 7 entries
					to give the real value. 
					Scalar = 1, +10, +100, +1000, +10000.
					If positive, scalar is used as a multiplier,
					if negative, scalar is used as a divisor.
					*/

	short scalco;	/* Scalar to be applied to the next 4 entries
					to give the real value. 
					Scalar = 1, +10, +100, +1000, +10000.
					If positive, scalar is used as a multiplier,
					if negative, scalar is used as a divisor.
					*/

	int  sx;	/* Source coordinate - X */

	int  sy;	/* Source coordinate - Y */

	int  gx;	/* Group coordinate - X */

	int  gy;	/* Group coordinate - Y */

	short counit;	/* Coordinate units: (for previous 4 entries and
					for the 7 entries before scalel)
					1 = Length (meters or feet)
					2 = Seconds of arc
					3 = Decimal degrees
					4 = Degrees, minutes, seconds (DMS)

					In case 2, the X values are longitude and 
					the Y values are latitude, a positive value designates
					the number of seconds east of Greenwich
					or north of the equator

					In case 4, to encode +-DDDMMSS
					counit = +-DDD*10^4 + MM*10^2 + SS,
					with scalco = 1. To encode +-DDDMMSS.ss
					counit = +-DDD*10^6 + MM*10^4 + SS*10^2 
					with scalco = -100.
					*/

	short wevel;	/* Weathering velocity. */

	short swevel;	/* Subweathering velocity. */

	short sut;	/* Uphole time at source in milliseconds. */

	short gut;	/* Uphole time at receiver group in milliseconds. */

	short sstat;	/* Source static correction in milliseconds. */

	short gstat;	/* Group static correction  in milliseconds.*/

	short tstat;	/* Total static applied  in milliseconds.
					(Zero if no static has been applied.)
					*/

	short laga;	/* Lag time A, time in ms between end of 240-
				byte trace identification header and time
				break, positive if time break occurs after
				end of header, time break is defined as
				the initiation pulse which maybe recorded
				on an auxiliary trace or as otherwise
				specified by the recording system */

	short lagb;	/* lag time B, time in ms between the time break
				and the initiation time of the energy source,
				may be positive or negative */

	short delrt;	/* delay recording time, time in ms between
					initiation time of energy source and time
					when recording of data samples begins
					(for deep water work if recording does not
					start at zero time) */

	short muts;	/* mute time--start */

	short mute;	/* mute time--end */

	unsigned short ns;	/* number of samples in this trace */

	unsigned short dt;	/* sample interval; in micro-seconds */

	short gain;	/* gain type of field instruments code:
				1 = fixed
				2 = binary
				3 = floating point
				4 ---- N = optional use */

	short igc;	/* instrument gain constant */

	short igi;	/* instrument early or initial gain */

	short corr;	/* correlated:
				1 = no
				2 = yes */

	short sfs;	/* sweep frequency at start */

	short sfe;	/* sweep frequency at end */

	short slen;	/* sweep length in ms */

	short styp;	/* sweep type code:
				1 = linear
				2 = cos-squared
				3 = other */

	short stas;	/* sweep trace length at start in ms */

	short stae;	/* sweep trace length at end in ms */

	short tatyp;	/* taper type: 1=linear, 2=cos^2, 3=other */

	short afilf;	/* alias filter frequency if used */

	short afils;	/* alias filter slope */

	short nofilf;	/* notch filter frequency if used */

	short nofils;	/* notch filter slope */

	short lcf;	/* low cut frequency if used */

	short hcf;	/* high cut frequncy if used */

	short lcs;	/* low cut slope */

	short hcs;	/* high cut slope */

	short year;	/* year data recorded */

	short day;	/* day of year */

	short hour;	/* hour of day (24 hour clock) */

	short minute;	/* minute of hour */

	short sec;	/* second of minute */

	short timbas;	/* time basis code:
					1 = local
					2 = GMT
					3 = other */

	short trwf;	/* trace weighting factor, defined as 1/2^N
				volts for the least sigificant bit */

	short grnors;	/* geophone group number of roll switch
					position one */

	short grnofr;	/* geophone group number of trace one within
					original field record */

	short grnlof;	/* geophone group number of last trace within
					original field record */

	short gaps;	/* gap size (total number of groups dropped) */

	short otrav;	/* overtravel taper code:
					1 = down (or behind)
					2 = up (or ahead) */
	int cdpx;  /* 181-184 x coordinate of ensemble (cdp) position of the trace */

	int cdpy;  /* 185-188 y coordinate of ensemble (cdp) position of the trace */

	int iline;  /* 189-192 for a 3d poststack data, this field should be used for the 
				in-line number, if one in-line per segy file is being recoreded,
				this value should be same for traces in the file, and the same 
				value will be recoed in the bytes 3205-3208 of the binary file header */
	int xline; /* 193-196 for a 3d postack data, this field should be used for the cross-line
			   number, this will typically be the same value as the ensemble (CDP) in
			   trace header bytes 21-24, but this does not have to be the case */

	short sp;  /* 97-198 shotpoint number */

	short spscal;  /* scale to be applied to shotpoint number in field 76*/

	short tvmu;  /* time value measurement uint */

	int trdman;  /* transduction constant mantissa */

	short trdexp;  /* transduction constant exponent */

	short trdun;  /* transductio units */
	
	short dti;  /* device/trace identifier */

	short timscal;  /* scalar to be applied to times in fields 29-38 and 
					any extension in SEG -Y trace header SEG00001*/

	short stypor;  /* source type/orientation */

	short sedir[3];  /* source energy direction with respect to the source orientation */

	int smman;  /* source measurement mantissa */

	short smexp;  /* source measurement exponent */

	short smun;   /* source measurement unit */

	int unass[2];  //unassigned

};


class SgyIO
{
public:

    enum ByteOrder
    {
		LittleEndian = 1,
		BigEndian = 2 
    };

    enum SampleDataFormat
    {
		IBMFloat = 4,
		IEEEFloat = 8,
		FourBytesInteger = 16,
		TwoBytesInteger = 32,
		OneByteInteger = 64
    };

    SgyIO();
    virtual ~SgyIO();

    virtual  bool setFile(const QString &fileName) = 0;
    int format() const;
    void setFormat(int format);

    /* about format */
    inline int byteOrder() const;
    inline int sampleDataFormat() const;


 
protected:
    void swapRollHeader(RollHeader &rollHeader) const;
    void swapTraceHeader(TraceHeader &traceHeader) const;
    void swapTraceData(float *data, int n) const;
    void swapTraceData(int *data, int n) const;
    void swapTraceData(short *data, int n) const;
    void ibm2ieee(int *from, int *to, int n) const;
    void ieee2ibm(int *frmo, int *to, int n) const;
    void swapBytes(void *p, int n) const;//sometime we need swap just one variable ,althrough we can declare it private .

protected:
    QFile *m_file;
    int m_format;

    bool m_needSwap;
    bool m_needFloatConversion;

};

inline int SgyIO::byteOrder() const
{
    return m_format & 0x00000003;
}

inline int SgyIO::sampleDataFormat() const
{
    return m_format & 0x0000007c;
}

#endif
