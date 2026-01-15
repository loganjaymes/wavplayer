#include <stdio.h>
#include <stdint.h>

/*
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
*/

int main() {
	// open wav & read
	unsigned char buf[44];  // header is 44 bytes
	FILE* fp = fopen("test.wav", "rb");
	fread(buf, sizeof(buf), 1, fp);

	for(int i = 0; i < 44; ++i) {
		printf("%x ", buf[i]);
	}
	printf("\n");

	// validate RIFF and WAVE
	// 


	return 0;
}
