// Copyright 2020 Bonedaddy (Alexandre Trottier)
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! @file colors.h
 * @brief macros and utilities for printing color to stdout
 * from https://www.quora.com/How-do-I-print-a-colored-output-in-C
 * Pass a `COLORS_x` enum value into `print_colored` and the color will be printed on
 * stdout Note that if you want to disable printing of that color you'll have to send
 * the `COLORS_RESET` enum value through to make non-bold change 0->1 (0;31m red) vs
 * (1;31m bold red)
 */

#pragma once

#include <stdbool.h>

#define ANSI_COLOR_RED "\x1b[1;31m"
#define ANSI_COLOR_SOFT_RED "\x1b[1;38;5;210m"
#define ANSI_COLOR_GREEN "\x1b[1;32m"
#define ANSI_COLOR_YELLOW "\x1b[1;33m"
#define ANSI_COLOR_BLUE "\x1b[1;34m"
#define ANSI_COLOR_MAGENTA "\x1b[1;35m"
#define ANSI_COLOR_CYAN "\x1b[1;36m"
#define ANSI_COLOR_RESET "\x1b[1;0m"

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief allows short-handed references to ANSI color schemes, and enables easier
 * color selection anytime you want to extend the available colors with an additional
 * enum, add a switch case in get_ansi_color_scheme
 */
typedef enum {
    COLORS_RED,
    COLORS_SOFT_RED,
    COLORS_GREEN,
    COLORS_YELLOW,
    COLORS_BLUE,
    COLORS_MAGENTA,
    COLORS_CYAN,
    COLORS_RESET
} COLORS;

/*! @brief prefixes a message with the given ANSI color code
 * @return Success: char pointer containing the message prefixed with the ANSI color
 * code
 * @return Failure: NULL pointer
 * @note you must free up the allocate memory for the returned vlaue
 */
char *format_colored(COLORS color, char *message);

/*! @brief returns an ansi color string to be used with printf
 */
char *get_ansi_color_scheme(COLORS color);

/*! @brief prints message to stdout with the given color
 */
void print_colored(COLORS color, char *message);

/*! @brief is like print_colored except it writes the data into the given file
 * descriptor
 * @return Success: 0
 * @return Failure: 1
 */
int write_colored(COLORS color, int file_descriptor, char *message);

#ifdef __cplusplus
}
#endif
