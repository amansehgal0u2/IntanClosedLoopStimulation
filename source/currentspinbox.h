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

#ifndef CURRENTSPINBOX_H
#define CURRENTSPINBOX_H

/* Current Spin Box is a widget containing a double spin box, customized to display current values from nA to mA */

#include <QWidget>
#include "globalconstants.h"

class QDoubleSpinBox;

class CurrentSpinBox : public QWidget
{
    Q_OBJECT
public:
    explicit CurrentSpinBox(double currentstep_uA, QWidget *parent = 0);
    void setRange(double minimum, double maximum);
    double getTrueValue();
    void setTrueMinimum(double min_us);
    QWidget* pointer();

public slots:
    void setValue(double val);
    void loadValue (double val);
    void round();

private:
    QDoubleSpinBox *doubleSpinBox;
    double value_uA, currentstep;

private slots:
    void scaleUnits(double val_uA);
    void sendSignalValueMicroA(double val);

signals:
    //pass-through signal that is emitted when the internal Double Spin Box has been edited
    void editingFinished();

    //pass-through signal that is emitted when the internal Double Spin Box's value has changed
    void valueChanged(double);

    //signal that is emitted when the internal Double Spin Box's value has changed, with the parameter always in microamps
    void trueValueChanged(double);

};

#endif // CURRENTSPINBOX_H
