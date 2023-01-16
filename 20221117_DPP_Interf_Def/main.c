#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "aligned_malloc.h"

#define DPP_VERSION_MAJOR            (0U)
#define DPP_VERSION_MINOR            (1U)
#define DPP_DECIMATION_ALGO_ID       (10U)
#define DPP_TEMPORAL_ALGO_ID         (20U)
#define DPP_NEW_ALGO_ID              (30U)
#define PAYLOAD_END_SIGNATURE        0xDEAD
#define PAYLOAD_END_SIGNATURE_SZ     (2U)

/*
|----------------------------------------------------------------------------------------------------|
|      common        |   dpp_cmd_header , dpp_cmd_decimation    |     dpp_cmd_header , algo | 0xDEAD |
|   dpp_icd_version  |   cmd_size       ,                       |     cmd_size       ,		|		 |
|                    |   id             ,                       |     id             ,		|		 |
|----------------------------------------------------------------------------------------------------|
*/


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

/// <summary>
/// simple cs
/// 
/// </summary>
/// <param name="pld_p"> pointer to payload </param>
/// <param name="sz"> size of payload </param>
/// <returns> the check sum</returns>
uint8_t cs(uint8_t* pld_p, size_t sz) {
	uint8_t cs = 0, i = 0;
	uint8_t* p_u8 = (pld_p);
	for (i = 0; i < sz; i++) {
		cs += *p_u8;
		p_u8++;
	}
	return cs;
}

/// <summary>
/// sample code to construct the payload from ARM side.
/// </summary>
/// <returns></returns>
void* sample_code_on_cpu() {
	uint8_t* u8_p_begin_of_payload = NULL;
	size_t mem_size = sizeof(struct dpp_common) +
		sizeof(struct dpp_cmd_decimation) + sizeof(struct dpp_cmd_temporal) +
		sizeof(struct dpp_new_algo) + PAYLOAD_END_SIGNATURE_SZ;

	void* payload_buffer = aligned_malloc(mem_size, sizeof(uint32_t));
	struct dpp_common* common = (struct dpp_common*)payload_buffer;
	common->ver.major = DPP_VERSION_MAJOR;
	common->ver.minor = DPP_VERSION_MINOR;
	common->payload_size = sizeof(struct dpp_cmd_decimation) + sizeof(struct dpp_cmd_temporal) + sizeof(struct dpp_new_algo);
	common->cycle_counter = 0U;

	size_t offset = sizeof(struct dpp_common);
	u8_p_begin_of_payload = (uint8_t*)payload_buffer + offset;
	struct dpp_cmd_decimation* decimation = (struct dpp_cmd_decimation*)(u8_p_begin_of_payload);

	decimation->header.cmd_id = DPP_DECIMATION_ALGO_ID;
	decimation->header.cmd_size = sizeof(struct dpp_cmd_decimation);
	decimation->frame_width = 960U;
	decimation->frame_height = 720U;
	decimation->scale_factor = 2U;

	offset += decimation->header.cmd_size;
	struct dpp_cmd_temporal* temporal = (struct dpp_cmd_temporal*)((uint8_t*)payload_buffer + offset);
	temporal->header.cmd_id = DPP_TEMPORAL_ALGO_ID;
	temporal->header.cmd_size = sizeof(struct dpp_cmd_temporal);
	temporal->frame_width = 480U;
	temporal->frame_height = 360U;
	temporal->alpha = 0.4F;
	temporal->delta = 23U;

	offset += temporal->header.cmd_size;
	struct dpp_new_algo* new_algo = (struct dpp_new_algo*)((uint8_t*)payload_buffer + offset);
	new_algo->header.cmd_id = DPP_NEW_ALGO_ID;
	new_algo->header.cmd_size = sizeof(struct dpp_new_algo);
	new_algo->frame_width = 74U;
	new_algo->frame_height = 46U;

	//calculate the cs
	common->byte_sum = cs(u8_p_begin_of_payload, common->payload_size);
	printf("\nPAYLOAD_CS: 0x%02X\n", common->byte_sum);

	offset += new_algo->header.cmd_size;
	*(((uint16_t*)((uint8_t*)payload_buffer + offset))) = PAYLOAD_END_SIGNATURE;

	return payload_buffer;
}

/// <summary>
/// Sample code to parse the payload on DSP.
/// </summary>
/// <param name="payload_buffer"></param>
void sample_code_on_dsp(void* payload_buffer) {
	uint8_t* u8_p_begin_of_payload = NULL;
	struct dpp_common* common = (struct dpp_common*)payload_buffer;
	struct dpp_cmd_header* header = NULL;
	struct dpp_cmd_decimation* decimation = NULL;
	struct dpp_cmd_temporal* temporal = NULL;
	struct dpp_new_algo* new_algo = NULL;

	size_t offset = sizeof(struct dpp_common);
	size_t begin = offset;
	u8_p_begin_of_payload = (uint8_t*)payload_buffer + offset;
	while (offset < common->payload_size + begin) {
		header = (struct dpp_cmd_header*)((uint8_t*)payload_buffer + offset);
		printf("offset: %zu \n", offset);
		switch (header->cmd_id) {
		case DPP_DECIMATION_ALGO_ID:
			decimation = (struct dpp_cmd_decimation*)((uint8_t*)payload_buffer + offset);
			printf("\n");
			printf("[DECIMATION] \n");
			printf("frame width  : %u \n", decimation->frame_width);
			printf("frame height : %u \n", decimation->frame_height);
			printf("scale factor : %u \n", decimation->scale_factor);
			break;

		case DPP_TEMPORAL_ALGO_ID:
			temporal = (struct dpp_cmd_temporal*)((uint8_t*)payload_buffer + offset);
			printf("\n");
			printf("[TEMPORAL] \n");
			printf("frame width  : %u \n", temporal->frame_width);
			printf("frame height : %u \n", temporal->frame_height);
			printf("delta        : %u \n", temporal->delta);
			printf("alpha        : %f \n", temporal->alpha);
			break;

		case DPP_NEW_ALGO_ID:
			new_algo = (struct dpp_new_algo*)((uint8_t*)payload_buffer + offset);
			printf("\n");
			printf("[DPP_NEW_ALGO_ID] \n");
			printf("frame width  : %u \n", new_algo->frame_width);
			printf("frame height : %u \n", new_algo->frame_height);
			break;

		default:

			// Not a valid algo.
			//log ...
			//error codes ...
			//what is the code is not valid.
			printf("Not a valid algo. \n");
			break;
		}
		offset += header->cmd_size;
		printf("offset: %zu \n", offset);
		printf("common->payload_size: %d \n", common->payload_size);
	}
	// 0xDEAD is always at the end of all algo command payloads
	assert(0xDEAD == *(uint16_t*)((uint8_t*)payload_buffer + offset));
	printf("PAYLOAD_END_SIGNATURE: 0x%04X\n", PAYLOAD_END_SIGNATURE);

	//calculate the cs
	assert(common->byte_sum == cs(u8_p_begin_of_payload, common->payload_size));
	printf("\nPAYLOAD_CS: 0x%02X\n", common->byte_sum);
}


//
//@brief Generic macro to trap size alignment violations at compile time
//
// A violation will result in a compilation error.
//@param struct_type The structure type to check
//@param alignment Byte alignment to enforce
//
#define CHECK_SIZE_ALIGNMENT(struct_type, alignment)                     \
    do {                                                                 \
        uint8_t arr[(sizeof(struct_type) % (alignment) == 0U) ? 1 : -1]; \
        (void)arr;                                                       \
    } while (false)

 //
 //@brief Macro to trap 32 bit size alignment violations at compile time
 //
#define CHECK_ALIGN32(struct_type) CHECK_SIZE_ALIGNMENT(struct_type, sizeof(uint32_t))

//
//Compile time checks only - this function is not to be called!
//
static inline void dpp_msg_body_size_check(void) {
	CHECK_ALIGN32(struct dpp_common);
	CHECK_ALIGN32(struct dpp_cmd_decimation);
	CHECK_ALIGN32(struct dpp_cmd_temporal);
}

int main(int argc, char* argv[]) {
	printf("dpp_icd_version %zu \n", sizeof(struct dpp_icd_version));
	printf("dpp_common %zu \n", sizeof(struct dpp_common));
	printf("dpp_cmd_decimation %zu \n", sizeof(struct dpp_cmd_decimation));
	printf("dpp_cmd_temporal %zu \n", sizeof(struct dpp_cmd_temporal));
	printf("dpp_new_algo %zu \n", sizeof(struct dpp_new_algo));

	void* payload_buffer = sample_code_on_cpu();

	if (NULL != payload_buffer) {
		sample_code_on_dsp(payload_buffer);
		aligned_free(payload_buffer);
	}
	else {
		printf("payload_buffer is NULL\n");
	}

	return 0;
}
