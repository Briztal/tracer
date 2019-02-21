/*
  tree.h Part of nostd

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


#ifndef NOSTD_TREE_H
#define NOSTD_TREE_H

/*
 * tree_head : defines the minimal struct to manipulate trees;
 */

struct tree_head {

	/*The tree node's parent;*/
	struct tree_head *t_parent;

	/*The first chilf of the tree node;*/
	struct tree_head *t_child;

	/*The list of the tree's siblings (that share the same parent);*/
	struct list_head *t_siblings;

};

#endif /*NOSTD_TREE_H*/
