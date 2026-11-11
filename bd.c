#include "monitor.h"

static Episode database[] = {
    {"S01E01", "The Hedge Knight", 9.2, "45,120"},
    {"S01E02", "Hard Salt Beef", 8.8, "38,900"},
    {"S01E03", "The Squire", 9.5, "52,400"},
    {"S01E04", "Seven", 9.1, "41,200"},
    {"S01E05", "In the Name of the Mother", 9.7, "68,300"},
    {"S01E06", "The Morrow", 9.4, "49,800"}
};

static int db_initialized = 0;
static size_t db_size = sizeof(database) / sizeof(database[0]);

int init_database(void) {
    db_initialized = 1;
    return 0;
}

Episode* find_episode_by_code(const char *code) {
    if (!db_initialized) {
        init_database();
    }
    
    for (size_t i = 0; i < db_size; i++) {
        if (strncmp(database[i].code, code, EPISODE_CODE_LEN) == 0) {
            return &database[i];
        }
    }
    return NULL;
}
