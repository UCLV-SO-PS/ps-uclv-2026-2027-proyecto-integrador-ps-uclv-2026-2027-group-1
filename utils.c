#include "monitor.h"

char* format_episode_response(const Episode *ep, char *buffer, size_t buffer_size) {
    if (ep == NULL || buffer == NULL || buffer_size == 0) {
        return NULL;
    }
    
    snprintf(buffer, buffer_size,
             "Código: %s\n"
             "Episodio/Título: %s\n"
             "Puntuación IMDB: %.1f\n"
             "Votos Registrados: %s\n",
             ep->code, ep->title, ep->rating, ep->votes);
    
    return buffer;
}
