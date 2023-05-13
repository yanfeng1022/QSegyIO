#ifndef SGYREADER_H
#define SGYREADER_H

#include "sgyio.h"
#include <qstring.h>


class SgyReader : public SgyIO
{
public:
    SgyReader(); 
    SgyReader(const QString &fileName);
    virtual ~SgyReader();

    virtual bool setFile(const QString &fileName);

    bool isValid() const;
    short sampleNumber() const;
    short sampleInterval() const;
    qint64 traceNumber() const;
    void readRollHeader(RollHeader &rollHeader) const;
    void readTraceHeader(TraceHeader &traceHeader, int seqNum) const;
    void readTraceData(float  *data, int n, int seqNum) const;

private:
   short m_sampleNum;
   short m_sampleInt;
   qint64 m_traceNum;
   bool m_isValid; 
}; 
#endif
