/*
  amp.h Part of nostd

  Copyright (c) 2018 Raphaël Outhier

  nostd is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  nostd is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with nostd.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NOSTD_AMP_H
#define NOSTD_AMP_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>
#include <error.h>

#include "list.h"

#include "mpool.h"



/*The amp channel type;*/
#define amp_channel_t size_t

/*The value for an invalid channel;*/
#define AMP_CHANNEL_INVALID ((amp_channel_t) (-1))

struct amp_allocator {

    /*Error flags;*/
    size_t a_error;

    /*The number of channels;*/
    amp_channel_t a_nb_channels;

    /*The first available channel;*/
    void *a_available_channel;

    /*The channels array;*/
    void *a_channels;

    /*TODO MUTEX FOR THE WHOLE ALLOCATOR*/

};


/*If set, attempted to reserve a channel with no channels available;*/
#define AMP_ERROR_CHANNEL_RESERVATION_FAILURE   ((uint32_t)    (1 << 0))

/*If set, a non available channel was found in the available list;*/
#define AMP_ERROR_CHANNEL_AVAILABILITY_ERROR   ((uint32_t)    (1 << 1))

/*If set, during the release of a channel, the provided index was invalid;*/
#define AMP_ERROR_RELEASE_INDEX_ERROR   ((uint32_t)    (1 << 2))

/*If set, during the release of a channel, the provided index lead to an available chanel;*/
#define AMP_ERROR_RELEASE_AVAILABILITY   ((uint32_t)    (1 << 3))

/*If set, during the release of a channel, the provided index was invalid;*/
#define AMP_ERROR_ALLOC_INDEX   ((uint32_t)    (1 << 4))

/*If set, during the release of a channel, the provided index lead to an available chanel;*/
#define AMP_ERROR_ALLOC_AVAILABILITY   ((uint32_t)    (1 << 5))

/*If set, during the release of a channel, the provided index was invalid;*/
#define AMP_ERROR_FREE_INDEX   ((uint32_t)    (1 << 6))

/*If set, during the release of a channel, the provided index lead to an available chanel;*/
#define AMP_ERROR_FREE_AVAILABILITY   ((uint32_t)    (1 << 7))



/*Construct a memory provider; thread unsafe*/
error_t amp_ctor(struct amp_allocator *amp, size_t nb_channels);

/*Destruct a memory provider; thread unsafe*/
void amp_dtor(struct amp_allocator *amp);


/*Return the number of available channels; thread safe;*/
size_t amp_available_channels(struct amp_allocator *amp);

/*Reserve a channel; thread safe*/
size_t amp_reserve_channel(struct amp_allocator *amp, size_t object_size, size_t slab_size);

/*Release a channel; thread safe*/
error_t amp_release_channel(struct amp_allocator *amp, size_t client_id);


/*Return how many objects can be allocated in a channel;*/
size_t amp_available_blocks(struct amp_allocator *amp, size_t channel_id);

/*Allocate an object in a channel;*/
void *amp_alloc_block(struct amp_allocator *amp, size_t channel_id);

/*Free an object in a channel;*/
void amp_free_block(struct amp_allocator *amp, size_t channel_id, void *block);


#endif /*NOSTD_AMP_H*/
