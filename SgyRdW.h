#pragma once

#include "sgyreader.h"
#include "sgywriter.h"

class SgyRdW
{
public:
	SgyRdW();
	SgyRdW(const std::string& sgyFile);
	~SgyRdW();

	void scanInfo();

private:
	std::string _sgyFile;
	SgyReader _sgyReader;

	int _traceNums;
	float _dt;
	float _datum;
	int _ilineMin, _ilineMax, _xlineMin, _xlineMax;
	double _xMin, _xMax, _yMin, _yMax;

};

struct SgyTrace {
	int id;
	int iline, xline;
	double x, y;
	std::vector<float> dat;
};