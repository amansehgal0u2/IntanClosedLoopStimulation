function [spike] = synthSpike()
% function to synthesize and simulate neural spikes
%% simulated spike params
sampleRate = 30000;                         % sampling freuency for the signal
synthSpikeAmplitude = rand()*500.0 - 400.0; % random spike peak amplitude value
synthSpikeDuration = rand()*2 +0.5; 
spikeDelay = rand()*0.5;                    % random jitter time
s=0:160;                                    % number of samples

%% simulate spike as a decaying low freq sinusoidal 
tStepMsec = 1000.0 / sampleRate;  % sampling period in ms
spike = synthSpikeAmplitude.* exp(-2.0.*(s.*tStepMsec - spikeDelay)).* ...
    sin(2.*pi.*(s.*tStepMsec - spikeDelay)./ synthSpikeDuration);
%% detect zero crossings
dir = diff(sign(spike(1:16)));
up = find(dir==-2);
down = find(dir==2);

%% zero out all data prior to the first zero cross to mimic a 0V baseline
if (~isempty(up))
    spike(1:up) =0;
else
    spike(1:down) =0;
end
%% remove the first most prominent peak when mulitple peaks are present
[~,loc]=findpeaks(abs(spike),'MinPeakHeight',10);
if length(loc) >3
    [~,argmin]=min(abs(spike(loc(1):loc(2))));
    spike(1:loc(1)+argmin-1)=0;
end
spike=spike+randn(1,length(spike));
plot(s*tStepMsec,spike);axis tight;
xlabel('time (ms)');
ylabel('amplitude');