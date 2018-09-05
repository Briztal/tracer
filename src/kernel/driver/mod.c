/*
  driver.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

#include <kernel/fs/dfs.h>
#include "driver.h"


//Register a driver;
void register_driver(const char *name, struct kernel_driver *driver) {

	//Create a file representing the driver;
	dfs_create(name, DFS_DRIVER, driver);

}



#endif //TRACER_DRIVER_H
