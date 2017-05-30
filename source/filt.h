/*
 * FIR filter, by Aman Sehgal
 * Copyright (c) 2017, University of Pennsylvania
 * Center of Neuroengineeirng and Therapeutics
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * USAGE
 * while(data_to_be_filtered){
 * 	x = // Get next sample
 * 	y = filter->do_sample( x );
 */

#ifndef _FILTER_H
#define _FILTER_H

#include <math.h>
#include <stdlib.h>
#include <unistd.h>


class FIRFilter{
	private:
		int m_num_taps;
		double *m_taps;
		double *m_sr;
        unsigned int spikeBandFilterSamplingFreqIdx;
        double *spikeBandFIRCoeffs[3];
	public:
        void setSpikeBandFilterSamplingFreq(unsigned int sr);
        FIRFilter();
        ~FIRFilter( );
        double filter(double x);
};

#endif
