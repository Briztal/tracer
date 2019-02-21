/*
  error.h Part of nostd

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

#ifndef NOSTD_ERROR_H
#define NOSTD_ERROR_H

/*An error code fits on a single byte;*/
#define error_t uint8_t

/*A null error code signals that no error happened;*/
#define NO_ERROR ((error_t) 0)

/*A null error code signals that no error happened;*/
#define DEFAULT_ERROR ((error_t) 1)


/*
 * HANDLE_ERROR : shortcut for handling standard errors (boolean error code);
 *
 * @param expr : an expression that evaluates as an error code;
 * @param error_label : a label where to jump if the expression in not evaluated to zero;
 *
 * @evaluates to void;
 */

#define HANDLE_ERROR(expr, error_label) {if ((expr)) goto error_label;}


#endif /*NOSTD_ERROR_H*/
