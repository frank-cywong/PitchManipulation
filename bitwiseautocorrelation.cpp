#include "AudioFile.h"
#include <iostream>
#include <bitset>
#include <climits>

using namespace std;

struct crossing{
	double starttime;
	double endtime;
	double maxval;
};

const double window = 0.02; // 1/50th, can detect as low as 100Hz
const double limitpercent = 0.5;

double reqvolume;
int sampleRate;

vector<crossing*> crossings;

void getBits(bitset<10000>* crossingbits, int shift){
	for(auto it = crossings.begin(); it != crossings.end(); it++){
		crossing* curcrossing = *it;
		//cout << reqvolume;
		//cout << curcrossing->maxval;
		if(curcrossing->maxval < reqvolume){
			//cout << "test";
		}
		//cout << curcrossing -> starttime;
		//cout << curcrossing -> endtime;
		for(int i = ((int)(curcrossing->starttime * sampleRate)) + 1; i <= (int)(curcrossing->endtime * sampleRate); i++){
			crossingbits->set(i);
			//cout << "test2";
		}
	}
	*crossingbits <<= shift;
}

int main(){
	AudioFile<double> audioFile;
	audioFile.load("5-440-1-880.wav");
	sampleRate = audioFile.getSampleRate();
	int windowsize = sampleRate * window;
	//audioFile.printSummary();
	const int channel = 0;
	double prevsample = 0;
	crossing* curcrossing;
	bool incrossing = false;
	double dx = 1 / (double)sampleRate;
	double maxvolume = 0;
	for(int i = 0; i < windowsize; i++){
		double cursample = audioFile.samples[channel][i];
		if(incrossing){
			if(cursample < 0){
				if(curcrossing->maxval > maxvolume){
					maxvolume = curcrossing->maxval;
				}
				double dy = cursample - prevsample;
				double t = ((double)i - (cursample / dy)) * dx;
				curcrossing->endtime = t;
				crossings.push_back(curcrossing);
				incrossing = false;
			}
			else if(cursample > curcrossing->maxval){
				curcrossing->maxval = cursample;
			}
		} else if (!incrossing && prevsample <= 0 && cursample > 0){
			curcrossing = new crossing();
			double dy = cursample - prevsample;
			double t = ((double)i - (cursample / dy)) * dx;
			curcrossing->starttime = t;
			curcrossing->maxval = cursample;
			incrossing = true;
		}
		prevsample = cursample;
	}
	reqvolume = maxvolume * limitpercent;
	bitset<10000> basecrossingbits;
	getBits(&basecrossingbits, 0);
	cout << "base loaded";
	cout << basecrossingbits;
	int cur_min_xor = INT_MAX;
	int min_xor_pos = 0;
	int temp_bit_count = 0;
	bitset<10000> curtest;
	for(int i = 10; i < windowsize; i++){
		curtest.reset();
		getBits(&curtest, i);
		curtest ^= basecrossingbits;
		temp_bit_count = (int)(curtest.count());
		if(temp_bit_count < cur_min_xor){
			cout << "set min xor to: " << temp_bit_count << endl;
			cur_min_xor = temp_bit_count;
			min_xor_pos = i;
		}
		cout << i << endl;
	}
	cout << "min xor pos: " << min_xor_pos << endl;
	cout << "calculated freq: " << (double)sampleRate / (double)min_xor_pos;
	return 0;
}
