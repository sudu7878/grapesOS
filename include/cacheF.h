//include/cacheF.h

#pragma once

#include "common.h"

#ifndef CACHE_H
#define CACHE_H

#define CACHE_LINE_SIZE 64

void clean_data_cache_for_address(void *addr, u32 size);

#endif