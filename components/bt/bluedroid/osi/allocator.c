/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "bt_defs.h"
#include "allocator.h"

extern void *pvPortZalloc(size_t size);
extern void vPortFree(void *pv);

#ifdef CONFIG_BLUEDROID_MEM_DEBUG

#define OSI_MEM_DBG_INFO_MAX    1024
typedef struct {
    void *p;
    int size;
    const char *func;
    int line;
} osi_mem_dbg_info_t;

static uint32_t mem_dbg_count = 0;
static uint32_t mem_dbg_count2 = 0;
static osi_mem_dbg_info_t mem_dbg_info[OSI_MEM_DBG_INFO_MAX];

void osi_mem_dbg_init(void)
{
    int i;

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        mem_dbg_info[i].p = NULL;
        mem_dbg_info[i].size = 0;
        mem_dbg_info[i].func = NULL;
        mem_dbg_info[i].line = 0;
    }
    mem_dbg_count = 0;
    mem_dbg_count2 = 0;
}

void osi_mem_dbg_record(void *p, int size, const char *func, int line)
{
    int i;

    if (!p || size == 0) {
        LOG_ERROR("%s invalid !!\n", __func__);
        return;
    }

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        if (mem_dbg_info[i].p == NULL) {
            mem_dbg_info[i].p = p;
            mem_dbg_info[i].size = size;
            mem_dbg_info[i].func = func;
            mem_dbg_info[i].line = line;
            mem_dbg_count++;
            break;
        }
    }

    if (i >= OSI_MEM_DBG_INFO_MAX) {
        LOG_ERROR("%s full !!\n", __func__);
    }
}

void osi_mem_dbg_clean(void *p)
{
    int i;

    if (!p) {
        LOG_ERROR("%s invalid\n", __func__);
        return;
    }

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        if (mem_dbg_info[i].p == p) {
            mem_dbg_info[i].p = NULL;
            mem_dbg_info[i].size = 0;
            mem_dbg_info[i].func = NULL;
            mem_dbg_info[i].line = 0;
            mem_dbg_count--;
            break;
        }
    }

    if (i >= OSI_MEM_DBG_INFO_MAX) {
        LOG_ERROR("%s full !!\n", __func__);
    }
}

void osi_mem_dbg_show(void)
{
    int i;

    for (i = 0; i < OSI_MEM_DBG_INFO_MAX; i++) {
        if (mem_dbg_info[i].p || mem_dbg_info[i].size != 0 ) {
            LOG_ERROR("--> p %p, s %d, f %s, l %d\n", mem_dbg_info[i].p, mem_dbg_info[i].size, mem_dbg_info[i].func, mem_dbg_info[i].line);
        }
    }
    LOG_ERROR("--> count %d\n", mem_dbg_count);
}
#endif

char *osi_strdup(const char *str)
{
    size_t size = strlen(str) + 1;  // + 1 for the null terminator
    char *new_string = (char *)calloc(1, size);

    if (!new_string) {
        return NULL;
    }

    memcpy(new_string, str, size);
    return new_string;
}

void *osi_malloc_func(size_t size)
{
    return calloc(1, size);
}

void *osi_calloc_func(size_t size)
{
    return calloc(1, size);
}

void osi_free_func(void *ptr)
{
    free(ptr);
}

const allocator_t allocator_malloc = {
    osi_malloc_func,
    osi_free_func
};

const allocator_t allocator_calloc = {
    osi_calloc_func,
    osi_free_func
};
