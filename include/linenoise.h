/* linenoise.h -- guerrilla line editing library against the idea that a
 * line editing lib needs to be 20,000 lines of C code.
 *
 * See linenoise.c for more information.
 *
 * Copyright (c) 2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LINENOISE_H
#define __LINENOISE_H

#define LINENOISE_VERSION "1.0.0"
#define LINENOISE_VERSION_MAJOR 1
#define LINENOISE_VERSION_MINOR 1

#include <uchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct linenoiseStatus {
    char32_t const* line;
    int pos;
    int len;
} linenoiseStatus;
typedef struct linenoiseCompletions linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char*, linenoiseStatus, linenoiseCompletions*);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback* fn);
void linenoiseAddCompletion(linenoiseCompletions* lc, const char* str);

// Special codes for keyboard input:
//
// Between Windows and the various Linux "terminal" programs, there is some
// pretty diverse behavior in the "scan codes" and escape sequences we are
// presented with.  So ... we'll translate them all into our own pidgin
// pseudocode, trying to stay out of the way of UTF-8 and international
// characters.  Here's the general plan.
//
// "User input keystrokes" (key chords, whatever) will be encoded as a single
// value.
// The low 21 bits are reserved for Unicode characters.  Popular function-type
// keys
// get their own codes in the range 0x10200000 to (if needed) 0x1FE00000,
// currently
// just arrow keys, Home, End and Delete.  Keypresses with Ctrl get ORed with
// 0x20000000, with Alt get ORed with 0x40000000.  So, Ctrl+Alt+Home is encoded
// as 0x20000000 + 0x40000000 + 0x10A00000 == 0x70A00000.  To keep things
// complicated,
// the Alt key is equivalent to prefixing the keystroke with ESC, so ESC
// followed by
// D is treated the same as Alt + D ... we'll just use Emacs terminology and
// call
// this "Meta".  So, we will encode both ESC followed by D and Alt held down
// while D
// is pressed the same, as Meta-D, encoded as 0x40000064.
//
// Here are the definitions of our component constants:
//
// Maximum unsigned 32-bit value    = 0xFFFFFFFF;   // For reference, max 32-bit
// value
// Highest allocated Unicode char   = 0x001FFFFF;   // For reference, max
// Unicode value
enum {
  LINENOISE_META = 0x40000000,  // Meta key combination
  LINENOISE_CTRL = 0x20000000,  // Ctrl key combination

  LINENOISE_SPECIAL_KEY = 0x10000000,   // Common bit for all special keys
  LINENOISE_UP_ARROW_KEY = 0x10200000,  // Special keys
  LINENOISE_DOWN_ARROW_KEY = 0x10400000,
  LINENOISE_RIGHT_ARROW_KEY = 0x10600000,
  LINENOISE_LEFT_ARROW_KEY = 0x10800000,
  LINENOISE_HOME_KEY = 0x10A00000,
  LINENOISE_END_KEY = 0x10C00000,
  LINENOISE_DELETE_KEY = 0x10E00000,
  LINENOISE_PAGE_UP_KEY = 0x11000000,
  LINENOISE_PAGE_DOWN_KEY = 0x11200000
};

typedef struct linenoiseKeyQueue linenoiseKeyQueue;
typedef int(linenoiseKeyCallback)(char32_t, linenoiseStatus, linenoiseKeyQueue*);
void linenoiseSetKeyCallback(linenoiseKeyCallback* fn);
void linenoiseQueueKey(linenoiseKeyQueue* queue, char32_t key);

char* linenoise(const char* prompt, int newline, int displayEmpty);
void linenoisePreloadBuffer(const char* preloadText);
int linenoiseHistoryAdd(const char* line);
int linenoiseHistorySetMaxLen(int len);
char* linenoiseHistoryLine(int index);
int linenoiseHistorySave(const char* filename);
int linenoiseHistoryLoad(const char* filename);
void linenoiseHistoryFree(void);
void linenoiseClearScreen(void);
void linenoiseSetMultiLine(int ml);
void linenoisePrintKeyCodes(void);
/* the following is extension to the original linenoise API */
int linenoiseInstallWindowChangeHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __LINENOISE_H */
