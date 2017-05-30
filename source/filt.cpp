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
 * USAGE:
 * while(data_to_be_filtered){
 * 	x = // Get next sample
 * 	y = filter->do_sample( x );
 */

#include "filt.h"
#include "spikebandfiltercoeffs.h"


// initialize the filter coefficients and the filter
// buffer window.
FIRFilter::FIRFilter()
{
    m_num_taps = MAX_NUM_FILTER_TAPS;

    spikeBandFilterSamplingFreqIdx = 2; //30Ks/s
    m_sr = nullptr;
    m_sr = new double [m_num_taps];
    spikeBandFIRCoeffs[2]=SpikeBandFilerCoeffs_30ks;
    spikeBandFIRCoeffs[1]=SpikeBandFilerCoeffs_25ks;
    spikeBandFIRCoeffs[0]=SpikeBandFilerCoeffs_20ks;

    m_taps = spikeBandFIRCoeffs[spikeBandFilterSamplingFreqIdx];
    // initialize filter signal buffer to 0;
    for(int i = 0; i < m_num_taps; i++)
    {
        m_sr[i] = 0;
    }
}

// free allocated memory for the filter taps and buffer window.
FIRFilter::~FIRFilter()
{
    if( m_sr) delete [] m_sr;
}

// update the sampling frequency index for the filter
void FIRFilter::setSpikeBandFilterSamplingFreq(unsigned int sr)
{
    this->spikeBandFilterSamplingFreqIdx = sr;
    m_taps = spikeBandFIRCoeffs[spikeBandFilterSamplingFreqIdx];
}

// apply the filter
double FIRFilter::filter(double x)
{
    double result =0.0;
    for(int i = m_num_taps - 1; i >= 1; i--)
    {
		m_sr[i] = m_sr[i-1];
	}	
    m_sr[0] = x;
    for(int i = 0; i < m_num_taps; i++)
    {
        result += m_sr[i] * m_taps[i];
    }
	return result;
}
