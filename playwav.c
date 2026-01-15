#include <stdio.h>
#include <stdint.h>
#include <string.h>

// keeping it simple (for now), only try and play WAVs made with the createwav.c file, since other chunks can exist (ie. metadata chunks). 
// once done w/ basic implementation, parse chunnks via strcmp and go from there (fe. if chunk_id == "fmt " => parse fmt chunk
// with that being said, even if going this route, can just ignore metadata chunks and not parse them since they are not integral to the file

/*
struct wav_header {
	char riff[4];
	int32_t riff_chunk_size;  // rcs = fisize - 8 (4 = RIFF, 4 = rcs)
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
	char buf[44];  // header is 44 bytes
	FILE* fp = fopen("test.wav", "rb");
	fread(buf, sizeof(buf), 1, fp);

	// populate wav & header struct?
	// struct wavheader wh;
	
	char riff[5];  // extra byte for null terminator, not sure how this will work for populating wh
	memcpy(riff, &buf[0], 4 * sizeof(buf[0]));
	
	int32_t rcs = 0;
	memcpy(&rcs, &buf[4], sizeof(fsize));
	printf("%d\n", rcs);
	// see comment inside wav_header

	
	// validation, may need to come at beginning of func to prevent buffer under/overflow errors...
	/*
	if() {
		stderr("RIFF header is invalid");
	}
	*/

	return 0;
}
