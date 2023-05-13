#include <QtCore/QCoreApplication>
#include "QFileDialog"
#include "QDebug"
#include <sgyreader.h>
#include <sgywriter.h>
#include <qfiledialog.h>
#include <qobject.h>
#include <random>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace std;


struct SegyTrace {
    int id;
    int iline, xline;
    double x, y;
    vector<float> dat;
};

void readSgy(int argc, char*argv[]);

void scanSgy(int argc, char* argv[]);

void writeIBM();
void writeSgy();


int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    
    //readSgy(argc, argv);
    scanSgy(argc, argv);
    //writeSgy();
    //writeIBM();

    //return a.exec();
    return 0;
}



void readSgy(int argc, char* argv[])
{

    string fileName = "..\\..\\test\\segy_test\\croped.segy";
    SgyReader sgyRd{ QString::fromStdString(fileName) };
    bool isvalid = sgyRd.isValid();
    int sampleNum = sgyRd.sampleNumber();
    int sampleDt = sgyRd.sampleInterval();
    qint64 traceNum = sgyRd.traceNumber();

    RollHeader rollheader;
    sgyRd.readRollHeader(rollheader);
    string asciiHeader = rollheader.getAsciiHeader();

    std::ofstream fout("..\\..\\test\\segy_test\\croped.txt");
    // fout << asciiHeader << endl;
    vector<string> ascHeadLines(40);
    for (int i = 0; i < 40; i++) {
        ascHeadLines[i] = asciiHeader.substr(80 * i, 80);
        fout << ascHeadLines[i] << endl;
        cout << ascHeadLines[i] << endl;
    }
    fout.flush();


    TraceHeader traceHeader;
    int traceSize = sizeof(traceHeader);


    for (int i = 1; i <= traceNum; i++) {
        sgyRd.readTraceHeader(traceHeader, i);
        vector<float> traceDat(sampleNum);
        sgyRd.readTraceData(traceDat.data(), sampleNum, i);
        for (auto item : traceDat) {
            fout << item <<" ";
        }
        fout << endl;
    }
    fout.close();


}

void scanSgy(int argc, char* argv[])
{
    string fileName = "..\\..\\test\\segy_test\\croped.segy";
    SgyReader sgyRd{ QString::fromStdString(fileName) };
    bool isvalid = sgyRd.isValid();
    int sampleNum = sgyRd.sampleNumber();
    int sampleDt = sgyRd.sampleInterval();
    qint64 traceNum = sgyRd.traceNumber();

    RollHeader rollheader;
    sgyRd.readRollHeader(rollheader);

    int ilineMin = INT32_MAX, ilineMax = -INT32_MAX;
    int xlineMin = INT32_MAX, xlineMax = -INT32_MAX;
    double xMin = DBL_MAX, xMax = -DBL_MAX;
    double yMin = DBL_MAX, yMax = -DBL_MAX;
    TraceHeader traceHeader;
    for (int i = 1; i <= traceNum; i++) {
        sgyRd.readTraceHeader(traceHeader, i);
        if (traceHeader.fldr < ilineMin) {
            ilineMin = traceHeader.fldr;
        }
        if (traceHeader.fldr > ilineMax) {
            ilineMax = traceHeader.fldr;
        }
        if (traceHeader.cdp < xlineMin) {
            xlineMin = traceHeader.cdp;
        }
        if (traceHeader.cdp > xlineMax) {
            xlineMax = traceHeader.cdp;
        }
        if (traceHeader.sx < xMin) {
            xMin = traceHeader.sx;
        }
        if (traceHeader.sx > xMax) {
            xMax = traceHeader.sx;
        }
        if (traceHeader.sy < yMin) {
            yMin = traceHeader.sy;
        }
        if (traceHeader.sy > yMax) {
            yMax = traceHeader.sy;
        }
    }
    cout << "iline range " << ilineMin << " " << ilineMax << endl;
    cout << "xline range " << xlineMin << " " << xlineMax << endl;
    cout << "x range " << xMin << " " << xMax << endl;
    cout << "y range " << yMin << " " << yMax << endl;

}

void writeIBM()
{

    QString inputSegyFileName = QFileDialog::getSaveFileName(nullptr, QObject::tr("Read Segy File"),
        "",
        QObject::tr("segy file (*.segy)"));

    QString outputFileName = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save Segy File"),
        "",
        QObject::tr("segy file (*.segy)"));

    
    SgyReader read(inputSegyFileName);

    //输出要为PC格式，可以改为SgyWriter write(filename,SgyIO::LittleEndian | SgyIO::IEEFloat)

    SgyWriter write(outputFileName, SgyIO::BigEndian | SgyIO::IBMFloat);//输出为IBM格式
    RollHeader rh;
    read.readRollHeader(rh);
    write.writeRollHeader(rh);
    int count = read.traceNumber();
    int number = read.sampleNumber();
    float* data;
    TraceHeader th;

    for (int i = 1; i <= count; i++)
    {
        read.readTraceHeader(th, i);
        write.writeTraceHeader(th);
        data = new float[number];
        read.readTraceData(data, number, i);
        write.writeTraceData(data, number);
        data = NULL;
    }

}

void writeSgy()
{
    SgyWriter porCube("por1.sgy", SgyIO::BigEndian | SgyIO::IBMFloat);
    std::string txtHeader;
    txtHeader.resize(SEGY_TEXT_HEADER_SIZE);

    auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    std::string timeMsg = ctime(&time);
    txtHeader.insert(0, "segy is created by qsgyio of iRDS software");
    txtHeader.insert(80, "file is created at " + timeMsg);

    float refStep = 2000;
    int ns = 5000;
    int inline_start = 100;
    int inline_end = 300;
    int crline_start = 300;
    int crline_end = 400;


    txtHeader.insert(160, "inline at 9 - 13, cross line at 21-24");
    txtHeader.insert(240, "sx at 73-76, xy at 77-80");
    txtHeader.insert(320, "inline range " + std::to_string(inline_start) + " to " + std::to_string(inline_end));
    txtHeader.insert(400, "cross line range " + std::to_string(crline_start) + " to " + std::to_string(crline_end));
    txtHeader.insert(480, "sample interval in micro secs "+std::to_string(refStep));
    txtHeader.insert(560, "number of samples per trace " + std::to_string(ns));
    txtHeader.resize(SEGY_TEXT_HEADER_SIZE);

    RollHeader rh;
    TraceHeader th;

    rh.hdt = refStep;
    rh.hns = ns;

    rh.setAsciiHeader(txtHeader);
    porCube.writeRollHeader(rh);

    th.dt = refStep;
    th.ns = ns;

    //float* trcData = new float[ns];
    //memset(trcData, 0, ns * sizeof(float));
    std::vector<float> trcData(ns);
    std::random_device rdevice;
    std::default_random_engine rnd(rdevice());
    std::normal_distribution<float> normDis(0, 0.1);
    float amp = 2;
    for (int i = 0; i < ns; i++) {
        trcData[i] = amp * (sin(0.01 * i) + normDis(rnd));
    }

    
    for (int inl = inline_start; inl <= inline_end; inl++)
        for (int crl = crline_start; crl <= crline_end; crl++)
        {
            th.fldr = inl;
            th.cdp = crl;

            th.sx = inl;
            th.sy = crl;
            th.gx = inl;
            th.gy = crl;

            
            porCube.writeTraceHeader(th);
            porCube.writeTraceData(trcData.data(), ns);
        }


    porCube.close();

    //delete[] trcData;
}
