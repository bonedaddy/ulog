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

#include "colors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief returns an ansi color string to be used with printf
 */
char *get_ansi_color_scheme(COLORS color) {

    switch (color) {
        case COLORS_RED:
            return ANSI_COLOR_RED;
        case COLORS_SOFT_RED:
            return ANSI_COLOR_SOFT_RED;
        case COLORS_GREEN:
            return ANSI_COLOR_GREEN;
        case COLORS_YELLOW:
            return ANSI_COLOR_YELLOW;
        case COLORS_BLUE:
            return ANSI_COLOR_BLUE;
        case COLORS_MAGENTA:
            return ANSI_COLOR_MAGENTA;
        case COLORS_CYAN:
            return ANSI_COLOR_CYAN;
        case COLORS_RESET:
            return ANSI_COLOR_RESET;
        default:
            return NULL;
    }
}

/*! @brief prefixes a message with the given ANSI color code
 * @return Success: char pointer containing the message prefixed with the ANSI color
 * code
 * @return Failure: NULL pointer
 * @note you must free up the allocate memory for the returned vlaue
 */
char *format_colored(COLORS color, char *message) {

    char *pcolor = get_ansi_color_scheme(color);
    if (pcolor == NULL) {
        return NULL;
    }

    char *formatted = malloc(sizeof(message) + sizeof(pcolor));
    if (formatted == NULL) {
        printf("failed to format colored string\n");
        return NULL;
    }

    strcat(formatted, pcolor);
    strcat(formatted, message);

    return formatted;
}

/*! @brief prints message to stdout with the given color
 */
void print_colored(COLORS color, char *message) {
    printf("%s%s%s\n", get_ansi_color_scheme(color), message, ANSI_COLOR_RESET);
}

/*! @brief is like print_colored except it writes the data into the given file
 * descriptor
 * @return Success: 0
 * @return Failure: 1
 */
int write_colored(COLORS color, int file_descriptor, char *message) {

    char *pcolor = get_ansi_color_scheme(color);
    if (pcolor == NULL) {
        return -1;
    }

    char *reset = get_ansi_color_scheme(COLORS_RESET);
    if (reset == NULL) {
        return -1;
    }

    size_t write_msg_size =
        strlen(pcolor) + strlen(reset) + strlen(message) + 2; // 2 for \n
    char write_message[write_msg_size];
    memset(write_message, 0, write_msg_size);

    strcat(write_message, pcolor);
    strcat(write_message, message);
    strcat(write_message, reset);
    strcat(write_message, "\n");

    int response = write(file_descriptor, write_message, strlen(write_message));
    if (response == -1) {
        printf("failed to write colored message\n");
        return response;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
