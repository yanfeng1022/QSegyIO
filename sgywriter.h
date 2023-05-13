#ifndef SGYWRITER_H
#define SGYWRITER_H

#include "sgyio.h"
#include <qfile.h>
#include <qtextstream.h>
class SgyWriter : public SgyIO
{
public:
    SgyWriter();
    SgyWriter(const QString &fileName, int format = LOCALFORMAT);
    virtual ~SgyWriter();


    virtual bool setFile(const QString &fileName);
    void writeRollHeader(const RollHeader &rollHeader) const;
    void writeTraceHeader(const TraceHeader &traceHeader) const;
    void writeTraceData(const float *data, int n) const;

 void close();
};
#endif
