/*
  EEPROMNode.cpp - Part of TRACER

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

#include "EEPROMTree.h"


EEPROMTree::EEPROMTree(string_t *name, uint8_t index, float *const data, uint8_t nb_children) :

        name(name), index(index), data(data), nb_children(nb_children), children(new EEPROMTree*[nb_children])

{}

EEPROMTree::~EEPROMTree() {

    //For every sub_tree :
    for (uint8_t sub_tree = 0; sub_tree < nb_children; sub_tree++) {

        //Delete the sub_tree;
        delete children[sub_tree];

    }

    //delete the nb_children array
    delete[] children;

}
