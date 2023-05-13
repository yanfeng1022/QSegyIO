#include "sgywriter.h"
#include <qfile.h>
#include <iostream>
using namespace std;

SgyWriter::SgyWriter()
: SgyIO()
{
}

SgyWriter::SgyWriter(const QString &fileName, int format)
    : SgyIO()
{
    setFile(fileName);
    setFormat(format);
}


SgyWriter::~SgyWriter()
{
}

bool SgyWriter::setFile(const QString &fileName)
{
    if(m_file == NULL)
    {
        m_file = new QFile(fileName);
        if(!m_file->open(QIODevice::WriteOnly))
        {
            cout << " can not create file : " << fileName.toStdString() << endl;
            return false;
        }
    }
    else
    {
        if(m_file->isOpen())
            m_file->close();
        delete m_file;     
        m_file = new QFile(fileName);
        if(!m_file->open(QIODevice::WriteOnly))
        {
            cout << "can not create file : " << fileName.toStdString() << endl;
            return false;
        }
    }

    return true;
}


void SgyWriter::writeRollHeader(const RollHeader &rollHeader) const
{
 RollHeader temp = rollHeader;

    switch(m_format & 0x0000007c)
    {
 case IBMFloat:
     {
  temp.format = 1;
  break;
     }
 case IEEEFloat:
     {
  temp.format = 5;
  break;
     }
 case FourBytesInteger:
     {
  temp.format = 2;
  break;
     }
 case TwoBytesInteger:
     {
  temp.format = 3;
  break;
     }
 case OneByteInteger:
     {
  temp.format = 8;
  break;
     }
 default:
     break;
    }

    if(m_needSwap)
 swapRollHeader(temp);
 
 m_file->write((char*)&temp, 3600);
}

void SgyWriter::writeTraceHeader(const TraceHeader &traceHeader) const
{
 TraceHeader temp = traceHeader;

    if(m_needSwap)
 swapTraceHeader(temp);

    m_file->write((char*)&temp, 240);
}

void SgyWriter::writeTraceData(const float *data, int n) const
{
 float *clone = new float[n];
 memcpy(clone, data, n*sizeof(float));

    int dataFormat= sampleDataFormat();
    switch(dataFormat)
    {
 case IBMFloat:
     {
  if(!m_needFloatConversion && !m_needSwap)
  {
      m_file->write((char*)clone, n * sizeof(float));
  }else
  {
   if(m_needFloatConversion)
   {
    float *temp = new float[n];
    memcpy(temp, clone, n * sizeof(float));

    ieee2ibm((int*)temp, (int*)clone, n);

    delete [] temp;
   }

      if(m_needSwap)
   swapTraceData(clone, n);

      m_file->write((char*)clone, n * sizeof(float));
  }

  break;
     }
 case IEEEFloat:
  {
   if(!m_needFloatConversion && !m_needSwap)
   {
    m_file->write((char*)clone, n * sizeof(float));
   }else
   {

    if(m_needFloatConversion)
    {
     float *temp = new float[n];
     memcpy(temp, clone, n * sizeof(float));

     ibm2ieee((int*)temp, (int*)clone, n);

     delete [] temp;
    }

    if(m_needSwap)
     swapTraceData(clone, n);

    m_file->write((char*)clone, n * sizeof(float));
   }

   break;
  }
 case FourBytesInteger:
  {
  int *temp = new int[n];
  for(int i = 0; i < n; i++)
      temp[i] = clone[i];

  if(m_needSwap)
      swapTraceData(temp, n);

  m_file->write((char*)temp, n *sizeof(int));

  delete []temp;

  break;
     }
 case TwoBytesInteger:
     {
  short *temp = new short[n];
  for(int i = 0; i < n; i++)
      temp[i] = clone[i];

  if(m_needSwap)
      swapTraceData(temp, n);

  m_file->write((char*)temp, n * sizeof(short));

  delete []temp;

  break;
     }
 case OneByteInteger:
     {
  char *temp = new char[n];
  for(int i = 0; i < n; i++)
      temp[i] = clone[i];

  m_file->write((char*)temp, n *sizeof(char));

  delete []temp;

  break;
     }
 default:     
     {
  break;
     }
    }/* end switch */

 delete [] clone;
}

void SgyWriter::close()
{
	if(m_file != NULL)
	{
		if(m_file->isOpen())
			m_file->close();
        m_file = NULL;
		delete m_file;
	}

}


























