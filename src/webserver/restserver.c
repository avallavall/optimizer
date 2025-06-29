#include "mongoose.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "webserver/restserver.h"

// Event handler function
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    (void)c;  // Unused parameter
    
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        
        // Handle GET requests to root endpoint
        if (mg_strcmp(hm->method, mg_str("GET")) == 0 && mg_strcmp(hm->uri, mg_str("/")) == 0) {
            #ifdef DEBUG
                printf("[DEBUG] Received GET request\n");
            #endif
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Hello from Mongoose!\n");
        }
    }
}

int start_webserver(void) {
    struct mg_mgr mgr;  // Event manager
    struct mg_connection *c;

    char url[64];
    snprintf(url, sizeof(url), BASE_URL ":%d", PORT);

    mg_mgr_init(&mgr);  // Initialize
    
    // Start HTTP listener
    if ((c = mg_http_listen(&mgr, url, fn, NULL)) == NULL) {
        fprintf(stderr, "Failed to start server on port %d\n", PORT);
        return 1;
    }

    printf("Server started on port %d\n", PORT);
    
    // Infinite event loop
    while (true) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}