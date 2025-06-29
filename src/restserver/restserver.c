#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "problem_manager/problem_manager.h"
#include "../lib/cJSON/cJSON.h"

#define BASE_URL "http://localhost"
#define PORT 8421
#define MAX_PROBLEM_TYPE_LEN 64

// Send a JSON response with CORS headers
static void send_json_response(struct mg_connection *c, int status, 
                             const char *message, const char *problem_type) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", status == 200 ? "success" : "error");
    cJSON_AddStringToObject(response, "message", message);
    if (problem_type && problem_type[0]) {
        cJSON_AddStringToObject(response, "problem", problem_type);
    }
    
    char *response_str = cJSON_PrintUnformatted(response);
    const char *headers = 
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type";
    
    mg_http_reply(c, status, headers, "%s", response_str);
    cJSON_free(response_str);
    cJSON_Delete(response);
}

// Map problem type string to enum
static problem_manager_type_t get_problem_type(const char *type_str) {
    if (!strcmp(type_str, "sudoku")) return TYPE_SUDOKU;
    if (!strcmp(type_str, "fertilizer")) return TYPE_FERTILIZER_MIXING;
    return TYPE_INVALID;
}

// Helper function to clean up resources
static void cleanup_resources(cJSON **root, char **body) {
    if (root && *root) {
        cJSON_Delete(*root);
        *root = NULL;
    }
    if (body && *body) {
        free(*body);
        *body = NULL;
    }
}

// Helper function to handle errors and send error response
static bool handle_error(struct mg_connection *c, cJSON **root, char **body, 
                        int status, const char *message) {
    cleanup_resources(root, body);
    send_json_response(c, status, message, NULL);
    return false;
}

// Handle POST /api/solve
// Returns true if the request was handled successfully, false otherwise
static bool handle_solve_post(struct mg_connection *c, struct mg_http_message *hm) {
    char *body = NULL;
    cJSON *root = NULL;
    
    // Copy and null-terminate the request body
    body = malloc(hm->body.len + 1);
    if (!body) {
        return handle_error(c, NULL, NULL, 500, "Failed to allocate memory");
    }
    
    memcpy(body, hm->body.buf, hm->body.len);
    body[hm->body.len] = '\0';
    printf("Request body: %s\n", body);
    
    // Parse JSON
    root = cJSON_ParseWithLength(body, hm->body.len);
    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        printf("JSON parse error: %s\n", error_ptr ? error_ptr : "Unknown error");
        return handle_error(c, &root, &body, 400, "Invalid JSON format");
    }
    
    // Get problem type
    cJSON *type_json = cJSON_GetObjectItemCaseSensitive(root, "problem");
    if (!type_json || !cJSON_IsString(type_json)) {
        printf("Missing or invalid 'problem' field\n");
        return handle_error(c, &root, &body, 400, "Missing or invalid 'problem' field");
    }
    
    // Validate problem type
    const char *type_str = type_json->valuestring;
    if (!type_str) {
        printf("Problem type is NULL\n");
        return handle_error(c, &root, &body, 400, "Problem type is missing");
    }
    
    // Make a copy of the type string for safety
    char type_str_copy[64] = {0};
    strncpy(type_str_copy, type_str, sizeof(type_str_copy) - 1);
    
    problem_manager_type_t type_enum = get_problem_type(type_str_copy);
    if (type_enum == TYPE_INVALID) {
        printf("Unsupported problem type: %s\n", type_str_copy);
        return handle_error(c, &root, &body, 400, "Unsupported problem type");
    }
    
    // Get problem data
    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
    if (!data) {
        printf("Missing 'data' field\n");
        return handle_error(c, &root, &body, 400, "Missing 'data' field");
    }
    
    if (cJSON_IsString(data)) {
        printf("Problem data: %s\n", data->valuestring);
    }
    
    // Clean up JSON as we don't need it anymore
    cJSON_Delete(root);
    root = NULL;
    
    // Clean up resources before processing
    cleanup_resources(&root, &body);
    
    // Process the solver
    printf("Dispatching solver for problem type: %s (enum: %d)\n", type_str_copy, type_enum);
    solver_result_t solver_result = problem_manager_dispatch_solver(type_enum);
    
    // Parse the solver result
    cJSON *result_json = cJSON_Parse(solver_result.result);
    if (!result_json) {
        send_json_response(c, 500, "Failed to parse solver result", type_str_copy);
        problem_manager_free_result(&solver_result);
        return false;
    }
    
    // Create response
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", solver_result.status == 0 ? "success" : "error");
    
    // Copy over all fields from the solver result
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, result_json) {
        cJSON_AddItemToObject(response, item->string, cJSON_Duplicate(item, 1));
    }
    
    // Add problem type to response
    cJSON_AddStringToObject(response, "problem", type_str_copy);
    
    // Send the response
    char *response_str = cJSON_PrintUnformatted(response);
    const char *headers = 
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type";
    
    mg_http_reply(c, solver_result.status == 0 ? 200 : 500, headers, "%s", response_str);
    
    // Clean up
    cJSON_Delete(response);
    cJSON_Delete(result_json);
    free(response_str);
    problem_manager_free_result(&solver_result);
    
    return solver_result.status == 0;
}

// Event handler function
static void event_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        
        // Handle GET requests to root endpoint
        if (mg_strcmp(hm->method, mg_str("GET")) == 0 && mg_strcmp(hm->uri, mg_str("/")) == 0) {
            printf("Received GET request on path %.*s\n", (int)hm->uri.len, hm->uri.buf);
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Hello from Mongoose!\n");
        }
        // Handle POST requests to /api/solve
        else if (mg_strcmp(hm->method, mg_str("POST")) == 0 && mg_strcmp(hm->uri, mg_str("/api/solve")) == 0) {
            printf("Received POST request to /api/solve\n");
            handle_solve_post(c, hm);
        }
        // Handle preflight OPTIONS request for CORS
        else if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {
            mg_http_reply(c, 200, 
                         "Access-Control-Allow-Origin: *\r\n"
                         "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                         "Access-Control-Allow-Headers: Content-Type\r\n"
                         "Content-Length: 0\r\n"
                         "\r\n", "");
        }
        // Handle 404 for all other routes
        else {
            fprintf(stderr, "Error: No handler for %.*s %.*s with body %.*s\n", 
                    (int)hm->method.len, hm->method.buf,
                    (int)hm->uri.len, hm->uri.buf,
                    (int)hm->body.len, hm->body.buf);
            mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "Not Found\n");
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
    
    char url[8192];
    snprintf(url, sizeof(url), BASE_URL ":%d", PORT);

    mg_mgr_init(&mgr);  // Initialize
    
    // Start HTTP listener
    if ((c = mg_http_listen(&mgr, url, event_handler, NULL)) == NULL) {
        fprintf(stderr, "Failed to start server on port %d\n", PORT);
        return 1;
    }

    printf("--------------------------------\n");
    printf("Server started on port %d\n", PORT);
    printf("Press 'h' for help\n");
    printf("--------------------------------\n");
    
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