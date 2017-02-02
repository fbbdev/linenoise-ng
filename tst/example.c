#include "linenoise.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char* examples[] = {
  "db", "hello", "hallo", "hans", "hansekogge", "seamann", "quetzalcoatl", "quit", "power", NULL
};

static void completionHook (char const* prefix, linenoiseStatus st, linenoiseCompletions* lc) {
  size_t i;

  ((void) st);

  for (i = 0;  examples[i] != NULL; ++i) {
    if (strncmp(prefix, examples[i], strlen(prefix)) == 0) {
      linenoiseAddCompletion(lc, examples[i]);
    }
  }
}

static int firstKey = 1;
static int isListOrString = 0;

static int keyHook(char32_t key, linenoiseStatus st, linenoiseKeyQueue* queue) {
  if (firstKey && key == '\b') {
    linenoiseQueueKey(queue, 'd');
    linenoiseQueueKey(queue, 'r');
    linenoiseQueueKey(queue, 'o');
    linenoiseQueueKey(queue, 'p');
    linenoiseQueueKey(queue, '\n');
    return -1;
  }

  firstKey = 0;

  switch (key) {
    case '\'':
      isListOrString = 1;
      if (st.line[st.pos] == '\'') {
        linenoiseQueueKey(queue, LINENOISE_RIGHT_ARROW_KEY);
        return -1;
      }

      linenoiseQueueKey(queue, '\'');
      linenoiseQueueKey(queue, LINENOISE_LEFT_ARROW_KEY);
      break;

    case '{':
      isListOrString = 1;
      linenoiseQueueKey(queue, '}');
      linenoiseQueueKey(queue, LINENOISE_LEFT_ARROW_KEY);
      break;

    case '}':
      isListOrString = 1;
      if (st.line[st.pos] == '}') {
        linenoiseQueueKey(queue, LINENOISE_RIGHT_ARROW_KEY);
        return -1;
      }

      break;

    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
    case '%':
    case L'²':
    case L'³':
      if (!isListOrString) {
        if (st.pos != st.len || st.line[st.pos-1] != ' ') {
          linenoiseQueueKey(queue, LINENOISE_END_KEY);
          linenoiseQueueKey(queue, ' ');
          linenoiseQueueKey(queue, key);
          return -1;
        }

        linenoiseQueueKey(queue, '\n');
      }
      break;
  }

  return 0;
}

int main (int argc, char** argv) {
  linenoiseInstallWindowChangeHandler();

  while(argc > 1) {
    argc--;
    argv++;
    if (!strcmp(*argv, "--keycodes")) {
      linenoisePrintKeyCodes();
      exit(0);
    }
  }

  const char* file = "./history";

  linenoiseHistoryLoad(file);
  linenoiseSetCompletionCallback(completionHook);
  linenoiseSetKeyCallback(keyHook);

  printf("starting...\n");

  char const* prompt = "\x1b[1;32mlinenoise\x1b[0m> ";

  while (1) {
    firstKey = 1;
    isListOrString = 0;
    char* result = linenoise(prompt, 1, 1);

    if (result == NULL) {
      break;
    } else if (!strncmp(result, "/history", 8)) {
      /* Display the current history. */
      for (int index = 0; ; ++index) {
        char* hist = linenoiseHistoryLine(index);
        if (hist == NULL) break;
        printf("%4d: %s\n", index, hist);
        free(hist);
       }
    }
    if (*result == '\0') {
      free(result);
      break;
    }

    printf("thanks for the input.\n");
    linenoiseHistoryAdd(result);
    free(result);
  }

  linenoiseHistorySave(file);
  linenoiseHistoryFree();
}
