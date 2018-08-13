# Description
* Extension of Intan Technology's neuro-electrophysiology tool for spike detection that 
enables feedback for closed loop stimulation based on spike detection as mentioned in the paper:

[1] *Unsupervised Spike Detection and Sorting with Wavelets and Superparamagnetic Clustering* , R. Quian Quiroga, Z. Nadasdy, Y. Ben-Shaul

* Also implemented are a spike-band filter in the range 300-3000Hz and a real-time spike discriminator tool that uses voltage and time constraints to sort spikes.

## Filter Design and Simulation

The `design_simulation` folder contains MATLAB files that can be used to simulate neural spikes at a specific sampling frequency and design filters to isolate the signal with a specific frequency content.
