/*
 * gbsplay is a Gameboy sound player
 *
 * SDL2 sound output plugin
 *
 * 2020 (C) by Christian Garbs <mitch@cgarbs.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#include <time.h>
#include <SDL.h>

#include "common.h"
#include "plugout.h"

#define PLAYBACK_MODE      0
#define NO_CHANGES_ALLOWED 0
#define UNPAUSE            0

#ifdef SDL_AUDIO_ALLOW_SAMPLES_CHANGE
	#define SDL_FLAGS SDL_AUDIO_ALLOW_SAMPLES_CHANGE
#else
	#define SDL_FLAGS NO_CHANGES_ALLOWED
#endif

int device;
SDL_AudioSpec obtained;

static long sdl_open(enum plugout_endian endian, long rate, long *buffer_bytes)
{
	SDL_AudioSpec desired;
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, _("Could not init SDL: %s\n"), SDL_GetError());
		return -1;
	}

	SDL_zero(desired);
	desired.freq = rate;
	desired.channels = 2;
	desired.samples = 1024;
	desired.callback = NULL;

	switch (endian) {
	case PLUGOUT_ENDIAN_BIG:    desired.format = AUDIO_S16MSB; break;
	case PLUGOUT_ENDIAN_LITTLE: desired.format = AUDIO_S16LSB; break;
	case PLUGOUT_ENDIAN_NATIVE: desired.format = AUDIO_S16SYS; break;
	}

	device = SDL_OpenAudioDevice(NULL, PLAYBACK_MODE, &desired, &obtained, SDL_FLAGS);
	if (device == 0) {
		fprintf(stderr, _("Could not open SDL audio device: %s\n"), SDL_GetError());
		return -1;
	}

	SDL_PauseAudioDevice(device, UNPAUSE);

	*buffer_bytes = obtained.samples * 4;
	return 0;
}

static ssize_t sdl_write(const void *buf, size_t count)
{
	int overqueued = SDL_GetQueuedAudioSize(device) - obtained.size;
	float delaynanos = (float)overqueued / 4.0 / obtained.freq * 1000000000.0;
	struct timespec interval = {.tv_sec = 0, .tv_nsec = (long)delaynanos};
	if (overqueued > 0) {
		nanosleep(&interval, NULL);
	}
	if (SDL_QueueAudio(device, buf, count) != 0) {
		fprintf(stderr, _("Could not write SDL audio data: %s\n"), SDL_GetError());
		return -1;
	}
	return count;
}

static void sdl_close()
{
	SDL_Quit();
}

const struct output_plugin plugout_sdl = {
	.name = "sdl",
	.description = "SDL sound driver",
	.open = sdl_open,
	.write = sdl_write,
	.close = sdl_close,
};
