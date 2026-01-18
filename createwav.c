#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdint.h"

// define notes
// to change octave, either div or mult by a factor of 2 (*2 => octave above, /2 => octave below)
#define C4 261.63
#define Db4 277.18
#define D4 293.66
#define Eb4 411.13
#define E4 329.63
#define F4 349.23
#define Gb4 369.99
#define G4 392.0
#define Ab4 415.30
#define A4 440.0
#define Bb4 466.16
#define B4 493.88


// create wav header: https://docs.fileformat.com/audio/wav/ 
struct wav_header {
	char riff[4];
	int32_t file_size;
	char wave[4];
	char fmt[4];
	int32_t chunk_size;
	int16_t format_tag;
	int16_t num_channels;
	int32_t sample_rate;
	int32_t bytes_sec;
	int16_t bytes_sample;
	int16_t bits_sample;
	char data[4];
	int32_t data_len;
};

int main() {
	const int sample_rate = 44100;

	struct wav_header wh;

	strncpy(wh.riff, "RIFF", 4);
	strncpy(wh.wave, "WAVE", 4);
	strncpy(wh.fmt, "fmt ", 4);
	strncpy(wh.data, "data", 4);

	wh.chunk_size = 16;
	wh.format_tag = 1;
	wh.num_channels = 1;
	wh.sample_rate = sample_rate;
	wh.bits_sample = 16;
	wh.bytes_sample = (wh.bits_sample * wh.num_channels) / 8;
	wh.bytes_sec = wh.sample_rate * wh.bytes_sample;

	// TODO: split chords by second(s) (or some other metric)
	const int dur_sec = 8; // audio len
	const int buf_size = sample_rate * dur_sec;

	wh.data_len = buf_size * wh.bytes_sample;
	wh.file_size = wh.data_len + 36;

	// write notes to wav
	short int buf[buf_size] = {};

	for (int i = 0; i < buf_size; ++i) {
		buf[i] += (short int)(1000 * sin((M_PI * A3 * i) / sample_rate));
		buf[i] += (short int)(1000 * sin((M_PI * E3 * i) / sample_rate));
	}

	// write to file
	FILE* fp = fopen("pchordtest.wav", "wb");
	fwrite(&wh, 1, sizeof(wh), fp);
	fwrite(buf, 2, buf_size, fp);
	fclose(fp);

	return 0;
}
