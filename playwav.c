#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <alsa/asoundlib.h>

// keeping it simple (for now), only try and play WAVs made with the createwav.c file, since other chunks can exist (ie. metadata chunks). 
// once done w/ basic implementation, parse chunnks via strcmp and go from there (fe. if chunk_id == "fmt " => parse fmt chunk
// with that being said, even if going this route, can just ignore metadata chunks and not parse them since they are not integral to the file

struct wav_header {
	char riff[4];
	int32_t riff_chunk_size;  // rcs = fisize - 8 (4 = RIFF, 4 = rcs)
	char wave[4];
	// after wave[4], chunks are in undetermined order
	char fmt[4];
	// not always 16bytes, only if PCM (which should be fine, since MOST wavs are PCM...)
	int32_t chunk_size;  // size of fmt chunk
	int16_t format_tag;
	int16_t num_channels;
	int32_t sample_rate;
	int32_t bytes_sec;
	int16_t bytes_sample;
	int16_t bits_sample;
	char data[4];
	int32_t data_len;  // size of data chunk
};

int main() {
	// open wav & read
	char buf[44];  // header is 44 bytes
	FILE* fp = fopen("test.wav", "rb");
	fread(buf, sizeof(buf), 1, fp);

	char riff[4];  // extra byte for null terminator, not sure how this will work for populating wh
	memcpy(riff, &buf[0], 4 * sizeof(buf[0]));
	
	int32_t r_cs = 0;
	memcpy(&r_cs, &buf[4], sizeof(r_cs));
	// see comment inside wav_header

	char wave[4]; 
	memcpy(wave, &buf[8], 4 * sizeof(buf[0]));

	/* IMPORTANT! PAST ABOVE POINT CHUNKS MAY NOT ALWAYS BE IN THIS ORDER, BUT BECAUSE BASING OFF OF createwav.c DOING IT TJIS WAY SHOULD NOT RESULT IN UNDEFINED BHV */

	char fmt[4];
	memcpy(fmt, &buf[12], 4 * sizeof(buf[0]));

	int32_t fmt_cs = 0;
	memcpy(&fmt_cs, &buf[16], sizeof(fmt_cs));

	int16_t fmt_type = 0;
	memcpy(&fmt_type, &buf[20], sizeof(fmt_type));

	int16_t num_channels = 0;
	memcpy(&num_channels , &buf[22], sizeof(num_channels));
	
	int32_t sample_rate = 0;
	memcpy(&fmt_cs, &buf[24], sizeof(sample_rate));

	// may differ based on wav spec..
	int32_t bytes_sec = 0;
	memcpy(&bytes_sec, &buf[28], sizeof(bytes_sec));
	
	int16_t bytes_sample = 0;
	memcpy(&bytes_sample, &buf[32], sizeof(bytes_sample));
	
	int16_t bits_sample = 0;
	memcpy(&bits_sample, &buf[34], sizeof(bits_sample));
	
	char data[4];
	memcpy(data, &buf[36], 4 * sizeof(buf[0]));
	
	int32_t data_len = 0;  // size of data chunk
	memcpy(&data_len, &buf[40], sizeof(data_len));


	struct wav_header wh;

	strncpy(wh.riff, riff, 4);
	strncpy(wh.wave, wave, 4);
	strncpy(wh.fmt, fmt, 4);
	strncpy(wh.data, data, 4);
	wh.riff_chunk_size = r_cs;
	wh.format_tag = fmt_type;
	wh.num_channels = num_channels;
	wh.sample_rate = sample_rate;
	wh.bits_sample = bits_sample;
	wh.bytes_sample = bytes_sample; 
	wh.bytes_sec = bytes_sec;
	
	// use alsa
	int rc = 0;
	snd_pcm_t* playback_handle;
	snd_pcm_hw_params_t* hw_params;

	// default sound device == 0,0
	rc = snd_pcm_open(&playback_handle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "Could not open default audio device");
		return 1;
		// note if i move this to another file use exit to immediately kill program lel
	}

	// validation, may need to come at beginning of func to prevent buffer under/overflow errors...
	/*
	if() {
		stderr("RIFF header is invalid");
	}
	*/

	return 0;
}
