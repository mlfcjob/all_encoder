
#include <stdlib.h>
#include <string.h>
#include "all_encoder.h"

const char *nal_type[13] = {
    "0:NAL_UNKNOWN",
    "1:NAL_SLICE",
    "2:NAL_SLICE_DPA",
    "3:NAL_SLICE_DPB", 
    "4:NAL_SLICE_DPC",
    "5:NAL_SLICE_IDR",
    "6:NAL_SEI",
    "7:NAL_SPS",
    "8:NAL_PPS",
    "9:NAL_AUD",
    "null",
    "null",
    "10:NAL_FILLER" };

void *x265_param_init(m_encoder *encoder)
{
    x265_param *param = x265_param_alloc();


	x265_param_default(param);
	param->bRepeatHeaders = 1;  // write sps, pps before key frame
	param->internalCsp = encoder->format;
	param->sourceWidth = encoder->width;
	param->sourceHeight = encoder->height;
	param->fpsNum = 25;
	param->fpsDenom = 1;

    encoder->m_x265_pic_in = x265_picture_alloc();
    x265_picture_init(param, encoder->m_x265_pic_in); 

    return (void*)param;
}

void *x264_param_init(m_encoder *encoder)
{
    encoder->m_x264_pic_in = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    x264_param_t  *param   = (x264_param_t*)malloc(sizeof(x264_param_t)); 

    x264_param_default(param);
    param->i_width = encoder->width;
    param->i_height = encoder->height;
    param->i_threads = 4;
    param->i_bframe = 0;
   
    /* analyse */ 
    param->analyse.inter = X264_ANALYSE_I4x4 | X264_ANALYSE_I8x8 | X264_ANALYSE_PSUB16x16;


    /* deadzone to reduce data size */
    //param->analyse.i_trellise = 0 ; // ? 1 开 / 关

    param->analyse.i_luma_deadzone[0] = 32;
    param->analyse.i_luma_deadzone[1] = 32; 

    /* open psnr */
    param->analyse.b_psnr = 1;

    /* rate control */
    /*VBR*/
    param->rc.i_rc_method = X264_RC_ABR;
    param->rc.i_bitrate = 60; // bps

    /*CQP*/
    //param->rc.i_rc_method = X264_RC_CQP;

    /* defualt crf */
    //param->rc.f_rf_constant = 35;  // 23 29 35 增加6，码率减少一半


    /* mux parameter */
    //`param->b_annexb = 0;  // default : 1

    param->i_csp = encoder->format;
    x264_param_apply_profile(param, x264_profile_names[5]);

    x264_picture_alloc(encoder->m_x264_pic_in, encoder->format, encoder->width, encoder->height);

printf("\n***************************cpu info***********************.\n");
printf("cpu: %08x, i_threads:%d, i_lookhead_threads:%d,"
		"b_sliced_threads:%d,"
		"b_deterministic:%d,"
		"b_cpu_independent: %d,"
		"i_sync_lookhead:%d,"
		"b_opencl:%d.\n", 
		param->cpu, param->i_threads, 
		param->i_lookahead_threads,
		param->b_sliced_threads,
		param->b_deterministic,
		param->b_cpu_independent,
		param->i_sync_lookahead,
		param->b_opencl);

printf("\n***************************video properties************************.\n");
printf("video properites: i_level_idc: %d.\n", param->i_level_idc);

printf("\n***************************nal hrd************************.\n");
printf("i_nal_hrd: %d.\n", param->i_nal_hrd);

printf("\n****************************VUI***************************.\n");
printf("i_sar_height:%d, i_sar_width:%d,"
	"i_overscan:%d,i_vidformat:%d,"
	"b_fullrange:%d, i_colorprim:%d,"
	"i_transfer:%d,i_colmatrix:%d,"
	"i_chroma_loc:%d.\n",
	param->vui.i_sar_height,
	param->vui.i_sar_width,
	param->vui.i_overscan,
	param->vui.i_vidformat,
	param->vui.b_fullrange,
	param->vui.i_colorprim,
	param->vui.i_transfer,
	param->vui.i_colmatrix,
	param->vui.i_chroma_loc);

printf("\n****************************bit stream parameters***************************.\n");
printf("i_frame_reference:%d, i_dpb_size:%d, "    
       "i_keyint_max:%d, i_keyint_min:%d," 
       "i_scenecut_threshold: %d," 
       "b_intra_refresh:%d,"
       "i_bframe:%d,"   
       "i_bframe_adaptive:%d,"
       "i_bframe_bias:%d,"
       "i_bframe_pyramid:%d,"   
       "b_open_gop:%d,"
       "b_bluray_compat:%d,"
       "i_avcintra_class:%d.\n",
       param->i_frame_reference,
       param->i_dpb_size,
       param->i_keyint_max,
       param->i_keyint_min,
       param->i_scenecut_threshold,
       param->b_intra_refresh,
       param->i_bframe,
       param->i_bframe_adaptive,
       param->i_bframe_bias,
       param->i_bframe_pyramid,
       param->b_open_gop,
       param->b_bluray_compat,
       param->i_avcintra_class);



printf("\n****************************CQM Quant***************************.\n");
printf("i_cqm_preset:%d, psz_cqm_file:%s.\n", param->i_cqm_preset, param->psz_cqm_file);
printf("cqm_4iy:\n");
int i, j;
for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
        printf("%d, ", param->cqm_4iy[ i * 4 + j ]);
    }
    printf("\n");
}


printf("cqm_4py:\n");
for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
        printf("%d, ", param->cqm_4py[ i * 4 + j ]);
    }
    printf("\n");
}

printf("cqm_4ic:\n");
for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
        printf("%d, ", param->cqm_4ic[ i * 4 + j ]);
    }
    printf("\n");
}

printf("cqm_4pc:\n");
for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
        printf("%d, ", param->cqm_4pc[ i * 4 + j]);
    }
    printf("\n");
}


printf("cqm_8iy:\n");
for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
        printf("%d, ", param->cqm_8iy[ i * 8 + j ]);
    }
    printf("\n");
}

printf("cqm_8py:\n");
for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
        printf("%d, ", param->cqm_8py[ i * 8 + j]);
    }
    printf("\n");
}

printf("cqm_8ic:\n");
for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
        printf("%d, ", param->cqm_8ic[ i * 8 + j]);
    }
    printf("\n");
}

printf("cqm_8pc:\n");
for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
        printf("%d, ", param->cqm_8pc[ i * 8 + j]);
    }
    printf("\n");
}

printf("\n****************************LOG***************************.\n");
printf("pf_log: %p,  p_log_private: %s, i_log_level: %d, b_full_recon: %d, psz_dump_yuv: %s.\n",
		param->pf_log, param->p_log_private, param->i_log_level, param->b_full_recon, param->psz_dump_yuv);

printf("\n****************************Encoder analasyse***************************.\n");
printf("intra: %d, inter: %d, b_transform_8x8: %d.\n", param->analyse.intra, param->analyse.inter, param->analyse.b_transform_8x8);
printf("i_weighted_pred: %d, b_weiget_bipred: %d.\n", param->analyse.i_weighted_pred, param->analyse.b_weighted_bipred);
printf("i_direct_mv_pred: %d, i_chroma_qp_offset: %d.\n", param->analyse.i_direct_mv_pred, param->analyse.i_chroma_qp_offset);
printf("i_me_method: %d, i_me_range: %d, i_mv_range: %d, i_mv_range_thread:%d.\n", param->analyse.i_me_method,
		                                                                   param->analyse.i_me_range,
										   param->analyse.i_mv_range,
										   param->analyse.i_mv_range_thread);
printf("i_subpel_refine: %d, b_chroma_me: %d.\n", param->analyse.i_subpel_refine, param->analyse.b_chroma_me);
printf("b_mixed_references: %d.\n", param->analyse.b_mixed_references);
printf("i_trellis: %d, b_fast_skip: %d.\n", param->analyse.i_trellis, param->analyse.b_fast_pskip);
printf("b_dct_decimate: %d, i_noise_reduction: %d .\n", param->analyse.b_dct_decimate, param->analyse.i_noise_reduction);
printf("f_psy_rd: %d, f_psy_trellis: %d.\n", param->analyse.f_psy_rd, param->analyse.f_psy_trellis);
printf("b_psy: %d, b_mb_info: %d, b_mb_info_update: %d.\n", param->analyse.b_psy, param->analyse.b_mb_info, param->analyse.b_mb_info_update);
printf("i_luma_deadzone[0]: %d, i_luma_deadzone[1]: %d.\n", param->analyse.i_luma_deadzone[0], param->analyse.i_luma_deadzone[1]);
printf("b_psnr: %d, b_ssim: %d. \n", param->analyse.b_psnr, param->analyse.b_ssim);

printf("\n****************************Rate control parameters ***************************.\n");
printf("i_rc_method: %d, q_qp_constant: %d, i_qp_min: %d, i_qp_max: %d, i_qp_step: %d.\n", param->rc.i_rc_method, param->rc.i_qp_constant, 
		param->rc.i_qp_min, param->rc.i_qp_max, param->rc.i_qp_step);    
printf("i_bitrate: %lf, f_rf_constant:%lf, f_rf_constant_max: %lf, f_rate_tolerance: %lf.\n", param->rc.i_bitrate, param->rc.f_rf_constant,
		param->rc.f_rf_constant_max, param->rc.f_rate_tolerance);
printf("i_vbv_max_bitrate: %d, i_vbv_buffer_size: %d.\n", param->rc.i_vbv_max_bitrate, param->rc.i_vbv_buffer_size);
printf("f_vbv_buffer_init: %lf, f_ip_factor: %lf, f_pb_factor: %lf.\n", param->rc.f_vbv_buffer_init, param->rc.f_ip_factor, param->rc.f_pb_factor);
printf("b_filler: %d.\n", param->rc.b_filler);
printf("i_aq_mode: %d, f_aq_strength: %lf, b_mb_tree: %d, i_lookahead: %d.\n", param->rc.i_aq_mode, param->rc.f_aq_strength, param->rc.b_mb_tree, param->rc.i_lookahead);
printf("two pass.\n");
printf("b_stat_write: %d, psz_stat_out: %s, b_state_wirte: %d, psz_stat_in: %s.\n", param->rc.b_stat_write, param->rc.psz_stat_out, param->rc.b_stat_read, param->rc.psz_stat_in);
printf("two pass parameters .\n");
printf("f_qcompress: %lf, f_qblur: %lf, f_complexity_blur: %lf, i_zones: %d, psz_ones: %s.\n", param->rc.f_qcompress, param->rc.f_qblur,
		       param->rc.f_complexity_blur, param->rc.i_zones, param->rc.psz_zones);
for(i = 0; i < param->rc.i_zones; i++) {
    printf("i_start: %d, i_end: %d, b_force_qp: %d, i_qp:%d, f_bitrate_factor: %lf. \n", param->rc.zones[i].i_start,
		    param->rc.zones[i].i_end, param->rc.zones[i].b_force_qp, 
		    param->rc.zones[i].i_qp, param->rc.zones[i].f_bitrate_factor);
    printf("param: %p.\n\n", param->rc.zones[i].param);
}

printf("\n****************************Crop rect ***************************.\n");
printf("left: %d, top: %d, right: %d, bottome: %d.\n", param->crop_rect.i_left, param->crop_rect.i_top, param->crop_rect.i_right, param->crop_rect.i_bottom);

printf("\n****************************I frame packing ***************************.\n");
printf("i_frame_packing: %d .\n", param->i_frame_packing);

printf("\n****************************Muxing paramters ***************************.\n");
printf("b_aud: %d, b_repeat_headers: %d, b_annexb %d .\n", param->b_aud, param->b_repeat_headers, param->b_annexb);
printf("i_sps_id: %d, b_vfr_input: %d, b_pullown: %d, i_fps_num: %d, i_fps_den: %d.\n", param->i_sps_id, param->b_vfr_input,
		param->b_pulldown, param->i_fps_num, param->i_fps_den);
printf("i_timebase_num: %d, i_timebase_den: %d.\n", param->i_timebase_num, param->i_timebase_den);

printf("\n****************************b pic struct ***************************.\n");
printf("b_pic_struct: %d. \n", param->b_pic_struct);

printf("\n****************************b fake interlaced  ***************************.\n");
printf("b_fake_interlaced: %d. \n", param->b_fake_interlaced);


printf("\n****************************b switchable  ***************************.\n");
printf("b_stichable: %d. \n", param->b_stitchable);

printf("\n**************************** opencl  ***************************.\n");
printf("b_opencl: %d, i_opencl_device: %d, opencl_device_id: %s, psz_clbin_file: %s. \n", param->b_opencl, param->i_opencl_device, param->opencl_device_id, param->psz_clbin_file );


printf("\n**************************** slicing parameters  ***************************.\n");
printf("i_slice_max_size: %d, i_slice_max_mbs: %d, i_slice_min_mbs: %d, i_slice_count: %d, i_slice_count_max: %d.\n",
				param->i_slice_max_size, param->i_slice_max_mbs,
				param->i_slice_min_mbs, param->i_slice_count, param->i_slice_count_max);


printf("\n**************************** param free callback  ***************************.\n");
printf("%p.\n", param->param_free);

printf("\n**************************** nalu process  ***************************.\n");
printf("%p.\n", param->nalu_process);

printf("\n");

    return (void*)param;
}

static void z_fill_x264_input_buffer(m_encoder *encoder, uint8_t *input_buf, uint32_t input_size)
{
    switch(encoder->format) {
        case X264_CSP_I420: {
            if (encoder->y_size *3 / 2 != input_size) {
                printf(" X264_CSP_I420: invalid size.\n");
                return;
            }
            memcpy(encoder->m_x264_pic_in->img.plane[0], input_buf, encoder->y_size);
            memcpy(encoder->m_x264_pic_in->img.plane[1], input_buf + encoder->y_size, encoder->y_size / 4);
            memcpy(encoder->m_x264_pic_in->img.plane[2], input_buf + encoder->y_size * 5 / 4, encoder->y_size / 4);
            break;
        }
        case X264_CSP_I444:{
            if (encoder->y_size != input_size) {
                printf(" X264_CSP_I444: invalid size.\n");
                return;
            }
            memcpy(encoder->m_x264_pic_in->img.plane[0], input_buf, encoder->y_size);
            memcpy(encoder->m_x264_pic_in->img.plane[1], input_buf + encoder->y_size, encoder->y_size);
            memcpy(encoder->m_x264_pic_in->img.plane[2], input_buf + encoder->y_size * 2, encoder->y_size);
            break;
        }
    }

}

static void z_fill_x265_input_buffer(m_encoder *encoder, uint8_t *input_buf, uint32_t input_size)
{
    uint8_t *buff = NULL;

    switch(encoder->format) {
        case X265_CSP_I420:{
            if (encoder->y_size * 3 / 2 != input_size) {
                printf("X265_CSP_I420, invlid size.\n");
                return;
            }
            buff = (uint8_t*)malloc(encoder->y_size * 3 / 2);
            encoder->m_x265_pic_in->planes[0] = buff;
            encoder->m_x265_pic_in->planes[1] = buff + encoder->y_size;
            encoder->m_x265_pic_in->planes[2] = buff + encoder->y_size * 5 / 4;

            encoder->m_x265_pic_in->stride[0] = encoder->width;
            encoder->m_x265_pic_in->stride[1] = encoder->width / 2;
            encoder->m_x265_pic_in->stride[2] = encoder->width / 2;
           
            memcpy(encoder->m_x265_pic_in->planes[0], input_buf, encoder->y_size);
            memcpy(encoder->m_x265_pic_in->planes[1], input_buf + encoder->y_size, encoder->y_size / 4);
            memcpy(encoder->m_x265_pic_in->planes[2], input_buf + encoder->y_size * 5 / 4, encoder->y_size / 4);
            break;
        }
        case X265_CSP_I444:{
            if ( encoder->y_size * 3 != input_size) {
                printf("X265_CSP_I444, invalid size.\n");
                return;
            }

            buff = (uint8_t*)malloc(encoder->y_size * 3);
            encoder->m_x265_pic_in->planes[0] = buff;
            encoder->m_x265_pic_in->planes[1] = buff + encoder->y_size;
            encoder->m_x265_pic_in->planes[2] = buff + encoder->y_size * 2;
            encoder->m_x265_pic_in->stride[0] = encoder->width;
            encoder->m_x265_pic_in->stride[1] = encoder->width;
            encoder->m_x265_pic_in->stride[2] = encoder->width;

            memcpy(encoder->m_x265_pic_in->planes[0], input_buf, encoder->y_size);
            memcpy(encoder->m_x265_pic_in->planes[1], input_buf + encoder->y_size, encoder->y_size);
            memcpy(encoder->m_x265_pic_in->planes[2], input_buf + encoder->y_size * 2, encoder->y_size);
            break;
        }
    }
}


void z_fill_input_buffer(m_encoder *encoder, uint8_t *input_buf, uint32_t input_size)
{
     switch(encoder->codec_type) {
         case ENCODE_X264: {
             z_fill_x264_input_buffer(encoder, input_buf, input_size);
             break;
         }
         case ENCODE_X265: {
             z_fill_x265_input_buffer(encoder, input_buf, input_size);
             break;
         }
         case ENCODE_VP8: {
             break;
         }
         case ENCODE_VP9: {
             break;
         }
     }
}

m_encoder *encoder_init(uint32_t codec_type, uint32_t width, uint32_t height,
                        uint32_t format)
{
    m_encoder *g_encoder = (m_encoder*)malloc(sizeof(m_encoder));
    g_encoder->width = width;
    g_encoder->height = height;
    g_encoder->format = format;
    g_encoder->codec_type = codec_type;
    g_encoder->y_size = width * height; 

    switch (codec_type) {
       case ENCODE_X264: {
           if ( NULL == (g_encoder->param = x264_param_init(g_encoder))) {
               printf("invalide init x264 encoder.\n");
               return NULL;
           }
           break;
       } 
       case ENCODE_X265: {
           if( NULL == (g_encoder->param = x265_param_init(g_encoder))) {
               printf("invlid init x265 encoder.\n");
               return NULL;
           }
           break;
       } 
       case ENCODE_VP8: {
           break;
       }
       case ENCODE_VP9: {
           break;
       }
    }

    return g_encoder;
}


void encoder_open(m_encoder* encoder)
{
	x264_t *codec = NULL;
    switch (encoder->codec_type) {
        case ENCODE_X264: {
            encoder->codec = x264_encoder_open((x264_param_t*)encoder->param);
            break;
        }
        case ENCODE_X265: {
            encoder->codec = x265_encoder_open((x265_param*)encoder->param);
            if ( NULL == encoder->codec) {
                printf("err open x265 encoder.\n");
                return;
            }
            break;
        }
    }
}

void encoder_encode(m_encoder* encoder, uint8_t **output_buf, uint32_t *output_size)
{
    uint32_t encode_size;
    uint32_t i;

    switch(encoder->codec_type) {
        case ENCODE_X264: {
            x264_nal_t *p_nals = NULL;
            int i_nal;
            encoder->m_x264_pic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));
            encode_size = x264_encoder_encode((x264_t*)encoder->codec, &p_nals, &i_nal, encoder->m_x264_pic_in, encoder->m_x264_pic_out);
            if (encode_size < 0) {
                printf("x264 encode error.\n");
                return;
            }
            *output_size = encode_size;
            *output_buf = (uint8_t*)malloc(encode_size * sizeof(uint8_t));
            uint8_t *out_buf =  *output_buf;

//printf("\n[%s] encode_size:%d, output_size:%d, output_buf:%p, *output_buf:%p.\n", __FUNCTION__, encode_size, *output_size, output_buf, *output_buf);
            for(i = 0; i < i_nal; i++) {
                memcpy(out_buf, p_nals[i].p_payload, p_nals[i].i_payload);
                out_buf += p_nals[i].i_payload;
		char *p = (char*) (p_nals[i].p_payload);
		//printf("size: %d.\n", p_nals[i].i_payload);
		//printf("%02x, %02x, %02x, %02x. \n",  p[0], p[1], p[2], p[3]);
	        //printf("%lu. \n", (p[0] << 24) | (p[1] << 16) | (p[2] << 8)  | (p[3]) );	
//printf("[%s]:  p_nals[%d].payload: %p, p_nals[%d].i_payload:%d.\n", __FUNCTION__, i, p_nals[i].p_payload, i, p_nals[i].i_payload);
                //printf("i_type: %s.\n", nal_type[p_nals[i].i_type]);
            }
            break;
        }
        case ENCODE_X265: {
            x265_nal *p_nals = NULL;
            uint32_t i_nal;
            encoder->m_x265_pic_out = (x265_picture*)malloc(sizeof(x265_picture));
            encode_size = x265_encoder_encode((x265_encoder*)encoder->codec, &p_nals, &i_nal, encoder->m_x265_pic_in, NULL);
            if (encode_size < 0) {
                printf("x265 encode error.\n");
                return;
            }

            *output_buf = (uint8_t*)malloc(4096 * sizeof(uint8_t));
            uint8_t *out_buf = *output_buf;

            for(i = 0; i < i_nal; i++) {
                memcpy(out_buf, p_nals[i].payload, p_nals[i].sizeBytes);
                //printf("[%s]:  p_nals[%d].payload: %p, p_nals[%d].sizeBytes:%d.\n", __FUNCTION__, i, p_nals[i].payload, i, p_nals[i].sizeBytes);
                out_buf += p_nals[i].sizeBytes;
                *output_size += p_nals[i].sizeBytes;
            }
            break;
        }
       
    }

}

void encoder_close(m_encoder*  encoder)
{

    switch (encoder->codec_type) {
        case ENCODE_X264: {
            x264_picture_clean(encoder->m_x264_pic_in);
            //x264_picture_clean(encoder->m_x264_pic_out);
            x264_encoder_close((x264_t*)encoder->codec);
            break;
        }
        case ENCODE_X265: {
            x265_picture_free(encoder->m_x265_pic_in);
            x265_picture_free(encoder->m_x265_pic_out);
            x265_encoder_close((x265_encoder*)encoder->codec);
            break;
        }
    }

}
