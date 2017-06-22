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
 * 	x = // Get next sample
 * 	y = <filter_object>->filter( x, stream, channel );
 */

#include "filt.h"
#include "spikebandfiltercoeffs.h"


// initialize the filter coefficients and the filter
// buffer window.
FIRFilter::FIRFilter()
{
    m_num_taps = MAX_NUM_FILTER_TAPS;

    spikeBandFilterSamplingFreqIdx = 2; //30Ks/s
    spikeBandFIRCoeffs[2]=SpikeBandFilerCoeffs_30ks;
    spikeBandFIRCoeffs[1]=SpikeBandFilerCoeffs_25ks;
    spikeBandFIRCoeffs[0]=SpikeBandFilerCoeffs_20ks;

    m_taps = spikeBandFIRCoeffs[spikeBandFilterSamplingFreqIdx];
}

void FIRFilter::initFilterVectors(unsigned int num_streams , unsigned int channels_per_stream)
{
    int i, j;
    if (num_streams == 0)
    {
        return;
    }
    // outermost vector is number of streams of which there are a max of 8
    m_sr.resize(num_streams);
    for (i = 0; i < num_streams; ++i)
    {
        // inner vector is channels per stream of which there are 16
        m_sr[i].resize(channels_per_stream);
        for (j = 0; j < channels_per_stream; ++j)
        {
            // each channel has a corresponding filter sample window that holds the sampled data
            m_sr[i][j].resize(m_num_taps);
        }
    }
    // initial condition for filter sample window is 0
    for (i = 0; i < num_streams; ++i)
    {
        for (j = 0; j < channels_per_stream; ++j)
        {
            m_sr[i][j].fill(0.0);
        }
    }
}

// free allocated memory for the filter taps and buffer window.
FIRFilter::~FIRFilter()
{

}

// update the sampling frequency index for the filter
void FIRFilter::setSpikeBandFilterSamplingFreq(unsigned int sr)
{
    this->spikeBandFilterSamplingFreqIdx = sr;
    m_taps = spikeBandFIRCoeffs[spikeBandFilterSamplingFreqIdx];
}

// apply the filter to the given stream and channel
double FIRFilter::filter(double x, unsigned int stream, unsigned int channel)
{
    double y = 0.0;
    // propagate the samples through the window
    for(int i = m_num_taps - 1; i >= 1; i--)
    {
        m_sr[stream][channel][i] = m_sr.at(stream).at(channel).at(i-1);
    }
    // assign new data sample
    m_sr[stream][channel][0] = x;
    // run the filter using the FIR coeffs
    for(int i = 0; i < m_num_taps; i++)
    {
        y += m_sr.at(stream).at(channel).at(i) * m_taps[i];
    }
    return y;
}
