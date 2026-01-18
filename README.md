<h1>
<p align="center">
  <br>SWAP
</h1>
  <p align="center">
    <br />
	SWAP is a Simple Wav Audio Player.
    <br />
    <a href="#about">About</a>
    <a href="#installation">Installation and Usage</a>
    <a href="#references">References</a>
  </p>
</p>

## About
SWAP is a Simple Wav Audio Player written for LINUX devices entirely in C. 
It uses LINUX's built in ALSA (Advanced Linux Sound Architecture) library to access the hardware's soundcard, writing information from a WAV file to it directly (as of right now, it only CONSISTENTLY supports those created with the `createwav.c` file). </br>
Reading other .WAV files is a feature I look towards including soon, since it should be a simple fix of parsing the corresponding file's RIFF container for the relevant chunks (format and data), while excluding others (such as any irrelevant metadata). </br>
Alongside this, the audio created and read by the `*.c` files is for mono (single channel) output. Reading stereo data might be easier to implement than expected (since it is more common in documentation/cookbooks online), but I would ideally like to implement creating stereo files before this for testing purposes.

## Installation
0. Ensure you are on an OS that uses the LINUX kernel.
1. Clone the repository.
2. Edit any changes you want for your .WAV in `createwav.c` (A is CLI coming... eventually ;]..).
3. Edit the filename in both `createwav.c` and `playwav.c` so the file is written and read correctly.
4. Compile both files using `make`
5. Run both `create` and `play`.

## References
1. Create WAV header: https://docs.fileformat.com/audio/wav/ (although there are some nuances that are not gone over)
2. A slightly more nuanced version of above: https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
3. ALSA/PCM documentation: https://vovkos.github.io/doxyrest/samples/alsa/group_PCM.html#doxid-group-p-c-m-1ga45d50841b307f2156fce1857bfac228c 
4. VERY good ALSA library setup guide: https://alsamodular.sourceforge.net/alsa_programming_howto.html#sect02
5. Another setup guide: https://www.linuxjournal.com/article/6735
