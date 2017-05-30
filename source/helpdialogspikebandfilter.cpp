//  ------------------------------------------------------------------------
//
//  This file is part of the Intan Technologies RHS2000 GUI
//  Version 1.01
//  Copyright (C) 2017 University of Pennsylvania
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

#include "helpdialogspikebandfilter.h"

// Software spike band filter help dialog window

// construct the dialog window
HelpDialogSpikeBandFilter::HelpDialogSpikeBandFilter(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Software Real-Time Spike Band Filter"));

    QPixmap image;
    image.load(":/images/spikebandFir.png", "PNG");
    QLabel *imageLabel = new QLabel();
    imageLabel->setPixmap(image);

    QLabel *label1 = new QLabel(tr("A software implementation of a spike band filter that is implemented as a FIR filter."
                                   "The 101 point FIR filter is designed with the MATLAB code:<br>"
                                   "<tt>fs = 30000; 30ks/s <br>"
                                   "N = 100; FIR design window size <br>"
                                   "f = [0 295 300 3000 3005 fs/2]/(fs/2); passband of 300 - 3000 Hz<br>"
                                   "m = [0 0 1 1 0 0];<br>"
                                   "bfir = fir2(N,f,m,gausswin(N+1,1)); uses gaussian window with STD of 1 </tt><br>"));
    label1->setWordWrap(true);

    QLabel *label2 = new QLabel(tr("The diagram below shows the Frequency response and phase response of the filter<br>"));
    label2->setWordWrap(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label1);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(imageLabel);

    setLayout(mainLayout);
}
