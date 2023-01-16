#ifndef DPP_ALGO_CMD_HEADER_H
#define  DPP_ALGO_CMD_HEADER_H
//file name DppAlgoCmdHeader.h

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#include "aligned_malloc.h"


// Make sure no padding to struct members.
#pragma pack(push, 1)

#define DPP_VERSION_MAJOR      (0U)
#define DPP_VERSION_MINOR      (1U)

#define DPP_DECIMATION_ALGO_ID (10U)
#define DPP_TEMPORAL_ALGO_ID   (20U)
#define DPP_NEW_ALGO_ID        (30U)

// ICD = inter-connection definition.
struct dpp_icd_version {
    uint8_t major;  //BF_BF_20221120_MAJOR_MINOR_NOT_ZERO, Major and minor cannot be both 0, otherwise the command is invalid.
    uint8_t minor;
    uint8_t reserved[2];
};

struct dpp_common {
    struct dpp_icd_version ver;
    uint32_t auxiliary_heap_size;
    uint32_t debug_out_size;
    uint16_t cycle_counter;
    uint16_t payload_size;  // Only the size of all algo payload.
    uint8_t byte_sum;       // The sum of all algo command payload;
    uint8_t reserved[3];
};

struct dpp_cmd_header {
    uint8_t cmd_size;
    uint8_t cmd_id;
    uint8_t reserved[2];
};

struct dpp_cmd_decimation {
    struct dpp_cmd_header header;
    uint16_t frame_width;
    uint16_t frame_height;
    uint8_t scale_factor;
    uint8_t reserved[3];
};

struct dpp_cmd_temporal {
    struct dpp_cmd_header header;
    uint16_t frame_width;
    uint16_t frame_height;
    float alpha;
    uint16_t delta;       //BF_20221120_DELTA_ALPHA_SWAP
    uint8_t reserved[2];
};

struct dpp_new_algo {
    struct dpp_cmd_header header;
    uint16_t frame_width;
    uint16_t frame_height;
    uint8_t reserved[4];
};

// Restore to default padding.
#pragma pack(pop)

#endif