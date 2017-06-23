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

#include <QtGui>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#endif

/* Stim Param Dialog is a dialog box that allows the user to view and change a channel's stimulation parameters */

#include "signalchannel.h"
#include "stimparamdialog.h"
#include "timespinbox.h"
#include "currentspinbox.h"
#include "stimfigure.h"
#include "mainwindow.h"
#include "signalprocessor.h"
#include <cmath>

StimParamDialog::StimParamDialog(StimParameters *parameter, SignalChannel* selectedChannel, double timestep_us, double currentstep_uA, QWidget *parent)
    : QDialog(parent)
{
    //set the parameter, timestep, and currentstep objects, so that they can be accessed by other functions in stimparamdialog.cpp
    parameters = parameter;
    thrSpikeDetector = 4.0;// recommended threshold value
    timestep = timestep_us;
    currentstep = currentstep_uA;
    mainWindow = (MainWindow*) parent;
    // channel properties will be needed later when the stim params are being saved
    this->selectedChannel = selectedChannel;
    //create a new StimFigure
    stimFigure = new StimFigure(parameters, this);

    //create stimulation waveform widgets
    stimWaveForm = new QGroupBox(tr("Stimulation Waveform"));

    stimShapeLabel = new QLabel(tr("Stimulation Shape: "));
    stimShape = new QComboBox();
    QStringList stimShapes;
    //stimShapes << "Biphasic" << "Biphasic with Delay" << "Triphasic" << "No Stimulation (Amp Settle Only)";
    stimShapes << "Biphasic" << "Biphasic with Delay" << "Triphasic";
    stimShape->addItems(stimShapes);

    stimPolarityLabel = new QLabel(tr("Stimulation Polarity: "));
    stimPolarity = new QComboBox();
    QStringList stimPolarities;
    stimPolarities << "Cathodic Current First" << "Anodic Current First";
    stimPolarity->addItems(stimPolarities);

    firstPhaseDurationLabel = new QLabel(tr("First Phase Duration (D1): "));
    firstPhaseDuration = new TimeSpinBox(timestep_us);
    firstPhaseDuration->setRange(0, 5000);

    secondPhaseDurationLabel = new QLabel(tr("Second Phase Duration (D2): "));
    secondPhaseDuration = new TimeSpinBox(timestep_us);
    secondPhaseDuration->setRange(0, 5000);

    interphaseDelayLabel = new QLabel(tr("Interphase Delay (DP): "));
    interphaseDelay = new TimeSpinBox(timestep_us);
    interphaseDelay->setRange(0, 5000);

    firstPhaseAmplitudeLabel = new QLabel(tr("First Phase Amplitude (A1): "));
    firstPhaseAmplitude = new CurrentSpinBox(currentstep_uA);
    firstPhaseAmplitude->setRange(0, 255 * currentstep_uA);

    secondPhaseAmplitudeLabel = new QLabel(tr("Second Phase Amplitude (A2): "));
    secondPhaseAmplitude = new CurrentSpinBox(currentstep_uA);
    secondPhaseAmplitude->setRange(0, 255 * currentstep_uA);

    totalPositiveChargeLabel = new QLabel();
    totalNegativeChargeLabel = new QLabel();
    chargeBalanceLabel = new QLabel(tr("Charge Balance Placeholder"));
    chargeBalanceLabel->setAlignment(Qt::AlignRight);

    /*
    //create step display widgets
    displayTimeStepLabel = new QLabel(tr("Fundamental time step: ") + QString::number(timestep_us) + " " + QSTRING_MU_SYMBOL + "s");
    displayCurrentStepLabel = new QLabel(tr("Fundamental current step: ") + QString::number(currentstep_uA) + " " + QSTRING_MU_SYMBOL + "A");
    if (currentstep_uA < 1)
        displayCurrentStepLabel->setText(tr("Fundamental current step: ") + QString::number(currentstep_uA * 1000) + " nA");
    */

    //create trigger information widgets
    trigger = new QGroupBox(tr("Trigger"));

    enableStim = new QCheckBox(tr("Enable"));

    triggerSourceLabel = new QLabel(tr("Trigger Source: "));
    triggerSource = new QComboBox();
    QStringList triggerSources;
    // the order in this list must be consistent with the ordering in StimParameters::TriggerSources
    triggerSources << "DIGITAL IN 1" << "DIGITAL IN 2" << "DIGITAL IN 3" << "DIGITAL IN 4" << "DIGITAL IN 5"
                   << "DIGITAL IN 6" << "DIGITAL IN 7" << "DIGITAL IN 8" << "DIGITAL IN 9" << "DIGITAL IN 10"
                   << "DIGITAL IN 11" << "DIGITAL IN 12" << "DIGITAL IN 13" << "DIGITAL IN 14" << "DIGITAL IN 15"
                   << "DIGITAL IN 16" << "ANALOG IN 1" << "ANALOG IN 2" << "ANALOG IN 3" << "ANALOG IN 4"
                   << "ANALOG IN 5" << "ANALOG IN 6" << "ANALOG IN 7" << "ANALOG IN 8" << "KEYPRESS: 1"
                   << "KEYPRESS: 2" << "KEYPRESS: 3" << "KEYPRESS: 4" << "KEYPRESS: 5" << "KEYPRESS: 6"
                   << "KEYPRESS: 7" << "KEYPRESS: 8" << "CLOSED LOOP STIM";
    triggerSource->addItems(triggerSources);

    calibWindowLabel = new QLabel(tr("Calibration Window Length for spike detector in (s)"));
    calibWindow = new QComboBox();
    QStringList calibWindowLenList;
    calibWindowLenList <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10";
    calibWindow->addItems(calibWindowLenList);
    calibWindow->setEnabled(false);// enabled only when the closed loop stim option is selected from trigger source list
    calibWindowLabel->setEnabled(false);

    thrSpikeDetectorLabel = new QLabel(tr("Threshold value for spike detector"));
    thrSpikeDetectorLineEdit = new QLineEdit(QString::number(thrSpikeDetector, 'f', 0));
    thrSpikeDetectorLineEdit->setValidator(new QDoubleValidator(0.0001, 9999.9999, 4, this));
    thrSpikeDetectorLabel->setEnabled(false);
    thrSpikeDetectorLineEdit->setEnabled(false);

    //triggerEdgeOrLevelLabel = new QLabel(tr("Stimulation is: "));
    triggerEdgeOrLevelLabel = new QLabel(tr(" "));
    triggerEdgeOrLevel = new QComboBox();
    QStringList triggerEdgeOrLevels;
    triggerEdgeOrLevels << "Edge Triggered" << "Level Triggered";
    triggerEdgeOrLevel->addItems(triggerEdgeOrLevels);

    //triggerHighOrLowLabel = new QLabel(tr("Stimulation will: "));
    triggerHighOrLowLabel = new QLabel(tr(" "));
    triggerHighOrLow = new QComboBox();
    QStringList triggerHighOrLows;
    triggerHighOrLows << "Trigger on High" << "Trigger on Low";
    triggerHighOrLow->addItems(triggerHighOrLows);

    postTriggerDelayLabel = new QLabel(tr("Post Trigger Delay: "));
    postTriggerDelay = new TimeSpinBox(timestep_us);
    postTriggerDelay->setRange(0, 500000);

    //create pulse train information widgets
    pulseTrain = new QGroupBox(tr("Pulse Train"));

    pulseOrTrainLabel = new QLabel(tr("Pulse Repetition: "));
    pulseOrTrain = new QComboBox();
    QStringList pulseOrTrains;
    pulseOrTrains << "Single Stim Pulse" << "Stim Pulse Train";
    pulseOrTrain->addItems(pulseOrTrains);

    numberOfStimPulsesLabel = new QLabel(tr("Number of Stim Pulses"));
    numberOfStimPulses = new QSpinBox();
    numberOfStimPulses->setRange(2, 256);

    pulseTrainPeriodLabel = new QLabel(tr("Pulse Train Period: "));
    pulseTrainPeriod = new TimeSpinBox(timestep_us);
    pulseTrainPeriod->setRange(0, 1000000);

    pulseTrainFrequencyLabel = new QLabel();

    refractoryPeriodLabel = new QLabel(tr("Post-Stim Refractory Period: "));
    refractoryPeriod = new TimeSpinBox(timestep_us);
    refractoryPeriod->setRange(0, 1000000);

    //create Amp Settle widgets
    ampSettle = new QGroupBox(tr("Amp Settle"));

    preStimAmpSettleLabel = new QLabel(tr("Pre Stim Amp Settle: "));
    preStimAmpSettle = new TimeSpinBox(timestep_us);
    preStimAmpSettle->setRange(0, 500000);

    postStimAmpSettleLabel = new QLabel(tr("Post Stim Amp Settle: "));
    postStimAmpSettle = new TimeSpinBox(timestep_us);
    postStimAmpSettle->setRange(0, 500000);

    maintainAmpSettle = new QCheckBox(tr("Maintain amp settle during pulse train"));

    enableAmpSettle = new QCheckBox(tr("Enable Amp Settle"));

    //create Charge Recovery widgets
    chargeRecovery = new QGroupBox(tr("Charge Recovery"));

    postStimChargeRecovOnLabel = new QLabel(tr("Post Stim Charge Recovery On: "));
    postStimChargeRecovOn = new TimeSpinBox(timestep_us);
    postStimChargeRecovOn->setRange(0, 1000000);

    postStimChargeRecovOffLabel = new QLabel(tr("Post Stim Charge Recovery Off: "));
    postStimChargeRecovOff = new TimeSpinBox(timestep_us);
    postStimChargeRecovOff->setRange(0, 1000000);

    enableChargeRecovery = new QCheckBox(tr("Enable Charge Recovery"));

    //create and connect 'OK' and 'Cancel' buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    //connect internal signals and slots
    connect(thrSpikeDetectorLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setThrSpikeDetector()));
    connect(calibWindow, SIGNAL(currentIndexChanged(int)), this, SLOT(setSpikeDetectorCalibWin(int)));
    connect(enableChargeRecovery, SIGNAL(stateChanged(int)), this, SLOT(enableWidgets()));
    connect(enableAmpSettle, SIGNAL(stateChanged(int)), this, SLOT(enableWidgets()));
    connect(enableStim, SIGNAL(stateChanged(int)), this, SLOT(enableWidgets()));
    connect(pulseOrTrain, SIGNAL(currentIndexChanged(int)), this, SLOT(enableWidgets()));
    connect(stimShape, SIGNAL(currentIndexChanged(int)), this, SLOT(enableWidgets()));
    connect(triggerSource,SIGNAL(currentIndexChanged(int)), this, SLOT(closeLoopStimIdxSelected(int)));
    connect(stimShape, SIGNAL(currentIndexChanged(int)), this, SIGNAL(chargeChanged()));
    connect(stimShape, SIGNAL(currentIndexChanged(int)), this, SIGNAL(minimumPeriodChanged()));
    connect(stimPolarity, SIGNAL(currentIndexChanged(int)), this, SIGNAL(chargeChanged()));
    connect(firstPhaseDuration, SIGNAL(valueChanged(double)), this, SIGNAL(chargeChanged()));
    connect(firstPhaseDuration, SIGNAL(valueChanged(double)), this, SIGNAL(minimumPeriodChanged()));
    connect(secondPhaseDuration, SIGNAL(valueChanged(double)), this, SIGNAL(chargeChanged()));
    connect(secondPhaseDuration, SIGNAL(valueChanged(double)), this, SIGNAL(minimumPeriodChanged()));
    connect(firstPhaseAmplitude, SIGNAL(valueChanged(double)), this, SIGNAL(chargeChanged()));
    connect(secondPhaseAmplitude, SIGNAL(valueChanged(double)), this, SIGNAL(chargeChanged()));
    connect(this, SIGNAL(chargeChanged()), this, SLOT(calculateCharge()));
    connect(pulseTrainPeriod, SIGNAL(valueChanged(double)), this, SLOT(calculatePulseTrainFrequency()));
    connect(this, SIGNAL(minimumPeriodChanged()), this, SLOT(constrainPulseTrainPeriod()));
    connect(preStimAmpSettle, SIGNAL(trueValueChanged(double)), this, SLOT(constrainPostTriggerDelay()));
    connect(postStimChargeRecovOn, SIGNAL(trueValueChanged(double)), this, SLOT(constrainPostStimChargeRecovery()));
    connect(postStimAmpSettle, SIGNAL(trueValueChanged(double)), this, SLOT(constrainRefractoryPeriod()));
    connect(postStimChargeRecovOff, SIGNAL(trueValueChanged(double)), this, SLOT(constrainRefractoryPeriod()));
    connect(firstPhaseDuration, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(secondPhaseDuration, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(interphaseDelay, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(interphaseDelay, SIGNAL(valueChanged(double)), this, SIGNAL(minimumPeriodChanged()));
    connect(postTriggerDelay, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(pulseTrainPeriod, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(refractoryPeriod, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(preStimAmpSettle, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(postStimAmpSettle, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(postStimChargeRecovOff, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(postStimChargeRecovOn, SIGNAL(editingFinished()), this, SLOT(roundTimeInputs()));
    connect(firstPhaseAmplitude, SIGNAL(editingFinished()), this, SLOT(roundCurrentInputs()));
    connect(secondPhaseAmplitude, SIGNAL(editingFinished()), this, SLOT(roundCurrentInputs()));
    connect(enableStim, SIGNAL(toggled(bool)), stimFigure, SLOT(highlightStimTrace(bool)));

    //connect signals to stimFigure's non-highlight slots
    connect(stimShape, SIGNAL(currentIndexChanged(int)), stimFigure, SLOT(updateStimShape(int)));
    connect(stimPolarity, SIGNAL(currentIndexChanged(int)), stimFigure, SLOT(updateStimPolarity(int)));
    connect(pulseOrTrain, SIGNAL(currentIndexChanged(int)), stimFigure, SLOT(updatePulseOrTrain(int)));
    connect(enableAmpSettle, SIGNAL(toggled(bool)), stimFigure, SLOT(updateEnableAmpSettle(bool)));
    connect(maintainAmpSettle, SIGNAL(toggled(bool)), stimFigure, SLOT(updateMaintainAmpSettle(bool)));
    connect(enableChargeRecovery, SIGNAL(toggled(bool)), stimFigure, SLOT(updateEnableChargeRecovery(bool)));

    //connect signals to stimFigure's highlight slots
    connect(this, SIGNAL(highlightFirstPhaseDuration(bool)), stimFigure, SLOT(highlightFirstPhaseDuration(bool)));
    connect(this, SIGNAL(highlightSecondPhaseDuration(bool)), stimFigure, SLOT(highlightSecondPhaseDuration(bool)));
    connect(this, SIGNAL(highlightInterphaseDelay(bool)), stimFigure, SLOT(highlightInterphaseDelay(bool)));
    connect(this, SIGNAL(highlightFirstPhaseAmplitude(bool)), stimFigure, SLOT(highlightFirstPhaseAmplitude(bool)));
    connect(this, SIGNAL(highlightSecondPhaseAmplitude(bool)), stimFigure, SLOT(highlightSecondPhaseAmplitude(bool)));
    connect(this, SIGNAL(highlightPostTriggerDelay(bool)), stimFigure, SLOT(highlightPostTriggerDelay(bool)));
    connect(this, SIGNAL(highlightPulseTrainPeriod(bool)), stimFigure, SLOT(highlightPulseTrainPeriod(bool)));
    connect(this, SIGNAL(highlightRefractoryPeriod(bool)), stimFigure, SLOT(highlightRefractoryPeriod(bool)));
    connect(this, SIGNAL(highlightPreStimAmpSettle(bool)), stimFigure, SLOT(highlightPreStimAmpSettle(bool)));
    connect(this, SIGNAL(highlightPostStimAmpSettle(bool)), stimFigure, SLOT(highlightPostStimAmpSettle(bool)));
    connect(this, SIGNAL(highlightPostStimChargeRecovOn(bool)), stimFigure, SLOT(highlightPostStimChargeRecovOn(bool)));
    connect(this, SIGNAL(highlightPostStimChargeRecovOff(bool)), stimFigure, SLOT(highlightPostStimChargeRecovOff(bool)));

    //update widgets' states to be different than the default states, so state changes are detected properly

    stimShape->setCurrentIndex(StimParameters::Triphasic);
    stimPolarity->setCurrentIndex(StimParameters::PositiveFirst);
    firstPhaseDuration->setValue(1);
    secondPhaseDuration->setValue(1);
    interphaseDelay->setValue(1);
    firstPhaseAmplitude->setValue(1);
    secondPhaseAmplitude->setValue(1);
    enableStim->setChecked(true);
    triggerSource->setCurrentIndex(StimParameters::AnalogIn1);
    triggerEdgeOrLevel->setCurrentIndex(StimParameters::Level);
    triggerHighOrLow->setCurrentIndex(StimParameters::Low);
    postTriggerDelay->setValue(2);
    pulseOrTrain->setCurrentIndex(StimParameters::SinglePulse);
    numberOfStimPulses->setValue(2);
    pulseTrainPeriod->setValue(1);
    refractoryPeriod->setValue(1);
    preStimAmpSettle->setValue(1);
    postStimAmpSettle->setValue(1);
    maintainAmpSettle->setChecked(true);
    enableAmpSettle->setChecked(false);
    postStimChargeRecovOff->setValue(1);
    postStimChargeRecovOn->setValue(1);
    enableChargeRecovery->setChecked(false);

    //update dialog's state based on structParameters
    loadParameters(parameters);

    //stimulation waveform widgets' layout
    QHBoxLayout *stimShapeRow = new QHBoxLayout;
    stimShapeRow->addWidget(stimShapeLabel);
    stimShapeRow->addStretch();
    stimShapeRow->addWidget(stimShape);

    QHBoxLayout *stimPolarityRow = new QHBoxLayout;
    stimPolarityRow->addWidget(stimPolarityLabel);
    stimPolarityRow->addStretch();
    stimPolarityRow->addWidget(stimPolarity);

    QHBoxLayout *firstPhaseDurationRow = new QHBoxLayout;
    firstPhaseDurationRow->addWidget(firstPhaseDurationLabel);
    firstPhaseDurationRow->addStretch();
    firstPhaseDurationRow->addWidget(firstPhaseDuration);

    QHBoxLayout *secondPhaseDurationRow = new QHBoxLayout;
    secondPhaseDurationRow->addWidget(secondPhaseDurationLabel);
    secondPhaseDurationRow->addStretch();
    secondPhaseDurationRow->addWidget(secondPhaseDuration);

    QHBoxLayout *interphaseDelayRow = new QHBoxLayout;
    interphaseDelayRow->addWidget(interphaseDelayLabel);
    interphaseDelayRow->addStretch();
    interphaseDelayRow->addWidget(interphaseDelay);

    QHBoxLayout *firstPhaseAmplitudeRow = new QHBoxLayout;
    firstPhaseAmplitudeRow->addWidget(firstPhaseAmplitudeLabel);
    firstPhaseAmplitudeRow->addStretch();
    firstPhaseAmplitudeRow->addWidget(firstPhaseAmplitude);

    QHBoxLayout *secondPhaseAmplitudeRow = new QHBoxLayout;
    secondPhaseAmplitudeRow->addWidget(secondPhaseAmplitudeLabel);
    secondPhaseAmplitudeRow->addStretch();
    secondPhaseAmplitudeRow->addWidget(secondPhaseAmplitude);

    QHBoxLayout *totalPositiveChargeRow = new QHBoxLayout;
    totalPositiveChargeRow->addWidget(totalPositiveChargeLabel);

    QHBoxLayout *totalNegativeChargeRow = new QHBoxLayout;
    totalNegativeChargeRow->addWidget(totalNegativeChargeLabel);

    QHBoxLayout *chargeBalanceRow = new QHBoxLayout;
    chargeBalanceRow->addWidget(chargeBalanceLabel);

    QVBoxLayout *stimWaveFormLayout = new QVBoxLayout;
    stimWaveFormLayout->addLayout(stimShapeRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(stimPolarityRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(firstPhaseDurationRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(secondPhaseDurationRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(interphaseDelayRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(firstPhaseAmplitudeRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(secondPhaseAmplitudeRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(totalPositiveChargeRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(totalNegativeChargeRow);
    //stimWaveFormLayout->addStretch();
    stimWaveFormLayout->addLayout(chargeBalanceRow);
    stimWaveFormLayout->addStretch();
    stimWaveForm->setLayout(stimWaveFormLayout);

    /*
    //display time/current step widgets' layout
    QHBoxLayout *displayTimeStepRow = new QHBoxLayout;
    displayTimeStepRow->addWidget(displayTimeStepLabel);
    displayTimeStepRow->setAlignment(Qt::AlignRight);


    QHBoxLayout *displayCurrentStepRow = new QHBoxLayout;
    displayCurrentStepRow->addWidget(displayCurrentStepLabel);
    displayCurrentStepRow->setAlignment(Qt::AlignRight);
    */

    //trigger information widgets' layout
    QHBoxLayout *enableStimRow = new QHBoxLayout;
    enableStimRow->addWidget(enableStim);

    QHBoxLayout *triggerSourceRow = new QHBoxLayout;
    triggerSourceRow->addWidget(triggerSourceLabel);
    triggerSourceRow->addStretch();
    triggerSourceRow->addWidget(triggerSource);

    QHBoxLayout *calibWindowRow = new QHBoxLayout;
    calibWindowRow->addWidget(calibWindowLabel);
    calibWindowRow->addStretch();
    calibWindowRow->addWidget(calibWindow);

    QHBoxLayout *thrSpikeDetectorRow = new QHBoxLayout;
    thrSpikeDetectorRow->addWidget(thrSpikeDetectorLabel);
    thrSpikeDetectorRow->addStretch();
    thrSpikeDetectorRow->addWidget(thrSpikeDetectorLineEdit);

    QHBoxLayout *triggerEdgeOrLevelRow = new QHBoxLayout;
    triggerEdgeOrLevelRow->addWidget(triggerEdgeOrLevelLabel);
    triggerEdgeOrLevelRow->addStretch();
    triggerEdgeOrLevelRow->addWidget(triggerEdgeOrLevel);

    QHBoxLayout *triggerHighOrLowRow = new QHBoxLayout;
    triggerHighOrLowRow->addWidget(triggerHighOrLowLabel);
    triggerHighOrLowRow->addStretch();
    triggerHighOrLowRow->addWidget(triggerHighOrLow);

    QHBoxLayout *postTriggerDelayRow = new QHBoxLayout;
    postTriggerDelayRow->addWidget(postTriggerDelayLabel);
    postTriggerDelayRow->addStretch();
    postTriggerDelayRow->addWidget(postTriggerDelay);

    QVBoxLayout *triggerLayout = new QVBoxLayout;
    triggerLayout->addLayout(enableStimRow);
    triggerLayout->addStretch();
    triggerLayout->addLayout(triggerSourceRow);
    triggerLayout->addStretch();
    triggerLayout->addLayout(calibWindowRow);
    triggerLayout->addStretch();
    triggerLayout->addLayout(thrSpikeDetectorRow);
    triggerLayout->addStretch();
    triggerLayout->addLayout(triggerEdgeOrLevelRow);
    triggerLayout->addStretch();
    triggerLayout->addLayout(triggerHighOrLowRow);
    triggerLayout->addStretch();
    triggerLayout->addLayout(postTriggerDelayRow);
    trigger->setLayout(triggerLayout);

    //pulse train information widgets' layout
    QHBoxLayout *pulseOrTrainRow = new QHBoxLayout;
    pulseOrTrainRow->addWidget(pulseOrTrainLabel);
    pulseOrTrainRow->addStretch();
    pulseOrTrainRow->addWidget(pulseOrTrain);

    QHBoxLayout *numberOfStimPulsesRow = new QHBoxLayout;
    numberOfStimPulsesRow->addWidget(numberOfStimPulsesLabel);
    numberOfStimPulsesRow->addStretch();
    numberOfStimPulsesRow->addWidget(numberOfStimPulses);

    QHBoxLayout *pulseTrainPeriodRow = new QHBoxLayout;
    pulseTrainPeriodRow->addWidget(pulseTrainPeriodLabel);
    pulseTrainPeriodRow->addStretch();
    pulseTrainPeriodRow->addWidget(pulseTrainPeriod);

    QHBoxLayout *pulseTrainFrequencyRow = new QHBoxLayout;
    pulseTrainFrequencyRow->addWidget(pulseTrainFrequencyLabel);

    QHBoxLayout *refractoryPeriodRow = new QHBoxLayout;
    refractoryPeriodRow->addWidget(refractoryPeriodLabel);
    refractoryPeriodRow->addStretch();
    refractoryPeriodRow->addWidget(refractoryPeriod);

    QVBoxLayout *pulseTrainLayout = new QVBoxLayout;
    pulseTrainLayout->addLayout(pulseOrTrainRow);
    pulseTrainLayout->addLayout(numberOfStimPulsesRow);
    pulseTrainLayout->addLayout(pulseTrainPeriodRow);
    pulseTrainLayout->addLayout(pulseTrainFrequencyRow);
    pulseTrainLayout->addLayout(refractoryPeriodRow);
    pulseTrain->setLayout(pulseTrainLayout);

    //amp Settle widgets' layout
    QHBoxLayout *preStimAmpSettleRow = new QHBoxLayout;
    preStimAmpSettleRow->addWidget(preStimAmpSettleLabel);
    preStimAmpSettleRow->addStretch();
    preStimAmpSettleRow->addWidget(preStimAmpSettle);

    QHBoxLayout *postStimAmpSettleRow = new QHBoxLayout;
    postStimAmpSettleRow->addWidget(postStimAmpSettleLabel);
    postStimAmpSettleRow->addStretch();
    postStimAmpSettleRow->addWidget(postStimAmpSettle);

    QHBoxLayout *maintainAmpSettleRow = new QHBoxLayout;
    maintainAmpSettleRow->addWidget(maintainAmpSettle);

    QHBoxLayout *enableAmpSettleRow = new QHBoxLayout;
    enableAmpSettleRow->addWidget(enableAmpSettle);

    QVBoxLayout *ampSettleLayout = new QVBoxLayout;
    ampSettleLayout->addLayout(enableAmpSettleRow);
    ampSettleLayout->addLayout(preStimAmpSettleRow);
    ampSettleLayout->addLayout(postStimAmpSettleRow);
    ampSettleLayout->addLayout(maintainAmpSettleRow);
    ampSettle->setLayout(ampSettleLayout);

    //charge Recovery widgets' layout
    QHBoxLayout *postStimChargeRecovOnRow = new QHBoxLayout;
    postStimChargeRecovOnRow->addWidget(postStimChargeRecovOnLabel);
    postStimChargeRecovOnRow->addStretch();
    postStimChargeRecovOnRow->addWidget(postStimChargeRecovOn);

    QHBoxLayout *postStimChargeRecovOffRow = new QHBoxLayout;
    postStimChargeRecovOffRow->addWidget(postStimChargeRecovOffLabel);
    postStimChargeRecovOffRow->addStretch();
    postStimChargeRecovOffRow->addWidget(postStimChargeRecovOff);

    QHBoxLayout *enableChargeRecoveryRow = new QHBoxLayout;
    enableChargeRecoveryRow->addWidget(enableChargeRecovery);

    QVBoxLayout *chargeRecoveryLayout = new QVBoxLayout;
    chargeRecoveryLayout->addLayout(enableChargeRecoveryRow);
    chargeRecoveryLayout->addLayout(postStimChargeRecovOnRow);
    chargeRecoveryLayout->addLayout(postStimChargeRecovOffRow);
    chargeRecovery->setLayout(chargeRecoveryLayout);

    //first Column
    QVBoxLayout *firstColumn = new QVBoxLayout;
    firstColumn->addWidget(trigger);
    firstColumn->addWidget(pulseTrain);
    firstColumn->addStretch();

    //second Column
    QVBoxLayout *secondColumn = new QVBoxLayout;
    secondColumn->addWidget(stimWaveForm);
    secondColumn->addStretch();

    //third Column
    QVBoxLayout *thirdColumn = new QVBoxLayout;
    thirdColumn->addWidget(ampSettle);
    thirdColumn->addWidget(chargeRecovery);
    thirdColumn->addStretch();

    //final Row
    QHBoxLayout *finalRow = new QHBoxLayout;
    /*
    finalRow->addLayout(displayTimeStepRow);
    finalRow->addStretch();
    finalRow->addLayout(displayCurrentStepRow);
    finalRow->addStretch();
    */
    finalRow->addWidget(buttonBox);

    //main Layout
    QHBoxLayout *columns = new QHBoxLayout;
    columns->addLayout(firstColumn);
    columns->addLayout(secondColumn);
    columns->addLayout(thirdColumn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(stimFigure);
    mainLayout->addLayout(columns);
    mainLayout->addStretch();
    mainLayout->addLayout(finalRow);
    setLayout(mainLayout);

    setWindowTitle("Stimulation Parameters: " + selectedChannel->nativeChannelName + " (" + selectedChannel->customChannelName + ")");
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());
}

StimParamDialog::~StimParamDialog()
{

}

/* Public member function that loads the widgets in StimParamDialog with the values from StimParameters */
void StimParamDialog::loadParameters(StimParameters *parameters)
{
    //set zero values initially to bring all ranges to microseconds, so values can be set properly
    preStimAmpSettle->setValue(0);
    postStimChargeRecovOn->setValue(0);
    postStimAmpSettle->setValue(0);
    postStimChargeRecovOff->setValue(0);
    firstPhaseDuration->setValue(0);
    secondPhaseDuration->setValue(0);
    interphaseDelay->setValue(0);
    postTriggerDelay->setValue(0);
    pulseTrainPeriod->setValue(0);
    refractoryPeriod->setValue(0);
    firstPhaseAmplitude->setValue(0);
    secondPhaseAmplitude->setValue(0);

    //load parameters that affect the ranges of other parameters first:
    //preStimAmpSettle, postStimChargeRecovOn, postStimAmpSettle, and postStimChargeRecovOff
    preStimAmpSettle->loadValue(parameters->preStimAmpSettle);
    postStimChargeRecovOn->loadValue(parameters->postStimChargeRecovOn);
    postStimAmpSettle->loadValue(parameters->postStimAmpSettle);
    postStimChargeRecovOff->loadValue(parameters->postStimChargeRecovOff);


    //load the rest of the parameters
    stimShape->setCurrentIndex(parameters->stimShape);
    stimPolarity->setCurrentIndex(parameters->stimPolarity);
    firstPhaseDuration->loadValue(parameters->firstPhaseDuration);
    secondPhaseDuration->loadValue(parameters->secondPhaseDuration);
    interphaseDelay->loadValue(parameters->interphaseDelay);
    firstPhaseAmplitude->loadValue(parameters->firstPhaseAmplitude);
    secondPhaseAmplitude->loadValue(parameters->secondPhaseAmplitude);
    enableStim->setChecked(parameters->enabled);
    triggerSource->setCurrentIndex(parameters->triggerSourceDisplay);
    triggerEdgeOrLevel->setCurrentIndex(parameters->triggerEdgeOrLevel);
    triggerHighOrLow->setCurrentIndex(parameters->triggerHighOrLow);
    postTriggerDelay->loadValue(parameters->postTriggerDelay);
    pulseOrTrain->setCurrentIndex(parameters->pulseOrTrain);
    numberOfStimPulses->setValue(parameters->numberOfStimPulses);
    pulseTrainPeriod->loadValue(parameters->pulseTrainPeriod);
    refractoryPeriod->loadValue(parameters->refractoryPeriod);
    maintainAmpSettle->setChecked(parameters->maintainAmpSettle);
    enableAmpSettle->setChecked(parameters->enableAmpSettle);
    enableChargeRecovery->setChecked(parameters->enableChargeRecovery);
    // the index value is
    calibWindow->setCurrentIndex(parameters->spikeDetectCalibWindow-1);
    thrSpikeDetectorLineEdit->setText(QString::number(parameters->spikeDetectionThr));
    //constrain time periods so that the first values the user sees are valid
    constrainPulseTrainPeriod();
    constrainPostStimChargeRecovery();
    constrainPostTriggerDelay();
    constrainRefractoryPeriod();

    //calculate frequency and charge so that the first labels that are displayed correspond to the loaded parameters
    calculatePulseTrainFrequency();
    calculateCharge();

}

/* Public slot that saves the values from the dialog box widgets into the parameters object, and closes the window */
void StimParamDialog::accept()
{
    bool enableClosedLoop = false;
    // check if close loop stim needs to be disabled
    if (parameters->triggerSourceDisplay == StimParameters::ClosedLoop &&
        (StimParameters::TriggerSources)triggerSource->currentIndex() != StimParameters::ClosedLoop &&
        enableStim->isChecked() )
    {
        mainWindow->ClosedLoopStimEnabled--;
        mainWindow->getSignalProcessorObj()->remSpikeDetectionChannel(selectedChannel->boardStream , selectedChannel->chipChannel);
    }
    // check if closed loop stim needs to be enabled
    if (parameters->triggerSourceDisplay != StimParameters::ClosedLoop &&
       (StimParameters::TriggerSources)triggerSource->currentIndex() == StimParameters::ClosedLoop &&
        enableStim->isChecked())
    {
        mainWindow->ClosedLoopStimEnabled++;
        unsigned int trigger = mainWindow->getSignalProcessorObj()->addSpikeDetectionChannel(selectedChannel->boardStream , selectedChannel->chipChannel);
        // remap the parameter triggerSource to a keypress that is available
        parameters->triggerSource = (StimParameters::TriggerSources)trigger;
        enableClosedLoop = true;
    }
    // check if global trigger is being disabled on a closed loop stim channel
    if (!enableStim->isChecked() && parameters->enabled && (StimParameters::TriggerSources)triggerSource->currentIndex() == StimParameters::ClosedLoop)
    {
        mainWindow->ClosedLoopStimEnabled--;
        mainWindow->getSignalProcessorObj()->remSpikeDetectionChannel(selectedChannel->boardStream , selectedChannel->chipChannel);
    }
    else if (enableStim->isChecked() && !parameters->enabled && (StimParameters::TriggerSources)triggerSource->currentIndex() == StimParameters::ClosedLoop)
    {
        mainWindow->ClosedLoopStimEnabled++;
        unsigned int trigger = mainWindow->getSignalProcessorObj()->addSpikeDetectionChannel(selectedChannel->boardStream , selectedChannel->chipChannel);
        // remap the parameter triggerSource to a keypress that is available
        parameters->triggerSource = (StimParameters::TriggerSources)trigger;
        enableClosedLoop = true;
    }

    // manage manual trigger setup
    if ( (StimParameters::TriggerSources) triggerSource->currentIndex() >= StimParameters::KeyPress1 &&
         (StimParameters::TriggerSources) triggerSource->currentIndex() <= StimParameters::KeyPress8 &&
         (StimParameters::TriggerSources) triggerSource->currentIndex() == parameters->triggerSourceDisplay &&
          enableStim->isChecked() && !parameters->enabled
       )
    {
        mainWindow->getSignalProcessorObj()->addManualTrigChannel(triggerSource->currentIndex() - StimParameters::KeyPress1);
    }
    else if( (StimParameters::TriggerSources) triggerSource->currentIndex() >= StimParameters::KeyPress1 &&
             (StimParameters::TriggerSources) triggerSource->currentIndex() <= StimParameters::KeyPress8 &&
             (StimParameters::TriggerSources) triggerSource->currentIndex() == parameters->triggerSourceDisplay &&
              !enableStim->isChecked() && parameters->enabled
            )
    {
        mainWindow->getSignalProcessorObj()->remManualTrigChannel(triggerSource->currentIndex() - StimParameters::KeyPress1);
    }
    // switching to a different manual trigger source keypress
    if ( (StimParameters::TriggerSources) triggerSource->currentIndex() >= StimParameters::KeyPress1 &&
         (StimParameters::TriggerSources) triggerSource->currentIndex() <= StimParameters::KeyPress8 &&
          parameters->triggerSourceDisplay >= StimParameters::KeyPress1 && parameters->triggerSourceDisplay <= StimParameters::KeyPress8 &&
          (StimParameters::TriggerSources) triggerSource->currentIndex() != parameters->triggerSourceDisplay &&
          enableStim->isChecked()
       )
    {
        // add a new trigger
        mainWindow->getSignalProcessorObj()->addManualTrigChannel(triggerSource->currentIndex() - StimParameters::KeyPress1);
        // remove old trigger source
        mainWindow->getSignalProcessorObj()->remManualTrigChannel(parameters->triggerSourceDisplay - StimParameters::KeyPress1);
    }
    // selected a manual trigger but was not a manual trigger previously
    if ( (StimParameters::TriggerSources) triggerSource->currentIndex() >= StimParameters::KeyPress1 &&
         (StimParameters::TriggerSources) triggerSource->currentIndex() <= StimParameters::KeyPress8 &&
          parameters->triggerSourceDisplay < StimParameters::KeyPress1 && parameters->triggerSourceDisplay > StimParameters::KeyPress8 &&
          (StimParameters::TriggerSources) triggerSource->currentIndex() != parameters->triggerSourceDisplay &&
          enableStim->isChecked()
       )
    {
         mainWindow->getSignalProcessorObj()->addManualTrigChannel(triggerSource->currentIndex() - StimParameters::KeyPress1);
    }
    // selected a trigger that is not a manual trigger but it was a manual trigger previously
    if ( (StimParameters::TriggerSources) triggerSource->currentIndex() < StimParameters::KeyPress1 &&
         (StimParameters::TriggerSources) triggerSource->currentIndex() > StimParameters::KeyPress8 &&
          parameters->triggerSourceDisplay >= StimParameters::KeyPress1 && parameters->triggerSourceDisplay <= StimParameters::KeyPress8 &&
          (StimParameters::TriggerSources) triggerSource->currentIndex() != parameters->triggerSourceDisplay &&
          enableStim->isChecked()
       )
    {
         mainWindow->getSignalProcessorObj()->remManualTrigChannel(triggerSource->currentIndex() - StimParameters::KeyPress1);
    }
    //save the values of the parameters from the dialog box into the object
    parameters->stimShape = (StimParameters::StimShapeValues) stimShape->currentIndex();
    parameters->stimPolarity = (StimParameters::StimPolarityValues) stimPolarity->currentIndex();
    parameters->firstPhaseDuration = firstPhaseDuration->getTrueValue();
    parameters->secondPhaseDuration = secondPhaseDuration->getTrueValue();
    parameters->interphaseDelay = interphaseDelay->getTrueValue();
    parameters->firstPhaseAmplitude = firstPhaseAmplitude->getTrueValue();
    parameters->secondPhaseAmplitude = secondPhaseAmplitude->getTrueValue();
    parameters->enabled = enableStim->isChecked();
    if (!enableClosedLoop)
        parameters->triggerSource = (StimParameters::TriggerSources) triggerSource->currentIndex();
    parameters->triggerSourceDisplay = (StimParameters::TriggerSources) triggerSource->currentIndex();
    parameters->triggerEdgeOrLevel = (StimParameters::TriggerEdgeOrLevels) triggerEdgeOrLevel->currentIndex();
    parameters->triggerHighOrLow = (StimParameters::TriggerHighOrLows) triggerHighOrLow->currentIndex();
    parameters->postTriggerDelay = postTriggerDelay->getTrueValue();
    parameters->pulseOrTrain = (StimParameters::PulseOrTrainValues) pulseOrTrain->currentIndex();
    parameters->numberOfStimPulses = numberOfStimPulses->value();
    parameters->pulseTrainPeriod = pulseTrainPeriod->getTrueValue();
    parameters->refractoryPeriod = refractoryPeriod->getTrueValue();
    parameters->maintainAmpSettle = maintainAmpSettle->isChecked();
    parameters->enableAmpSettle = enableAmpSettle->isChecked();
    parameters->enableChargeRecovery = enableChargeRecovery->isChecked();
    parameters->preStimAmpSettle = preStimAmpSettle->getTrueValue();
    parameters->postStimChargeRecovOn = postStimChargeRecovOn->getTrueValue();
    parameters->postStimAmpSettle = postStimAmpSettle->getTrueValue();
    parameters->postStimChargeRecovOff = postStimChargeRecovOff->getTrueValue();

    parameters->spikeDetectCalibWindow = calibWindow->currentText().toInt();
    parameters->spikeDetectionThr = thrSpikeDetectorLineEdit->text().toDouble();

    //close the window
    done(Accepted);
}


/* Public slot that emits signals when widgets that can be highlighted gain or lose focus */
void StimParamDialog::notifyFocusChanged(QWidget *lostFocus, QWidget *gainedFocus)
{
    //emit signals when a widget loses focus
    if (lostFocus == firstPhaseDuration->pointer()) {
        emit highlightFirstPhaseDuration(false);
    }
    if (lostFocus == secondPhaseDuration->pointer()) {
        emit highlightSecondPhaseDuration(false);
    }
    if (lostFocus == interphaseDelay->pointer()) {
        emit highlightInterphaseDelay(false);
    }
    if (lostFocus == firstPhaseAmplitude->pointer()) {
        emit highlightFirstPhaseAmplitude(false);
    }
    if (lostFocus == secondPhaseAmplitude->pointer()) {
        emit highlightSecondPhaseAmplitude(false);
    }
    if (lostFocus == postTriggerDelay->pointer()) {
        emit highlightPostTriggerDelay(false);
    }
    if (lostFocus == pulseTrainPeriod->pointer()) {
        emit highlightPulseTrainPeriod(false);
    }
    if (lostFocus == refractoryPeriod->pointer()) {
        emit highlightRefractoryPeriod(false);
    }
    if (lostFocus == preStimAmpSettle->pointer()) {
        emit highlightPreStimAmpSettle(false);
    }
    if (lostFocus == postStimAmpSettle->pointer()) {
        emit highlightPostStimAmpSettle(false);
    }
    if (lostFocus == postStimChargeRecovOn->pointer()) {
        emit highlightPostStimChargeRecovOn(false);
    }
    if (lostFocus == postStimChargeRecovOff->pointer()) {
        emit highlightPostStimChargeRecovOff(false);
    }

    //emit signals when a widget gains focus
    if (gainedFocus == firstPhaseDuration->pointer()) {
        emit highlightFirstPhaseDuration(true);
    }
    if (gainedFocus == secondPhaseDuration->pointer()) {
        emit highlightSecondPhaseDuration(true);
    }
    if (gainedFocus == interphaseDelay->pointer()) {
        emit highlightInterphaseDelay(true);
    }
    if (gainedFocus == firstPhaseAmplitude->pointer()) {
        emit highlightFirstPhaseAmplitude(true);
    }
    if (gainedFocus == secondPhaseAmplitude->pointer()) {
        emit highlightSecondPhaseAmplitude(true);
    }
    if (gainedFocus == postTriggerDelay->pointer()) {
        emit highlightPostTriggerDelay(true);
    }
    if (gainedFocus == pulseTrainPeriod->pointer()) {
        emit highlightPulseTrainPeriod(true);
    }
    if (gainedFocus == refractoryPeriod->pointer()) {
        emit highlightRefractoryPeriod(true);
    }
    if (gainedFocus == preStimAmpSettle->pointer()) {
        emit highlightPreStimAmpSettle(true);
    }
    if (gainedFocus == postStimAmpSettle->pointer()) {
        emit highlightPostStimAmpSettle(true);
    }
    if (gainedFocus == postStimChargeRecovOn->pointer()) {
        emit highlightPostStimChargeRecovOn(true);
    }
    if (gainedFocus == postStimChargeRecovOff->pointer()) {
        emit highlightPostStimChargeRecovOff(true);
    }
}

/* Private slot that looks at each widget individually and the state of its control widgets to see if it should be enabled or disabled */
void StimParamDialog::enableWidgets()
{
    //boolean conditional statements reflect if each widget should be enabled

    /* Trigger Group Box */
    triggerSourceLabel->setEnabled(enableStim->isChecked());
    triggerSource->setEnabled(enableStim->isChecked());
    triggerEdgeOrLevel->setEnabled(enableStim->isChecked());
    triggerHighOrLow->setEnabled(enableStim->isChecked());
    postTriggerDelayLabel->setEnabled(enableStim->isChecked());
    postTriggerDelay->setEnabled(enableStim->isChecked());
    // only disable but not enable
    if(!enableStim->isChecked())
    {
        calibWindow->setEnabled(enableStim->isChecked());
        calibWindowLabel->setEnabled(enableStim->isChecked());
        thrSpikeDetectorLabel->setEnabled(enableStim->isChecked());
        thrSpikeDetectorLineEdit->setEnabled(enableStim->isChecked());
    }
    // stim is enabled but the selected option is also closed loop.
    else if(enableStim->isChecked() && triggerSource->currentIndex() == StimParameters::ClosedLoop)
    {
        calibWindow->setEnabled(enableStim->isChecked());
        calibWindowLabel->setEnabled(enableStim->isChecked());
        thrSpikeDetectorLabel->setEnabled(enableStim->isChecked());
        thrSpikeDetectorLineEdit->setEnabled(enableStim->isChecked());
    }

    /* Pulse Train Group Box */
    pulseOrTrainLabel->setEnabled(enableStim->isChecked());
    pulseOrTrain->setEnabled(enableStim->isChecked());
    numberOfStimPulsesLabel->setEnabled(enableStim->isChecked() && pulseOrTrain->currentIndex() == StimParameters::PulseTrain);
    numberOfStimPulses->setEnabled(enableStim->isChecked() && pulseOrTrain->currentIndex() == StimParameters::PulseTrain);
    pulseTrainPeriodLabel->setEnabled(enableStim->isChecked() && pulseOrTrain->currentIndex() == StimParameters::PulseTrain);
    pulseTrainPeriod->setEnabled(enableStim->isChecked() && pulseOrTrain->currentIndex() == StimParameters::PulseTrain);
    pulseTrainFrequencyLabel->setEnabled(enableStim->isChecked() && pulseOrTrain->currentIndex() == StimParameters::PulseTrain);
    refractoryPeriodLabel->setEnabled(enableStim->isChecked());
    refractoryPeriod->setEnabled(enableStim->isChecked());

    /* Stimulation Waveform */
    stimShapeLabel->setEnabled(enableStim->isChecked());
    stimShape->setEnabled(enableStim->isChecked());
    stimPolarityLabel->setEnabled(enableStim->isChecked());
    stimPolarity->setEnabled(enableStim->isChecked());
    firstPhaseDurationLabel->setEnabled(enableStim->isChecked());
    firstPhaseDuration->setEnabled(enableStim->isChecked());
    secondPhaseDurationLabel->setEnabled(enableStim->isChecked());
    secondPhaseDuration->setEnabled(enableStim->isChecked());
    interphaseDelayLabel->setEnabled(enableStim->isChecked() && stimShape->currentIndex() == StimParameters::BiphasicWithInterphaseDelay);
    interphaseDelay->setEnabled(enableStim->isChecked() && stimShape->currentIndex() == StimParameters::BiphasicWithInterphaseDelay);
    firstPhaseAmplitudeLabel->setEnabled(enableStim->isChecked());
    firstPhaseAmplitude->setEnabled(enableStim->isChecked());
    secondPhaseAmplitudeLabel->setEnabled(enableStim->isChecked());
    secondPhaseAmplitude->setEnabled(enableStim->isChecked());
    totalPositiveChargeLabel->setEnabled(enableStim->isChecked());
    totalNegativeChargeLabel->setEnabled(enableStim->isChecked());

    /* Amp Settle */
    enableAmpSettle->setEnabled(enableStim->isChecked());
    preStimAmpSettleLabel->setEnabled(enableStim->isChecked() && enableAmpSettle->isChecked());
    preStimAmpSettle->setEnabled(enableStim->isChecked() && enableAmpSettle->isChecked());
    postStimAmpSettleLabel->setEnabled(enableStim->isChecked() && enableAmpSettle->isChecked());
    postStimAmpSettle->setEnabled(enableStim->isChecked() && enableAmpSettle->isChecked());
    maintainAmpSettle->setEnabled(enableStim->isChecked() && enableAmpSettle->isChecked() && pulseOrTrain->currentIndex() == StimParameters::PulseTrain);

    /* Charge Recovery */
    enableChargeRecovery->setEnabled(enableStim->isChecked());
    postStimChargeRecovOnLabel->setEnabled(enableStim->isChecked() && enableChargeRecovery->isChecked());
    postStimChargeRecovOn->setEnabled(enableStim->isChecked() && enableChargeRecovery->isChecked());
    postStimChargeRecovOffLabel->setEnabled(enableStim->isChecked() && enableChargeRecovery->isChecked());
    postStimChargeRecovOff->setEnabled(enableStim->isChecked() && enableChargeRecovery->isChecked());

    /* Reset Text for First Phase Labels */
    if (stimShape->currentIndex() == StimParameters::Biphasic || stimShape->currentIndex() == StimParameters::BiphasicWithInterphaseDelay)
    {
        firstPhaseDurationLabel->setText(tr("First Phase Duration (D1): "));
        firstPhaseAmplitudeLabel->setText(tr("First Phase Amplitude (A1): "));
    }
    else if (stimShape->currentIndex() == StimParameters::Triphasic)
    {
        firstPhaseDurationLabel->setText(tr("First/Third Phase Duration (D1): "));
        firstPhaseAmplitudeLabel->setText(tr("First/Third Phase Amplitude (A1): "));
    }
}
// check if the spike band filter is enabled.
// Only then allow close loop stimulation.
void StimParamDialog::closeLoopStimIdxSelected(int idx)
{
    if(!(mainWindow)->getSignalProcessorObj()->getSpikeBandFitlerStatus() && idx == StimParameters::ClosedLoop)
    {
        // undo the selection
        triggerSource->setCurrentIndex(StimParameters::DigitalIn1);
        // generate warning to the user that the spike band filter is not enabled
        QMessageBox::information(this,tr("Error"),
                                      tr("The spike band filter"
                                         "is disabled. Enable the spike band filter"
                                         "in order to use this option."),QMessageBox::Ok);
    }
    // the spike detector re-maps the stim signal source to a keypress. There are only 8 supported keypresses.
    // Also check to see if a manual keypress is being allotted as a trigger and whether this is possible
    // since there are only 8 slots for a manual trigger
    else if( !(mainWindow)->getSignalProcessorObj()->closedLoopStimTriggersAvailable() && idx == StimParameters::ClosedLoop )
    {
        triggerSource->setCurrentIndex(StimParameters::DigitalIn1);
        QMessageBox::information(this,tr("Error"),
                                      tr("No More manual stim striggers available."
                                         "Currently, there are only 8 manual stim triggers available."
                                         "Please remove some manual stim channels or CLosed Loop"
                                         "Stim channels and try again."),QMessageBox::Ok);
    }
    else if((mainWindow)->getSignalProcessorObj()->getSpikeBandFitlerStatus() && idx == StimParameters::ClosedLoop)
    {
        //enable calibration window for the spike detector
        calibWindow->setEnabled(true);
        calibWindowLabel->setEnabled(true);
        // enable the threshold text box.
        thrSpikeDetectorLabel->setEnabled(true);
        thrSpikeDetectorLineEdit->setEnabled(true);        
    }
    else
    {
        calibWindow->setEnabled(false);
        calibWindowLabel->setEnabled(false);
        thrSpikeDetectorLabel->setEnabled(false);
        thrSpikeDetectorLineEdit->setEnabled(false);
    }
    // check if any more manual stim triggers can be allotted
    if(idx >= StimParameters::KeyPress1 && idx <= StimParameters::KeyPress8)
    {
        if(!(mainWindow)->getSignalProcessorObj()->closedLoopStimTriggersAvailable(idx-StimParameters::KeyPress1))
        {
            triggerSource->setCurrentIndex(StimParameters::DigitalIn1);
            QMessageBox::information(this,tr("Error"),
                                          tr("This trigger source has been already allotted"
                                             "to a closed loop stim channel. Please select another"
                                             "trigger source or disable the closed loop trigger channel."),QMessageBox::Ok);
        }
    }
}

void StimParamDialog::setThrSpikeDetector()
{
    //update the value of the spike detector since the user has entered a new value
    thrSpikeDetector = thrSpikeDetectorLineEdit->text().toDouble();
}

void StimParamDialog::setSpikeDetectorCalibWin(int idx)
{
    //the window values start from 1 and have a one-to-one mapping
    //to the combo box index value
    spikeDetectorCalibWin = calibWindow->currentText().toInt();
}

/* Private slot that calculates the positive and negative charges generated by the current phase amplitudes and durations */
void StimParamDialog::calculateCharge()
{
    //calculate Qmin and Qmax
    double firstCharge = (firstPhaseAmplitude->getTrueValue()) * (firstPhaseDuration->getTrueValue());
    double secondCharge = (secondPhaseAmplitude->getTrueValue()) * (secondPhaseDuration->getTrueValue());
    if (stimShape->currentIndex() == StimParameters::Triphasic)
        firstCharge = firstCharge * 2;
    double Qmin = qMin(firstCharge, secondCharge);
    double Qmax = qMax(firstCharge, secondCharge);

    //calculate imbalance as a percentage
    double imbalance;
    if (Qmax == 0)
        imbalance = 0;
    else
        imbalance = 100 * (1 - Qmin/Qmax);

    //if there's no imbalance (or negligible imbalance due to floating point rounding), display text in a green font
    if (imbalance < .001)
    {
        chargeBalanceLabel->setText("Charge is perfectly balanced.");
        chargeBalanceLabel->setStyleSheet("QLabel {color: green}");
    }

    //if there's an imbalance of less than 10%, display text in a yellow font
    else if (imbalance < 10)
    {
        chargeBalanceLabel->setText("Charge is imbalanced by " + QString::number(imbalance, 'f', 1) + "%.");
        chargeBalanceLabel->setStyleSheet("QLabel {color: orange}");
    }

    //if there's an imbalance of more than 10%, display text in a red font
    else
    {
        chargeBalanceLabel->setText("Charge is imbalanced by " + QString::number(imbalance, 'f', 1) + "%.");
        chargeBalanceLabel->setStyleSheet("QLabel {color: red}");
    }

    double totalPositive, totalNegative;

    //update total positive and negative charge labels
    if (stimPolarity->currentIndex() == StimParameters::PositiveFirst)
    {
        totalPositive = firstCharge;
        totalNegative = secondCharge;
    }
    else
    {
        totalPositive = secondCharge;
        totalNegative = firstCharge;
    }

    if (totalPositive < 999)
        totalPositiveChargeLabel->setText("Total positive charge injected per pulse: " + QString::number(totalPositive, 'f', 1) + " pC");
    else if (totalPositive < 999000)
        totalPositiveChargeLabel->setText("Total positive charge injected per pulse: " + QString::number(totalPositive/1000, 'f', 1) + " nC");
    else
        totalPositiveChargeLabel->setText("Total positive charge injected per pulse: " + QString::number(totalPositive/1000000, 'f', 1) + " " + QSTRING_MU_SYMBOL + "C");

    if (totalNegative < 999)
        totalNegativeChargeLabel->setText("Total negative charge injected per pulse: " + QString::number(totalNegative, 'f', 1) + " pC");
    else if (totalNegative < 999000)
        totalNegativeChargeLabel->setText("Total negative charge injected per pulse: " + QString::number(totalNegative/1000, 'f', 1) + " nC");
    else
        totalNegativeChargeLabel->setText("Total negative charge injected per pulse: " + QString::number(totalNegative/1000000, 'f', 1) + " " + QSTRING_MU_SYMBOL + "C");
}


/* Private slot that calculates the frequency of the pulse train, given the user-selected period */
void StimParamDialog::calculatePulseTrainFrequency()
{
    double frequency;

    if (pulseTrainPeriod->getTrueValue() == 0)
        pulseTrainFrequencyLabel->setText(tr("Pulse Train Frequency: -"));

    else if (pulseTrainPeriod->getTrueValue() < 1000)
    {
        frequency = 1 / pulseTrainPeriod->getTrueValue();
        pulseTrainFrequencyLabel->setText("Pulse Train Frequency: " + QString::number(frequency * 1000, 'f', 2) + " kHz");
    }

    else
    {
        frequency = 1 / pulseTrainPeriod->getTrueValue();
        pulseTrainFrequencyLabel->setText("Pulse Train Frequency: " + QString::number(frequency * 1000000, 'f', 2) + " Hz");
    }
}


/* Private slot that constrains postTriggerDelay's lowest possible value to the current value of preStimAmpSettle */
void StimParamDialog::constrainPostTriggerDelay()
{
    postTriggerDelay->setTrueMinimum(preStimAmpSettle->getTrueValue());
}

/* Private slot that constrains postStimChargeRecovOff's lowest possible value to the current value of postStimChargeRecovOn */
void StimParamDialog::constrainPostStimChargeRecovery()
{
    postStimChargeRecovOff->setTrueMinimum(postStimChargeRecovOn->getTrueValue());
}

/* Private slot that constrains refractoryPeriod's lowest possible value to the higher of postStimAmpSettle and postStimChargeRecovoff
 * In other words, postStimAmpSettle, postStimChargeRecovOff, and postStimChargeRecovOn cannot surpass the value of refractoryPeriod */
void StimParamDialog::constrainRefractoryPeriod()
{
    refractoryPeriod->setTrueMinimum(qMax(postStimAmpSettle->getTrueValue(), postStimChargeRecovOff->getTrueValue()));
}

/* Private slot that constrains pulseTrainPeriod's lowest possible value to the sum of the durations of active phases */
void StimParamDialog::constrainPulseTrainPeriod()
{
    double minimum;
    //if biphasic
    if (stimShape->currentIndex() == StimParameters::Biphasic)
    {
        //minimum equals D1 + D2
        minimum = firstPhaseDuration->getTrueValue() + secondPhaseDuration->getTrueValue();
    }

    //if biphasic with interphase delay
    else if (stimShape->currentIndex() == StimParameters::BiphasicWithInterphaseDelay)
    {
        //minimum equals D1 + D2 + D_int
        minimum = firstPhaseDuration->getTrueValue() + secondPhaseDuration->getTrueValue() + interphaseDelay->getTrueValue();
    }

    //if triphasic
    else if (stimShape->currentIndex() == StimParameters::Triphasic)
    {
        //minimum equals 2*D1 + D2
        minimum = (2 * firstPhaseDuration->getTrueValue()) + secondPhaseDuration->getTrueValue();
    }

    pulseTrainPeriod->setTrueMinimum(minimum);
}


/* Private slot that rounds all TimeSpinBox values to the nearest integer multiple of timestep_us when the user has finished editing them */
void StimParamDialog::roundTimeInputs()
{
    /* for all time inputs, if they are not divisible by timestep_us, round them to the nearest multiple of timestep_us */

    //firstPhaseDuration
    firstPhaseDuration->round();

    //secondPhaseDuration
    secondPhaseDuration->round();

    //interphaseDelay
    interphaseDelay->round();

    //postTriggerDelay
    postTriggerDelay->round();

    //pulseTrainPeriod
    pulseTrainPeriod->round();

    //refractoryPeriod
    refractoryPeriod->round();

    //preStimAmpSettle   
    preStimAmpSettle->round();

    //postStimAmpSettle    
    postStimAmpSettle->round();

    //postStimChargeRecovOn
    postStimChargeRecovOn->round();

    //postStimChargeRecovOff
    postStimChargeRecovOff->round();
}

/* Private slot that rounds all CurrentSpinBox values to the nearest integer multiple of timestep_us when the user has finished editing them */
void StimParamDialog::roundCurrentInputs()
{
    /* for all current inputs, if they are not divisible by currentstep_us, round them to the nearest multiple of currentstep_us */

    //firstPhaseAmplitude
    firstPhaseAmplitude->round();

    //secondPhaseAmplitude
    secondPhaseAmplitude->round();
}
