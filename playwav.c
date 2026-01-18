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
	int32_t fmt_chunk_size;  // size of fmt chunk
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
	// TODO: locate all chunks below and filter out others (since things like metadata do not matter)

	char fmt[4];
	memcpy(fmt, &buf[12], 4 * sizeof(buf[0]));

	int32_t fmt_cs = 0;
	memcpy(&fmt_cs, &buf[16], sizeof(fmt_cs));

	int16_t fmt_type = 0;
	memcpy(&fmt_type, &buf[20], sizeof(fmt_type));

	int16_t num_channels = 0;
	memcpy(&num_channels , &buf[22], sizeof(num_channels));
	
	int32_t sample_rate = 0;
	memcpy(&sample_rate, &buf[24], sizeof(sample_rate));

	// may differ based on wav spec..
	int32_t bytes_sec = 0;
	memcpy(&bytes_sec, &buf[28], sizeof(bytes_sec));  // byterate
	
	int16_t bytes_sample = 0;
	memcpy(&bytes_sample, &buf[32], sizeof(bytes_sample));  // blockalign
	
	int16_t bits_sample = 0;
	memcpy(&bits_sample, &buf[34], sizeof(bits_sample));
	
	char data[4];
	memcpy(data, &buf[36], 4 * sizeof(buf[0]));
	
	int32_t data_len = 0;  // size of data chunk
	memcpy(&data_len, &buf[40], sizeof(data_len));

	unsigned char* data_buf = malloc(data_len);
	if (fread(data_buf, 1, data_len, fp) != data_len) {
		fprintf(stderr, "Failde to read data");
		free(data_buf);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	// const int dur_sec = data_len / (sample_rate * bytes_sample);
	// const int data_buf_size = sample_rate * dur_sec;  // buffer size 
	// short int* data_info = malloc(data_buf_size);

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
	wh.data_len = data_len;
	
	// use alsa
	// TODO: Error handling
	int size = 0;
	snd_pcm_t* playback_handle;
	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
	snd_pcm_hw_params_t* hw_params;
	unsigned int val = 0;
	snd_pcm_uframes_t frame;
	char* buff;
	snd_pcm_hw_params_malloc(&hw_params);

	// default sound device == 0,0
	// TODO: replace "plughw:0,0" w/ char* pcm_name; (using default is simplest for now tho...
	if (snd_pcm_open(&playback_handle, "plughw:0,0", stream, 0) < 0) {
		fprintf(stderr, "Could not open default audio device\n");
		return -1;
		// note if i move this to another file use EXIT and not return to immediately kill program lel
	}

	if (snd_pcm_hw_params_any(playback_handle, hw_params) < 0) {
		fprintf(stderr, "Could not configure hardware\n");
		return -1;
	}

	// https://vovkos.github.io/doxyrest/samples/alsa/group_PCM.html#doxid-group-p-c-m-1ga45d50841b307f2156fce1857bfac228c 
	// configuring hw to run wave file
	int pd = 2;
	snd_pcm_uframes_t pd_size = 8192;
	int16_t frames_size = num_channels * bytes_sample;  // alsa reads in frames (segments of wav), not samples or bytes
	
	//unsigned int latency = pd_size * pd / (sample_rate * bytes_sample); 
	unsigned int latency = 500000;

	snd_pcm_set_params(playback_handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, num_channels, sample_rate, 1, latency); 

	unsigned int num_frames = data_len / frames_size;
	// start playback
	// snd_pcm_sframes_t snd_pcm_writei(playback_handle, data_info, num_frames);

	unsigned char* d_ptr = data_buf;
	snd_pcm_sframes_t frames_left = data_len / frames_size;
	
	while (frames_left > 0) {
		snd_pcm_sframes_t written = snd_pcm_writei(playback_handle, d_ptr, frames_left);

		if (written == -EPIPE) {  // buffer underrun
			snd_pcm_prepare(playback_handle);
			continue;
		} 
		if (written < 0) {
			fprintf(stderr, "write error: %s\n", snd_strerror(written));
			break;
		}

		d_ptr += written * frames_size;
		frames_left -= written;
	}


	snd_pcm_drain(playback_handle);

	

#if 0
	snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S32_LE);  // checked w/ pw-top 
	snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_channels(playback_handle, hw_params, num_channels);
	
	// set sample rate (most audio files are 44100, but my soundcard is 48000 for playback
	int hw_rate = sample_rate;
	snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &hw_rate, 0);
	if (hw_rate != sample_rate) {
		fprintf(stderr, "The rate %d is unsupported, using %d instead\n", hw_rate, sample_rate);  // not an issue, since most modern hardware can support most common sample rates (ie. 44100, 48000, etc.)
	}


	// no idea
	int periods = 2;
	snd_pcm_hw_params_set_periods(playback_handle, hw_params, periods, 0)

	

	snd_pcm_uframes_t pd_size = 8192;  // bytes
#endif

	// TODO: validate wav file (ie. RIFF format)
	// TODO: validate alsa device
	free(data_buf);
	snd_pcm_hw_params_free(hw_params);
	return 0;
}
