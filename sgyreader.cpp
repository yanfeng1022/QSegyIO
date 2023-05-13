#include "sgyreader.h"
#include <qfile.h>
#include <iostream>
using namespace std;

SgyReader::SgyReader()
    : SgyIO(), m_sampleNum(0), m_sampleInt(0), m_isValid(false)
{
}

SgyReader::SgyReader(const QString &fileName)
    : SgyIO(), m_sampleNum(0), m_sampleInt(0), m_isValid(false)
{
    setFile(fileName);
}

SgyReader::~SgyReader()
{
}

/*this function comprises two parts. one is open the specified 
 * file,return ture if success, return false otherwise, the other
 * is verifing whether the file is sgy file, if yes,then get some 
 * vital information such as sample number, sample interval, samp-
 * le data format,etc,and set the m_valid true at the same time.
 * otherwise, return and set the m_valid false meanwhile .
 */
bool  SgyReader::setFile(const QString &fileName)
{
    //open file
    if(m_file == NULL)
    {
 m_file = new QFile(fileName);
 if(!m_file->open(QIODevice::ReadOnly))
 {
     cout << "can not open the file : " << fileName.toStdString() << endl;
     return false;
 }
    }else
    {
 if(m_file->isOpen())
     m_file->close();

 delete m_file;
 m_file = new QFile(fileName);


 if(!m_file->open(QIODevice::ReadOnly))
 {
     cout << "can not open the file : " << fileName.toStdString() << endl;
     return false;
 }
    }

    //verifing file
    short sampFormat;
    m_file->seek(3224);
    m_file->read((char*)&sampFormat, sizeof(short));
    char *p = (char*)&sampFormat;
    if(p[0] == 0)
    {
 switch(p[1])
 {
     case 1:
  {
      setFormat(BigEndian | IBMFloat);
      m_isValid = true;
      break;
  }
     case 2:
  {
      setFormat(BigEndian | FourBytesInteger);
      m_isValid = true;
      break;
  }
     case 3:
  {
      setFormat(BigEndian | TwoBytesInteger);
      m_isValid = true;
      break;
  }
     case 5:
  {
      setFormat(BigEndian | IEEEFloat);
      m_isValid = true;
      break;
  }
     case 8:
  {
      setFormat(BigEndian | OneByteInteger);
      m_isValid = true;
      break;
  }
     default:
  {
      m_isValid = false;
      break;
  }
 }
    }else if(p[1] == 0)
    {
 switch(p[0])
 {
     case 1:
  {
      setFormat(LittleEndian | IBMFloat);
      m_isValid = true;
      break;
  }
     case 2:
  {
      setFormat(LittleEndian | FourBytesInteger);
      m_isValid = true;
      break;
  }
     case 3:
  {
      setFormat(LittleEndian | TwoBytesInteger);
      m_isValid =true;
      break;
  }
     case 5:
  {
      setFormat(LittleEndian | IEEEFloat);
      m_isValid = true;
      break;
  }
     case 8:
  {
      setFormat(LittleEndian | OneByteInteger);
      m_isValid = true;
      break;
  }
     default:
  {
      m_isValid = false;
      break;
  }
 }
    }else
    {
 m_format = 0;
 m_isValid = false;
    }

    //get sample number ,sample interval and trace number if m_isValid is true
    if(m_isValid)
    {
 m_file->seek(3216);
        m_file->read((char*)&m_sampleInt, sizeof(m_sampleInt));

 m_file->seek(3220);
 m_file->read((char*)&m_sampleNum, sizeof(m_sampleNum));

 if(m_needSwap)
 {
     swapBytes(&m_sampleInt, 2);
     swapBytes(&m_sampleNum, 2);
 }

 //int size_int = m_file->size();//must be qint64 .bug for the first time.
 qint64 size = m_file->size();
 m_traceNum = (size - 3600) / (240 + m_sampleNum * 4);

    }

    return true;
}

bool SgyReader::isValid() const
{
    return m_isValid;
}

short SgyReader::sampleNumber() const
{
    return m_sampleNum;
}

short SgyReader::sampleInterval() const
{
    return m_sampleInt;
}

qint64 SgyReader::traceNumber() const
{
    return m_traceNum;
}

void SgyReader::readRollHeader(RollHeader &rollHeader) const
{
    m_file->seek(0);
    m_file->read((char*)&rollHeader, 3600);

    if(m_needSwap)
       swapRollHeader(rollHeader);
}

void SgyReader::readTraceHeader(TraceHeader &traceHeader, int seqNum) const
{
    int tSize = 0;
    int sampType =  m_format & 0x0000007c; // source file sample data format

    switch(sampType)
    {
 case TwoBytesInteger:
     {
  tSize = 2;
  break;
     }
 case OneByteInteger:
     {
  tSize = 1;
  break;
     }
 default:
     {
  tSize = 4;
  break;
     }
    }
    qint64 nnn1=3600 ,nnn2=(seqNum - 1),nnn3=(240 + m_sampleNum * tSize);
    qint64 pos = nnn1+nnn2* nnn3;//here wo must use qint64 instead of int ,because of the support to large file .
    m_file->seek(pos);
    m_file->read((char*)&traceHeader, 240);

    if(m_needSwap)
        swapTraceHeader(traceHeader);
}


/*this function is used for reading trace data for type floating .
 *(i.e ieee or ibm). the pointer data points to the actual data, 
 *the n represents the size of the array , usually, which is the
 *number of the sample, seqNum represent which trace should be
 *read .
 *note : seqNum starts from 1.....N .
 */
void SgyReader::readTraceData(float *data, int n, int seqNum) const
{
    int dataFormat = sampleDataFormat();
    switch(dataFormat)
    {
 case IBMFloat:
     {
qint64  nnn1=3840,nnn2=(seqNum - 1) ,nnn3= (240 + m_sampleNum * sizeof(float));
qint64 pos= nnn1+nnn2*nnn3;
//  qint64 pos = 3840 + (seqNum - 1) * (240 + m_sampleNum * sizeof(float));
  m_file->seek(pos);


  if(!m_needSwap && !m_needFloatConversion)
  {
      m_file->read((char*)data, n * sizeof(float));
  }else
  {
      float *temp = new float[n];

      m_file->read((char*)temp, n * sizeof(float));

      if(m_needSwap)
   swapTraceData(temp, n);

      if(m_needFloatConversion)
      {
   ibm2ieee((int*)temp, (int*)data, n);
   delete []temp;
   return;
      }

      memcpy(data, temp, n * sizeof(float));
      delete []temp;
  }

  break;
     } 
 case IEEEFloat:
     {
        qint64  nnn11=3840,nnn22=(seqNum - 1) ,nnn33= (240 + m_sampleNum * sizeof(float));
        qint64 pos= nnn11+nnn22*nnn33;
  //qint64 pos = 3840 + (seqNum - 1) * (240 + m_sampleNum * sizeof(float));
  m_file->seek(pos);


  if(!m_needSwap && !m_needFloatConversion)
  {
      m_file->read((char*)data, n * sizeof(float));
  }else
  {
      float *temp = new float[n];

      m_file->read((char*)temp, n * sizeof(float));

      if(m_needSwap)
   swapTraceData(temp, n);

      if(m_needFloatConversion)
      {
   ieee2ibm((int*)temp, (int*)data, n);
   delete []temp;
   return;
      }

      memcpy(data, temp, n * sizeof(float));
      delete []temp;
  }

  break;
     }
 case FourBytesInteger:
     {
 qint64  nnn111=3840,nnn222=(seqNum - 1) ,nnn333= (240 + m_sampleNum * sizeof(float));
              qint64 pos= nnn111+nnn222*nnn333;
  //qint64 pos = 3840 + ( seqNum - 1) * (240 + m_sampleNum * sizeof(int));
  m_file->seek(pos);


  int *temp = new int[n];
  m_file->read((char*)temp, n * sizeof(int));

  if(m_needSwap)
      swapTraceData(temp, n);

  for(int i = 0; i < n; i++)
      data[i] = temp[i];

  delete []temp;


  break;
     }
 case TwoBytesInteger:
     {
  qint64  n1=3840,n2=(seqNum - 1) ,n3= (240 + m_sampleNum * sizeof(float));
  qint64 pos= n1+n2*n3;
//qint64 pos = 3840 + (seqNum - 1) * (240 + m_sampleNum * sizeof(short));
  m_file->seek(pos);

  short *temp = new short[n];
  m_file->read((char*)temp, n * sizeof(short));
  if(m_needSwap)
      swapTraceData(temp, n);

  for(int i = 0; i < n; i++)
      data[i] = temp[i];

  delete []temp;

  break;
     }
 case OneByteInteger:
     {
qint64  n_1=3840,n_2=(seqNum - 1) ,n_3= (240 + m_sampleNum * sizeof(float));
qint64 pos=n_1+n_2*n_3; 
 // qint64 pos = 3840 + (seqNum - 1) * (240 + m_sampleNum * sizeof(char));
  m_file->seek(pos);

  char *temp = new char[n];
  m_file->read(temp, n * sizeof(char));

  for(int i = 0; i < n; i++)
      data[i] = temp[i];

  delete []temp;

  break;
     }
 default:
     {
  break;
     }
    }/* end switch */

}




































