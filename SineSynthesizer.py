import pyaudio
import numpy as np
import sys

args = sys.argv[1:]

# Input:
# [strength] [freq (Hz)] [strength] [freq (Hz)] etc.
if(len(args) % 2 != 0):
    raise ValueError("Arguments in incorrect format.")

fs = 44100
duration = 1.0
volume = 0.5

output = np.empty(int(fs*duration), np.float32)

for i in range(0,len(args),2):
    strength = float(args[i])
    freq = int(args[i+1])
    samples = (np.sin(2*np.pi*np.arange(fs*duration)*freq/fs)).astype(np.float32)
    samples = strength * samples
    output = output + samples

output = output / np.amax(output)
output = output * volume

p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paFloat32, channels = 1, rate = fs, output = True)
stream.write(output)
stream.stop_stream()
stream.close()
p.terminate()