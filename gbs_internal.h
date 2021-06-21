/*
 * gbsplay is a Gameboy sound player
 *
 * 2021 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#ifndef _GBS_INTERNAL_API_H_
#define _GBS_INTERNAL_API_H_

#include "common.h"

#include <stdint.h>
#include <stdlib.h>

#include "libgbs.h"

/***
 ***  THIS IS THE INTERNAL API AND MAY CHANGE FREELY BETWEEN VERSION.
 ***/

typedef const uint8_t* (get_bootrom_fn)(void);
typedef void (write_rom_fn)(struct gbs *gbs, FILE *out, const uint8_t *logo_data);

struct gbs_internal_api {
	const char *version;
	get_bootrom_fn *get_bootrom;
	write_rom_fn *write_rom;
};

extern struct gbs_internal_api gbs_internal_api;

static inline void assert_internal_api_valid() {
	if (strcmp(gbs_internal_api.version, GBS_VERSION) != 0) {
		fprintf(stderr, _("Bad libgbs API version, want %s but got: %s"), GBS_VERSION, gbs_internal_api.version);
		exit(1);
	}
};

#endif
