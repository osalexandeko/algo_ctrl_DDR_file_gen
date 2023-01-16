///*
//Some points ...
// 
//BF_20221110_IS_NOT_ALIGNED_4   - It was not aligned 4, it is easy when one keeps small blocks already aligned,
// 								   for example: sizeof( header struct ) was 2, so it can easily brake the alignmet
//  								   without paying attention, when one adds it to struct dpp_decimation...
//  								   
//NF_20221110_PALOAD_OPTION      - An option for the payload, It can make things easier and more flexible.
//
//NF_20221110_SIMPLE_CS          - A simple checksum could be of great help.
//
//*/
//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include <string.h>
//
//// Make sure no padding to struct members.
//#pragma pack(push, 1)
//
//#define MAJOR 0
//#define MINOR 1
//
//#define DPP_PL_BFFR_SZ 4      //NF_20221110_PALOAD_OPTION the size of the payload. 
//
//#define DECIMATION_ID  10
//#define TEMPORAL_ID    11
//#define NEW_ALGO_ID    254
//
//
//
//struct dpp_common {
//    uint16_t version;
//    uint16_t payload_size;
//    uint16_t cycle_counter;
//    uint32_t auxiliary_heap_size;
//    uint32_t debug_out_size;
//    //uint8_t reserved[1];      //BF_20221110_IS_NOT_ALIGNED_4, I saw it in previous meeting, but than we moved to dpp_decimation, before 
//    							// adding the header to it :-) .
//    uint8_t reserved[2]; 
//};
//
//struct cmd_header {
//    uint8_t cmd_size;
//    uint8_t cmd_id;
//    uint8_t reserved[2];        //BF_20221110_IS_NOT_ALIGNED_4
//};
//
//struct dpp_decimation {
//    struct cmd_header header;
//    uint16_t frame_width;
//    uint16_t frame_height;
//    uint8_t scale_factor;
//    //uint8_t reserved[1];      //BF_20221110_IS_NOT_ALIGNED_4
//    uint8_t reserved[3];
//};
//
//struct dpp_temporal {
//    struct cmd_header header;
//    uint16_t frame_width;
//    uint16_t frame_height;
//    uint16_t delta;
//    float alpha;
//    //uint8_t reserved[8];         //BF_20221110_IS_NOT_ALIGNED_4
//    uint8_t reserved[10];  
//};
//
//struct dpp_new_algo {
//    struct cmd_header header;
//    uint16_t frame_width;
//    uint16_t frame_height;
//    uint16_t delta;
//    float alpha;
//    //uint8_t reserved[8];         //BF_20221110_IS_NOT_ALIGNED_4
//    uint8_t reserved[10];
//};
//
//struct dpp_payload {
//    struct dpp_common common;
//    struct dpp_decimation decimation;
//    struct dpp_temporal temporal;
//    struct dpp_new_algo new_algo;
//};
//
//
////NF_20221110_PALOAD_OPTION 
////=============================================================================
//typedef union dpp_algo_un {
//    struct dpp_common common;
//    struct dpp_decimation decimation;
//    struct dpp_temporal temporal;
//    struct dpp_new_algo new_algo;
//} dpp_algo_un_t;
//
//typedef struct dpp_pld {
//	uint8_t  pld_sz;                             //the payload size can be 1,2,3... DPP_PL_BFFR_SZ
//	uint8_t  pld_cs;                             //the checksum of the payload.
//	uint8_t  reserved[2];  
// 	dpp_algo_un_t pld[DPP_PL_BFFR_SZ];           //the payload buffer
//} dpp_pld_t;
//
///*simple cs */
//bool cs(dpp_pld_t * pld_p){
//	
//	uint8_t cs = 0;
// 	size_t i = 0, sz =  pld_p->pld_sz * sizeof(dpp_algo_un_t);
// 	uint8_t * p_u8 = (uint8_t * ) (&pld_p->pld[0]);
//	for(i = 0; i < sz; i++){
//		cs +=  *p_u8;
//		p_u8++;
//	} 
//	printf("cs: %d, pld_cs: %d  \n",cs,pld_p->pld_cs);
//	return cs == pld_p->pld_cs;
//}
//
///*simple cs calculation */
//void clc_cs(size_t algo_nm, dpp_pld_t * pld_p){
//	size_t i = 0,sz = algo_nm * sizeof(dpp_algo_un_t);
//	uint8_t * p_u8 = (uint8_t * ) &(pld_p->pld[0]);
//	for(i = 0; i < sz; i++){
//		pld_p->pld_cs += *p_u8;
//		p_u8++;
//	} 
//}
//	
//
//
////just for demo
//void simple_dpp_pld_t_parser(dpp_pld_t * pld_p){
//	size_t  i          = 0;
//	size_t  algo_nmbr  =  pld_p->pld_sz;
//	uint8_t id         = 0xFF;
//	
//	
//	for(i = 0; i < algo_nmbr ; i++ ){
//		id =  pld_p->pld[i].decimation.header.cmd_id;
//		switch(id){
//			case DECIMATION_ID:
//				printf("\n");
//				printf("[DECIMATION] \n");
//				printf("frame width  : %d \n", pld_p->pld[i].decimation.frame_width);
//				printf("frame height : %d \n", pld_p->pld[i].decimation.frame_height);
//				printf("scale factor : %d \n", pld_p->pld[i].decimation.scale_factor);
//				 
//			break;
//			
//	 
//			
//			case TEMPORAL_ID:
//				printf("\n");
//				printf("[TEMPORAL] \n");
//				printf("frame width  : %d \n", pld_p->pld[i].temporal.frame_width);
//				printf("frame height : %d \n", pld_p->pld[i].temporal.frame_height);
//				printf("delta        : %d \n", pld_p->pld[i].temporal.delta);
//				printf("alpha        : %f \n", pld_p->pld[i].temporal.alpha);
//				 
//			break;
//			
//			case NEW_ALGO_ID:
//				printf("\n");
//				printf("[NEW_ALGO] \n");
//				printf("frame width  : %d \n", pld_p->pld[i].new_algo.frame_width);
//				printf("frame height : %d \n", pld_p->pld[i].new_algo.frame_height);
//				printf("delta        : %d \n", pld_p->pld[i].new_algo.delta);
//				printf("alpha        : %f \n", pld_p->pld[i].new_algo.alpha);
//			 
//			break;
//				
//			default:
//				printf("the algo is unknown \n");
//				break;
//		}
//	}
//	
//	(cs(pld_p) == true)?(printf("\nCS is OK \n")):(	printf("\nCS NOT OK \n"));
//	
//
// 
//}
//
// 
//
//void make_a_simple_test_decimation(){
//	printf("\nSimple test decimation only:");
//	dpp_pld_t pld;
//	memset(&pld, 0, sizeof(dpp_pld_t));  //just for the test ... 
//	
//	pld.pld_sz =  1;  // for decimation only.
//	pld.pld[0].decimation.header.cmd_id   = DECIMATION_ID;
//	pld.pld[0].decimation.header.cmd_size = 12;
//	pld.pld[0].decimation.frame_width     = 960; 
//	pld.pld[0].decimation.frame_height    = 720;
//	pld.pld[0].decimation.scale_factor    = 2;
//
//	clc_cs(pld.pld_sz, &pld);
//	simple_dpp_pld_t_parser(&pld);
//	
//}
//
//void make_a_simple_test_temporal(){
//	printf("\nSimple test temporal only:");
//	dpp_pld_t pld;
//	memset(&pld, 0, sizeof(dpp_pld_t));  //just for the test ... 
//	
//	pld.pld_sz =  1;  // for temporal only.
//	pld.pld[0].temporal.header.cmd_id   = TEMPORAL_ID;
//	pld.pld[0].temporal.header.cmd_size = 24;
//	pld.pld[0].temporal.frame_width     = 480; 
//	pld.pld[0].temporal.frame_height    = 360;
//	pld.pld[0].temporal.delta   = 20;
//	pld.pld[0].temporal.alpha   = 0.4;
//
//	simple_dpp_pld_t_parser(&pld);
//}
//
//void make_a_simple_test_decimation_and_temporal(){
//	printf("\nSimple test decimation and temporal:");
//	dpp_pld_t pld;
//	memset(&pld, 0, sizeof(dpp_pld_t));  //just for the test ... 
//	
//	pld.pld_sz =  2;  // for decimation and temporal.
//	
//	//decimation.
//	pld.pld[0].decimation.header.cmd_id   = DECIMATION_ID;
//	pld.pld[0].decimation.header.cmd_size = 12;
//	pld.pld[0].decimation.frame_width     = 960; 
//	pld.pld[0].decimation.frame_height    = 720;
//	pld.pld[0].decimation.scale_factor    = 2;
//	
//	//temporal.
//	pld.pld[1].temporal.header.cmd_id   = TEMPORAL_ID;
//	pld.pld[1].temporal.header.cmd_size = 24;
//	pld.pld[1].temporal.frame_width     = 480; 
//	pld.pld[1].temporal.frame_height    = 360;
//	pld.pld[1].temporal.delta   = 20;
//	pld.pld[1].temporal.alpha   = 0.4;
//
//	simple_dpp_pld_t_parser(&pld);
//}
//
//void make_a_simple_test_decimation_and_temporal_and_new_algo(){
//	printf("\nSimple test decimation, temporal and new algo :");
//	dpp_pld_t pld;
//	memset(&pld, 0, sizeof(dpp_pld_t));  //just for the test ... 
//	
//	pld.pld_sz =  3;  // for decimation,temporal and new algo.
//	size_t i = 0;
//	
//	//decimation.
//	pld.pld[0].decimation.header.cmd_id   = DECIMATION_ID;
//	pld.pld[0].decimation.header.cmd_size = 12;
//	pld.pld[0].decimation.frame_width     = 960; 
//	pld.pld[0].decimation.frame_height    = 720;
//	pld.pld[0].decimation.scale_factor    = 2;
//	
//	//temporal.
//	pld.pld[1].temporal.header.cmd_id   = TEMPORAL_ID;
//	pld.pld[1].temporal.header.cmd_size = 24;
//	pld.pld[1].temporal.frame_width     = 480; 
//	pld.pld[1].temporal.frame_height    = 360;
//	pld.pld[1].temporal.delta   = 20;
//	pld.pld[1].temporal.alpha   = 0.4;
//	
//	//new algo.
//	pld.pld[2].new_algo.header.cmd_id   = NEW_ALGO_ID;
//	pld.pld[2].new_algo.header.cmd_size = 24;
//	pld.pld[2].new_algo.frame_width     = 1024; 
//	pld.pld[2].new_algo.frame_height    = 4096;
//	pld.pld[2].new_algo.delta   = 56;
//	pld.pld[2].new_algo.alpha   = 12.34;
//	
//	
//	clc_cs(pld.pld_sz, &pld);
//
//	simple_dpp_pld_t_parser(&pld);
//}
//
//
////=============================================================================
//
//
//// Restore to default padding.
//#pragma pack(pop)
//
///**
// * @brief Generic macro to trap size alignment violations at compile time
// *
// * A violation will result in a compilation error.
// * @param struct_type The structure type to check
// * @param alignment Byte alignment to enforce
// */
//#define CHECK_SIZE_ALIGNMENT(struct_type, alignment)                     \
//    do {                                                                 \
//        uint8_t arr[(sizeof(struct_type) % (alignment) == 0U) ? 1 : -1]; \
//        (void)arr;                                                       \
//    } while (false)
//
///**
// * @brief Macro to trap 32 bit size alignment violations at compile time
// */
//#define CHECK_ALIGN32(struct_type) CHECK_SIZE_ALIGNMENT(struct_type, sizeof(uint32_t))
//
///**
// * Compile time checks only - this function is not to be called!
// */
//static inline void dpp_msg_body_size_check(void) {
//    CHECK_ALIGN32(struct dpp_common);
//    CHECK_ALIGN32(struct dpp_decimation);
//    CHECK_ALIGN32(struct dpp_temporal);
//    CHECK_ALIGN32(struct dpp_payload);
//    CHECK_ALIGN32(struct dpp_payload);
//    CHECK_ALIGN32(dpp_algo_un_t);
//    CHECK_ALIGN32(dpp_pld_t);
//}
//
//
//
//
//
//int main(int argc, char *argv[]) {
//	printf("dpp_common %d \n",sizeof(struct dpp_common));	
//	printf("cmd_header %d \n",sizeof(struct cmd_header));
//	printf("dpp_decimation %d \n",sizeof(struct dpp_decimation));
//	printf("dpp_temporal %d \n",sizeof(struct dpp_temporal));
//	printf("dpp_new_algo %d \n",sizeof(struct dpp_new_algo));
//	printf("dpp_new_algo %d \n",sizeof(struct dpp_payload));
//	printf("dpp_algo_un_t %d \n",sizeof(dpp_algo_un_t));
//	printf("dpp_pld_t %d \n",sizeof(dpp_pld_t));
//	
//	
//	
//	make_a_simple_test_decimation();
//	make_a_simple_test_temporal();
//	make_a_simple_test_decimation_and_temporal();
//	make_a_simple_test_decimation_and_temporal_and_new_algo();
//	
//	return 0;
//}
