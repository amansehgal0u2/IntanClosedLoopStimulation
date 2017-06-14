close all;
%% filter design
fmin = 300;            
fmax = 3000;             
fs = 20000; 
N = 100;
f = [0 295 300 3000 3005 fs/2]/(fs/2);
m = [0 0 1 1 0 0];
bfir = fir2(N,f,m,gausswin(N+1,1));
figure
freqz(bfir,1,2048);

Wn = [ 300 3000]/(fs/2);
b = fir1(N,Wn,'bandpass');
% figure
% freqz(b,1);
% print the coefficients as a string out to the terminal formatted as a
% string
% sprintf('%10.40f,',b)
% [bbutt,abutt]=butter(2, [fmin fmax]/(fs/2));
% figure
% freqz(bbutt,abutt,2048);

%% apply filter to spike
spike = synthSpike();
filt_spike=filter(b,1,spike);

plot(spike);hold on;plot(filt_spike);axis tight;
legend('raw spike signal','filtered spike signal');