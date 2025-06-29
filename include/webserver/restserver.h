#ifndef RESTSERVER_H
#define RESTSERVER_H

#include "mongoose.h"

#define BASE_URL "http://localhost"
#define PORT 8421
#define API_PATH "/api/solve"

int start_webserver(void);

#endif