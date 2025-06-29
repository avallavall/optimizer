#include "mongoose.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "restserver/restserver.h"

#define BASE_URL "http://localhost"
#define PORT 8421
#define API_PATH "/api/solve"

// Event handler function
static void event_handler(struct mg_connection *c, int ev, void *ev_data) {
    (void)c;  // Unused parameter
    
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        
        // Handle GET requests to root endpoint
        if (mg_strcmp(hm->method, mg_str("GET")) == 0 && mg_strcmp(hm->uri, mg_str("/")) == 0) {
            printf("Received GET request on path %.*s\n", (int)hm->uri.len, hm->uri.buf);
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Hello from Mongoose!\n");
        }
    }
}

// Function to print help message
static void print_help(void) {
    printf("\n=== Optimizer Server Commands ===\n");
    printf("h   - Show this help message\n");
    printf("1   - Stop the server and exit\n");
    printf("\n");
}

int start_webserver(void) {
    struct mg_mgr mgr;  // Event manager
    struct mg_connection *c;
    int running = 1;
    char input;
    fd_set fds;
    
    char url[64];
    snprintf(url, sizeof(url), BASE_URL ":%d", PORT);

    mg_mgr_init(&mgr);  // Initialize
    
    // Start HTTP listener
    if ((c = mg_http_listen(&mgr, url, event_handler, NULL)) == NULL) {
        fprintf(stderr, "Failed to start server on port %d\n", PORT);
        return 1;
    }

    printf("Server started on port %d\n", PORT);
    printf("Press 'h' for help\n");
    
    // Main event loop
    while (running) {
        // Handle HTTP requests
        mg_mgr_poll(&mgr, 100);
        
        // Check for keyboard input
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        
        // Check for input without blocking
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &(struct timeval){0}) > 0) {
            if (read(STDIN_FILENO, &input, 1) > 0) {
                switch (input) {
                    case 'h':
                    case 'H':
                    case '?':
                        print_help();
                        break;
                    case '1':
                        printf("\nStopping server...\n");
                        running = 0;
                        break;
                    case '\n':  // Ignore newline
                        break;
                    default:
                        printf("\nUnknown command. Press 'h' for help.\n");
                        break;
                }
            }
        }
    }

    mg_mgr_free(&mgr);
    return 0;
}