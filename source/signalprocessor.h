//  ------------------------------------------------------------------------
//
//  This file is part of the Intan Technologies RHS2000 Interface
//  Version 1.01
//  Copyright (C) 2013-2017 Intan Technologies
//
//  ------------------------------------------------------------------------
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <queue>
#include "mainwindow.h"
#include "rhs2000datablock.h"

// The maximum number of Rhs2000DataBlock objects needed during normal data acquisition.
// (Used in MainWindow::changeSampleRate(), maximum sample rate case.)
#define MAX_NUM_BLOCKS_TO_READ 8
#define NOISE_FLOOR_CALIBRATION_CONSTANT (double)0.6745
using namespace std;
class QDataStream;
class SignalSources;
class Rhs2000DataBlock;
class RandomNumber;
class FIRFilter;

class SignalProcessor
{

public:
    SignalProcessor();
    ~SignalProcessor();

    void allocateMemory(int numStreams);
    void setNotchFilter(double notchFreq, double bandwidth, double sampleFreq);
    void setNotchFilterEnabled(bool enable);
    void setSpikeBandFitlerEnabled(bool enable);
    bool getSpikeBandFitlerStatus() const;
    void setSpikeBandFitlerSamplingRate(unsigned int sr_idx);
    void setHighpassFilter(double cutoffFreq, double sampleFreq);
    void setHighpassFilterEnabled(bool enable);
    int loadAmplifierData(queue<Rhs2000DataBlock> &dataQueue, int numBlocks,
                          bool lookForTrigger, int triggerChannel, int triggerPolarity,
                          int &triggerIndex, bool addToBuffer,
                          queue<Rhs2000DataBlock> &bufferQueue, bool saveToDisk, QDataStream &out,
                          SaveFormat format, bool saveTtlOut, bool saveDcAmps, int timestampOffset, ReferenceSource referenceSource);
    int loadSyntheticData(int numBlocks, double sampleRate, bool saveToDisk,
                          QDataStream &out, SaveFormat format, bool saveTtlOut, bool saveDcAmps, ReferenceSource &referenceSource);
    int saveBufferedData(queue<Rhs2000DataBlock> &bufferQueue, QDataStream &out, SaveFormat format, bool saveTtlOut, bool saveDcAmps, int timestampOffset);
    void createSaveList(SignalSources *signalSources, bool addTriggerChannel, int triggerChannel, double stimStepSize);
    void createTimestampFilename(QString path);
    int calibrateSpikeDetector(SignalSources *signalSources, double boardSampleRate, unsigned int numBlocks);
    void runSpikeDetector(const QVector<QVector<bool> > &channelVisible, unsigned int numBlocks, double boardSampleRate, MainWindow* mainWindow);
    void openTimestampFile();
    void closeTimestampFile();
    void createSignalTypeFilenames(QString path);
    void openSignalTypeFiles(bool saveTtlOut, bool saveDcAmps);
    void closeSignalTypeFiles();
    void createFilenames(SignalSources *signalSources, QString path);
    void openSaveFiles(SignalSources *signalSources, bool saveDcAmps);
    void closeSaveFiles(SignalSources *signalSources, bool saveDcAmps);
    int bytesPerBlock(SaveFormat saveFormat, bool saveTtlOut);
    void filterData(int numBlocks, const QVector<QVector<bool> > &channelVisible);
    void measureComplexAmplitude(QVector<QVector<QVector<double> > > &measuredMagnitude,
                           QVector<QVector<QVector<double> > > &measuredPhase,
                           int capIndex, int stream, int chipChannel,
                           int numBlocks, double sampleRate, double frequency, int numPeriods);
    unsigned int addSpikeDetectionChannel(unsigned int boardStream, unsigned int chipChannel);
    void addManualTrigChannel(unsigned int trig);
    void remManualTrigChannel(unsigned int trig);
    void remSpikeDetectionChannel(unsigned int boardStream, unsigned int chipChannel);
    void resetSpikeDetector();
    bool closedLoopStimTriggersAvailable(int trigger = -1);
    void updateChannelNumSigma(unsigned int channel, unsigned int stream, double numSigma = 100);
    inline QList<channel_id_t>* getSpikedetectorChannelIdList()
    {
        return &this->spikeDetection_channelIdList;
    }
    // QVector<QVector<QVector<double> > > amplifierPreFilter;
    double* amplifierPreFilterFast;
    bool spikeDetectorCalibrated;
    QVector<QVector<QVector<double> > > amplifierPostFilter;
    QVector<QVector<QVector<double> > > dcAmplifier;
    QVector<QVector<QVector<int> > > complianceLimit;
    QVector<QVector<QVector<int> > > stimOn;
    QVector<QVector<QVector<int> > > stimPol;
    QVector<QVector<QVector<int> > > ampSettle;
    QVector<QVector<QVector<int> > > chargeRecov;
    QVector<QVector<double> > boardDac;
    QVector<QVector<double> > boardAdc;
    QVector<QVector<int> > boardDigIn;
    QVector<QVector<int> > boardDigOut;

private:
    QVector<QVector<QVector<double> > > prevAmplifierPreFilter;
    QVector<QVector<QVector<double> > > prevAmplifierPostFilter;
    QVector<QVector<double> > highpassFilterState;
    int numDataStreams;
    double a1;
    double a2;
    double b0;
    double b1;
    double b2;
    bool notchFilterEnabled;
    bool spikeBandFilterEnabled;
    double aHpf;
    double bHpf;
    bool highpassFilterEnabled;

    bool saveListBoardDigIn;
    QVector<SignalChannel*> saveListAmplifier;
    QVector<SignalChannel*> saveListBoardDac;
    QVector<SignalChannel*> saveListBoardAdc;
    QVector<SignalChannel*> saveListBoardDigitalIn;
    QVector<SignalChannel*> saveListBoardDigitalOut;
    QVector<int> posStimAmplitudeList;
    QVector<int> negStimAmplitudeList;
    QList<double*> spikeDetectorCalib_heapList;
    QList<channel_id_t> spikeDetection_channelIdList;
    QVector<int> availableStimTriggers; // directory for tracking availability of manual triggers

    QString timestampFileName;
    QFile *timestampFile;
    QDataStream *timestampStream;

    QString amplifierFileName;
    QFile *amplifierFile;
    QDataStream *amplifierStream;

    QString dcAmplifierFileName;
    QFile *dcAmplifierFile;
    QDataStream *dcAmplifierStream;

    QString stimFileName;
    QFile *stimFile;
    QDataStream *stimStream;

    QString adcInputFileName;
    QFile *adcInputFile;
    QDataStream *adcInputStream;

    QString dacOutputFileName;
    QFile *dacOutputFile;
    QDataStream *dacOutputStream;

    QString digitalInputFileName;
    QFile *digitalInputFile;
    QDataStream *digitalInputStream;

    QString digitalOutputFileName;
    QFile *digitalOutputFile;
    QDataStream *digitalOutputStream;

    void allocateDoubleArray3D(QVector<QVector<QVector<double> > > &array3D,
                               int xSize, int ySize, int zSize);
    void allocateIntArray3D(QVector<QVector<QVector<int> > > &array3D,
                            int xSize, int ySize, int zSize);
    void allocateDoubleArray2D(QVector<QVector<double> > &array2D,
                               int xSize, int ySize);
    void allocateIntArray2D(QVector<QVector<int> > &array2D,
                            int xSize, int ySize);
    void allocateDoubleArray1D(QVector<double> &array1D, int xSize);
    void fillZerosDoubleArray3D(QVector<QVector<QVector<double> > > &array3D);
    void fillZerosDoubleArray2D(QVector<QVector<double> > &array2D);
    void amplitudeOfFreqComponent(double &realComponent, double &imagComponent,
                                  double *data, int stream, int channel,
                                  int startIndex, int endIndex,
                                  double sampleRate, double frequency);

    inline int fastIndex(int stream, int channel, int t) const
    {
        return ((t * numDataStreams * CHANNELS_PER_STREAM) + (channel * numDataStreams) + stream);
    }

    RandomNumber *random;
    FIRFilter *spikeBandFIRfilter;
    QVector<QVector<QVector<double> > > synthSpikeAmplitude;
    QVector<QVector<QVector<double> > > synthSpikeDuration;
    QVector<QVector<double> > synthRelativeSpikeRate;
    QVector<QVector<double> > synthEcgAmplitude;
    double tPulse;
    unsigned int synthTimeStamp;

    // To speed up writing to disk, we must create a char array which we fill with bytes of raw data
    // and use QDataStream::writeRawData(const char *s, int len) to stream out more efficiently.
    char dataStreamBuffer[4 * MAX_NUM_DATA_STREAMS * CHANNELS_PER_STREAM * SAMPLES_PER_DATA_BLOCK];
    char dataStreamBufferArray[MAX_NUM_DATA_STREAMS * CHANNELS_PER_STREAM][2 * SAMPLES_PER_DATA_BLOCK * MAX_NUM_BLOCKS_TO_READ];
    char dataStreamBufferArrayDc[MAX_NUM_DATA_STREAMS * CHANNELS_PER_STREAM][2 * SAMPLES_PER_DATA_BLOCK * MAX_NUM_BLOCKS_TO_READ];
    int bufferArrayIndex[MAX_NUM_DATA_STREAMS * CHANNELS_PER_STREAM];
    int bufferArrayIndexDc[MAX_NUM_DATA_STREAMS * CHANNELS_PER_STREAM];
};

#endif // SIGNALPROCESSOR_H
