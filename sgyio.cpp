#include "sgyio.h"
#include <qfile.h>
#include <iostream>
#include <tuple>

using namespace std;

/* roll header */
RollHeader::RollHeader()
:jobid(0),
lino(0),
reno(0),
ntrpr(1),
nart(1),
hdt(2000),
dto(0),
hns(1000),
nso(0),
format(5),
fold(1),
tsort(1),
vscode(0),
hsfs(0),
hsfe(0),
hslen(0),
hstyp(0),
schn(0),
hstae(0),
htatyp(0),
hcorr(1),
bgrcv(2),
rcvm(1),
mfeet(1),
polyt(1),
vpol(1)
{
	for(int i = 0; i < SEGY_TEXT_HEADER_SIZE; i++)
		textualHd[i] = '\0';

	for(int i = 0; i < 170; i++)
		hunass[i] = 0;
} 

RollHeader::~RollHeader()
{
}

void RollHeader::setAsciiHeader(const std::string& asciiHeader)
{
    std::string txtHeader;
    txtHeader.resize(SEGY_TEXT_HEADER_SIZE);
    if (asciiHeader.size() > SEGY_TEXT_HEADER_SIZE) {
        txtHeader = asciiHeader.substr(0, SEGY_TEXT_HEADER_SIZE);
    }
    else
    {
        for(int i=0;i<asciiHeader.size();i++)
            txtHeader[i] = asciiHeader[i];
    }

    encode(textualHd, txtHeader.c_str(), a2e, SEGY_TEXT_HEADER_SIZE);
}

std::string RollHeader::getAsciiHeader()
{
    char txthead[SEGY_TEXT_HEADER_SIZE];
    encode(txthead, textualHd, e2a, SEGY_TEXT_HEADER_SIZE);

    std::string asciiHeader = txthead;
    asciiHeader.resize(SEGY_TEXT_HEADER_SIZE);
    return asciiHeader;
}

/* trace header */
TraceHeader::TraceHeader()
:tracl(1),
tracr(1),
fldr(1),
tracf(1),
ep(0),
cdp(1),
cdpt(1),
trid(1),
nvs(0),
nhs(0),
duse(1),
offset(0),
gelev(0),
selev(0),
sdepth(0),
gdel(0),
sdel(0),
swdep(0),
gwdep(0),
scalel(1),
scalco(1),
sx(0),
sy(0),
gx(0),
gy(0),
counit(1),
wevel(0),
swevel(0),
sut(0),
gut(0),
sstat(0),
gstat(0),
tstat(0),
laga(0),
lagb(0),
delrt(0),
muts(0),
mute(0),
ns(1000),
dt(2000),
gain(1),
igc(0),
igi(0),
corr(1),
sfs(0),
sfe(0),
slen(0),
styp(1),
stas(0),
stae(0),
tatyp(0),
afilf(0),
afils(0),
nofilf(0),
nofils(0),
lcf(0),
hcf(0),
lcs(0),
hcs(0),
year(0),
day(0),
hour(0),
minute(0),
sec(0),
timbas(1),
trwf(0),
grnors(0),
gaps(0),
otrav(1),
cdpx(0),
cdpy(0),
iline(0),
xline(0),
sp(0),
spscal(0),
tvmu(0),
trdman(0),
trdexp(0),
trdun(0),
dti(0),
timscal(0),
stypor(0),
smman(0),
smexp(0),
smun(0)
{
    sedir[0] = 0;
    sedir[1] = 0;
    sedir[2] = 0;
	for(int i = 0; i < 2; i++)
		unass[i] = 0;
}

TraceHeader::~TraceHeader()
{
}


/* sgy io */
SgyIO::SgyIO()
    : m_format(0),
    m_file(NULL),
    m_needSwap(false),
    m_needFloatConversion(false)
{
}

SgyIO::~SgyIO()
{
    if(m_file != NULL)
    {
	    if(m_file->isOpen())
	        m_file->close();
	    delete m_file;
    }
    /* note: Although delete a NULL pointer is allowed, if m_file is NULL, m_file->isOpen() is illegal.
    so here we must check the m_file if it is NULL .*/
}

int SgyIO::format() const
{
    return m_format;
}

void SgyIO::setFormat(int format)
{
    if(m_format != format)
    {
	m_format = format;

	m_needSwap = (LOCALFORMAT & 0x00000003) != (m_format & 0x00000003) ? true : false;

	int LFFormat = LOCALFORMAT & 0x0000007c;//local float format
	int FSFormat = m_format & 0x0000007c;//file sample data format
        
        if((FSFormat == IBMFloat) || (FSFormat == IEEEFloat))
            m_needFloatConversion = LFFormat != FSFormat ? true : false;	    
    }
}

void SgyIO::swapRollHeader(RollHeader &rollHeader) const
{
    swapBytes(&rollHeader.jobid, 4);
    swapBytes(&rollHeader.lino, 4);
    swapBytes(&rollHeader.reno, 4);
    swapBytes(&rollHeader.ntrpr, 2);
    swapBytes(&rollHeader.nart, 2);
    swapBytes(&rollHeader.hdt, 2);
    swapBytes(&rollHeader.dto, 2);
    swapBytes(&rollHeader.hns, 2);
    swapBytes(&rollHeader.nso, 2);
    swapBytes(&rollHeader.format, 2);
    swapBytes(&rollHeader.fold, 2);
    swapBytes(&rollHeader.tsort, 2);
    swapBytes(&rollHeader.vscode, 2);
    swapBytes(&rollHeader.hsfs, 2);
    swapBytes(&rollHeader.hsfe, 2);
    swapBytes(&rollHeader.hstyp, 2);
    swapBytes(&rollHeader.schn, 2);
    swapBytes(&rollHeader.hstas, 2);
    swapBytes(&rollHeader.hstae, 2);
    swapBytes(&rollHeader.htatyp, 2);
    swapBytes(&rollHeader.hcorr, 2);
    swapBytes(&rollHeader.bgrcv, 2);
    swapBytes(&rollHeader.rcvm, 2);
    swapBytes(&rollHeader.mfeet, 2);
    swapBytes(&rollHeader.polyt, 2);
    swapBytes(&rollHeader.vpol, 2);
}

void SgyIO::swapTraceHeader(TraceHeader &traceHeader) const
{
    swapBytes(&traceHeader.tracl, 4);
    swapBytes(&traceHeader.tracr, 4);
    swapBytes(&traceHeader.fldr, 4);
    swapBytes(&traceHeader.tracf, 4);
    swapBytes(&traceHeader.ep, 4);
    swapBytes(&traceHeader.cdp, 4);
    swapBytes(&traceHeader.cdpt, 4);
    swapBytes(&traceHeader.trid, 2);
    swapBytes(&traceHeader.nvs, 2);
    swapBytes(&traceHeader.nhs, 2);
    swapBytes(&traceHeader.duse, 2);
    swapBytes(&traceHeader.offset,4);
    swapBytes(&traceHeader.gelev, 4);
    swapBytes(&traceHeader.selev, 4);
    swapBytes(&traceHeader.sdepth, 4);
    swapBytes(&traceHeader.gdel, 4);
    swapBytes(&traceHeader.sdel, 4);
    swapBytes(&traceHeader.swdep, 4);
    swapBytes(&traceHeader.gwdep, 4);
    swapBytes(&traceHeader.scalel, 2);
    swapBytes(&traceHeader.scalco, 2);
    swapBytes(&traceHeader.sx, 4);
    swapBytes(&traceHeader.sy, 4);
    swapBytes(&traceHeader.gx, 4);
    swapBytes(&traceHeader.gy, 4);
    swapBytes(&traceHeader.counit, 2);
    swapBytes(&traceHeader.wevel, 2);
    swapBytes(&traceHeader.swevel, 2);
    swapBytes(&traceHeader.sut, 2);
    swapBytes(&traceHeader.gut, 2);
    swapBytes(&traceHeader.sstat, 2);
    swapBytes(&traceHeader.gstat, 2);
    swapBytes(&traceHeader.tstat, 2);
    swapBytes(&traceHeader.laga, 2);
    swapBytes(&traceHeader.lagb, 2);
    swapBytes(&traceHeader.delrt, 2);
    swapBytes(&traceHeader.muts, 2);
    swapBytes(&traceHeader.mute, 2);
    swapBytes(&traceHeader.ns, 2);
    swapBytes(&traceHeader.dt, 2);
    swapBytes(&traceHeader.gain, 2);
    swapBytes(&traceHeader.igc, 2);
    swapBytes(&traceHeader.igi, 2);
    swapBytes(&traceHeader.corr, 2);
    swapBytes(&traceHeader.sfs, 2);
    swapBytes(&traceHeader.sfe, 2);
    swapBytes(&traceHeader.slen, 2);
    swapBytes(&traceHeader.styp, 2);
    swapBytes(&traceHeader.stas, 2);
    swapBytes(&traceHeader.stae, 2);
    swapBytes(&traceHeader.tatyp, 2);
    swapBytes(&traceHeader.afilf, 2);
    swapBytes(&traceHeader.afils, 2);
    swapBytes(&traceHeader.nofilf, 2);
    swapBytes(&traceHeader.nofils, 2);
    swapBytes(&traceHeader.lcf, 2);
    swapBytes(&traceHeader.hcf, 2);
    swapBytes(&traceHeader.lcs, 2);
    swapBytes(&traceHeader.hcs, 2);
    swapBytes(&traceHeader.year, 2);
    swapBytes(&traceHeader.day, 2);
    swapBytes(&traceHeader.hour, 2);
    swapBytes(&traceHeader.minute, 2);
    swapBytes(&traceHeader.sec, 2);
    swapBytes(&traceHeader.timbas, 2);
    swapBytes(&traceHeader.trwf, 2);
    swapBytes(&traceHeader.grnors, 2);
    swapBytes(&traceHeader.grnofr, 2);
    swapBytes(&traceHeader.grnlof, 2);
    swapBytes(&traceHeader.gaps, 2);
    swapBytes(&traceHeader.otrav, 2);
    swapBytes(&traceHeader.cdpx, 4);
    swapBytes(&traceHeader.cdpy, 4);
    swapBytes(&traceHeader.iline, 4);
    swapBytes(&traceHeader.xline, 4);
    swapBytes(&traceHeader.sp, 2);
    swapBytes(&traceHeader.spscal, 2);
    swapBytes(&traceHeader.tvmu, 2);
    swapBytes(&traceHeader.trdman, 4);
    swapBytes(&traceHeader.trdexp, 2);
    swapBytes(&traceHeader.trdun, 2);
    swapBytes(&traceHeader.dti, 2);
    swapBytes(&traceHeader.timscal, 2);
    swapBytes(&traceHeader.stypor, 2);
    swapBytes(&traceHeader.sedir[0], 2);
    swapBytes(&traceHeader.sedir[1], 2);
    swapBytes(&traceHeader.sedir[2], 2);
    swapBytes(&traceHeader.smman, 4);
    swapBytes(&traceHeader.smexp, 2);
    swapBytes(&traceHeader.smun, 2);
}


void SgyIO::swapTraceData(float *data, int n) const
{
    for(int i = 0; i < n; i++)
	swapBytes(&data[i], 4);
}

void SgyIO::swapTraceData(int *data, int n) const
{
    for(int i = 0; i < n; i++)
	swapBytes(&data[i], 4);
}

void SgyIO::swapTraceData(short *data, int n) const
{
    for(int i = 0; i < n; i++)
	swapBytes(&data[i], 2);
}

void SgyIO::swapBytes(void *p, int n) const
{
    int nl = 0;
    unsigned char *ptr = (unsigned char*)p;
    unsigned char temp; 
    if(n < 2) 
	return;
    n--;
    while(nl < n)
    {
        temp = ptr[nl] ;	
	    ptr[nl] = ptr[n];
        ptr[n] = temp;
	    nl++;
	    n--;
    }
}


void SgyIO::ibm2ieee(int *from, int *to, int n) const
{
    register int fconv, fmant, i, t ;

    for(i = 0; i < n; i++)
    {
	fconv = from[i];
	
	if(fconv)
	{
	    fmant = 0x00ffffff & fconv;

	    if(fmant == 0)
		cout << "mantissa is zero data may not be in IBM FLOAT format !" << endl;
	    t = (int) ((0x7f000000 & fconv) >> 22) - 130;

	    while (!(fmant & 0x00800000))
	    {
		--t;
		fmant <<= 1;
	    }

	    if(t > 254) 
		fconv = (0x80000000 & fconv) | 0x7f7fffff;
	    else if(t <=0 )
		fconv = 0;
	    else
		fconv = (0x80000000 & fconv) | (t << 23) | (0x007fffff & fmant);
	}

	to[i] = fconv;

    }

}

void SgyIO::ieee2ibm(int *from, int *to, int n) const
{
    register int fconv, fmant, i, t;

    for(i = 0; i < n; ++i)
    {
	fconv = from[i];

	if(fconv)
	{
	    fmant = (0x007fffff & fconv) | 0x00800000;
	    t = (int) ((0x7f800000 & fconv) >> 23) - 126;

	    while(t & 0x3)
	    {
		++t;
		fmant >>= 1;
	    }

	    fconv = (0x80000000 & fconv) | (((t >> 2) + 64) << 24) | fmant;
	}

	to[i] = fconv;
    }

}

