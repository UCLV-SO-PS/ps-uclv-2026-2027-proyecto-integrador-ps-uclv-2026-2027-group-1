#ifndef MONITOR_H
#define MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPISODE_CODE_LEN 6
#define MAX_TITLE_LEN 64

typedef struct {
    char code[EPISODE_CODE_LEN + 1];
    char title[MAX_TITLE_LEN];
    double rating;
    char votes[32];
} Episode;

int init_database(void);
Episode* find_episode_by_code(const char *code);
char* format_episode_response(const Episode *ep, char *buffer, size_t buffer_size);

#endif
