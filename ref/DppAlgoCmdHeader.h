
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// Make sure no padding to struct members.
#pragma pack(push, 1)

#define DPP_VERSION_MAJOR (0U)
#define DPP_VERSION_MINOR (1U)

#define DPP_DECIMATION_ALGO_ID (10U)
#define DPP_TEMPORAL_ALGO_ID (20U)
#define DPP_NEW_ALGO_ID (30U)

// ICD = inter-connection definition.
struct dpp_icd_version {
    uint8_t major;
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
    uint16_t delta;
    float alpha;
    uint8_t reserved[2];
};

struct dpp_new_algo {
    struct dpp_cmd_header header;
    uint16_t frame_width;
    uint16_t frame_height;
    uint8_t reserved[4];
};

///
//  sample code to construct the payload.
// 
void sample_code_on_cpu() {
    size_t payload_size =
        sizeof(dpp_common) + sizeof(dpp_cmd_decimation) + sizeof(dpp_cmd_temporal);
    void* payload_buffer = malloc(payload_size);
    dpp_common* common = (dpp_common*)payload_buffer;
    common->ver.major = DPP_VERSION_MAJOR;
    common->ver.minor = DPP_VERSION_MINOR;
    common->payload_size = sizeof(dpp_cmd_decimation) + sizeof(dpp_cmd_temporal);
    common->cycle_counter = 0U;
    // ...

    size_t offset = sizeof(dpp_common);
    dpp_cmd_decimation* decimation = (dpp_cmd_decimation*)((uint8_t*)payload_buffer + offset);
    decimation->header.cmd_id = DPP_DECIMATION_ALGO_ID;
    decimation->header.cmd_size = sizeof(dpp_cmd_decimation);
    decimation->frame_width = 960U;
    decimation->frame_height = 720U;
    decimation->scale_factor = 2U;

    offset += decimation->header.cmd_size;
    dpp_cmd_temporal* temporal = (dpp_cmd_temporal*)((uint8_t*)payload_buffer + offset);
    temporal->header.cmd_id = DPP_TEMPORAL_ALGO_ID;
    temporal->header.cmd_size = sizeof(dpp_cmd_temporal);
    temporal->frame_width = 480U;
    temporal->frame_height = 360U;
    temporal->alpha = 0.4F;
    temporal->delta = 20U;
}

///
//  sample code to parse the payload on DSP.
// 
void sample_code_on_dsp(void* payload_buffer) {
    dpp_common* common = (dpp_common*)payload_buffer;
    dpp_cmd_header* header = NULL;
    dpp_cmd_decimation* decimation = NULL;
    dpp_cmd_temporal* temporal = NULL;
    dpp_new_algo* new_algo = NULL;

    size_t offset = sizeof(dpp_common);
    while (offset < common->payload_size) {
        header = (dpp_cmd_header*)((uint8_t*)payload_buffer + offset);
        switch (header->cmd_id) {
        case DPP_DECIMATION_ALGO_ID:
            decimation = (dpp_cmd_decimation*)((uint8_t*)payload_buffer + offset);
            break;

        case DPP_TEMPORAL_ALGO_ID:
            temporal = (dpp_cmd_temporal*)((uint8_t*)payload_buffer + offset);
            break;

        case DPP_NEW_ALGO_ID:
            new_algo = (dpp_new_algo*)((uint8_t*)payload_buffer + offset);
            break;

        default:
            // Not a valid algo.
            break;
        }
        offset += header->cmd_size;
    }
    // 0xDEAD is always at the end of all algo command payloads
    assert(*(uint32_t*)((uint8_t*)payload_buffer + offset) == 0xDEAD);
}

// Restore to default padding.
#pragma pack(pop)

/**
 * @brief Generic macro to trap size alignment violations at compile time
 *
 * A violation will result in a compilation error.
 * @param struct_type The structure type to check
 * @param alignment Byte alignment to enforce
 
#define CHECK_SIZE_ALIGNMENT(struct_type, alignment)                     \
    do {                                                                 \
        uint8_t arr[(sizeof(struct_type) % (alignment) == 0U) ? 1 : -1]; \
        (void)arr;                                                       \
    } while (false)

/**
 * @brief Macro to trap 32 bit size alignment violations at compile time

#define CHECK_ALIGN32(struct_type) CHECK_SIZE_ALIGNMENT(struct_type, sizeof(uint32_t))

/**
 * Compile time checks only - this function is not to be called!

static inline void dpp_msg_body_size_check(void) {
    CHECK_ALIGN32(struct dpp_common);
    CHECK_ALIGN32(struct dpp_cmd_decimation);
    CHECK_ALIGN32(struct dpp_cmd_temporal);
}

