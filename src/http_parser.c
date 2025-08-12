#include "http_parser.h"
#include <stdio.h>
#include <string.h>

void parse_http_request(const char *request) {
    char method[8], path[256];
    // Simple parsing: expects "METHOD PATH HTTP/1.1"
    if (sscanf(request, "%7s %255s", method, path) == 2) {
        printf("HTTP Method: %s\n", method);
        printf("HTTP Path: %s\n", path);
    } else {
        printf("Failed to parse HTTP request line.\n");
    }
}