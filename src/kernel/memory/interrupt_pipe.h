/*
  interrupt_pipe.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TRACER_INTERRUPT_PIPE_H
#define TRACER_INTERRUPT_PIPE_H

#include "memory_stream.h"


//Predeclare the interrupt pipe structure;
struct interrupt_pipe_t;

//Typedef the transfer function pointer type for readability;
typedef void (*interrupt_pipe_transfert_t)(struct interrupt_pipe_t *const pipe, struct stream_t *const src,
										   struct stream_t *const dst, const size_t nb_blocks);

struct interrupt_pipe_t {

	//The base stream;
	struct stream_pipe_t pipe;

	//The state flag. Set when the transfer is active;
	volatile bool transfer_active;

	//The provided transfer function;
	interrupt_pipe_transfert_t transfer_function;

	//If the optimised transfer function is used, this function will be used to transfer blocs efficiently;
	void (*mmap_transfer)(struct mem_map_t src, struct mem_map_t dst, size_t block_size, size_t nb_blocks);

	//Delete the interrupt pipe's dynamic data; Called by pipe only;
	void (*const destructor)(struct interrupt_pipe_t *);

};


//Initialise an interrupt pipe, providing the owner and the
void interrupt_pipe_initialise(struct interrupt_pipe_t *pipe,
							   struct stream_t *owner,
							   void (*update_watermark)(struct interrupt_pipe_t *, size_t),
							   void (*enable_interupt)(struct interrupt_pipe_t *),
							   void (*disable_interupt)(struct interrupt_pipe_t *),
							   void (*destructor)(struct interrupt_pipe_t *));

//Execute the pipe transfer;
void interrupt_pipe_transfer(struct interrupt_pipe_t *pipe);


#endif //TRACER_INTERRUPT_PIPE_H
