/*
 * The License to Kill Project
 *
 * Copyright (C) 1995-2022 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include "logger.h"

void log_format(const char *tag, const char *message, va_list args)
{
    time_t now;
    time(&now);
    char *date = ctime(&now);
    date[strlen(date) - 1] = '\0';
    printf("%s [%s] ", date, tag);
    vprintf(message, args);
    printf("\n");
}

void log_error(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    printf(RED);
    log_format("Error", message, args);
    printf(RESET "");
    va_end(args);
}

void log_info(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    log_format("Info", message, args);
    va_end(args);
}

void log_debug(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    printf(CYN);
    log_format("Debug", message, args);
    printf(RESET "");
    va_end(args);
}
