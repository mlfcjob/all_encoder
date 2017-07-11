#ifndef __LIB_Z_ENCODER_H__
#define __LIB_Z_ENCODER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined __cplusplus
extern "C"{
#include <x265/x265.h>
#include <x265/x265_config.h>

#include <x264/x264.h>
#include <x264/x264_config.h>
}
#else
#include <x264/x264.h>
#include <x264/x264_config.h>
#include <x265/x265.h>
#include <x265/x265_config.h>
#endif


enum ENCODE_CODEC_TYPE {
    ENCODE_X264 = 1,
    ENCODE_X265 = 2,
    ENCODE_VP8  = 3,
    ENCODE_VP9  = 4,
    ENCODE_LAST,
};

typedef struct m_encoder{
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint8_t codec_type;
    uint32_t y_size;

    x264_picture_t *m_x264_pic_in;
    x264_picture_t *m_x264_pic_out;

    x265_picture *m_x265_pic_in;
    x265_picture *m_x265_pic_out;

    void *codec;
    void *param;
}m_encoder;

void *x264_param_init(m_encoder *encoder);
void *x265_param_init(m_encoder *encoder);

void z_fill_input_buffer(m_encoder *encoder, uint8_t *input_buf, uint32_t input_size);

m_encoder *encoder_init(uint32_t codec_type, uint32_t width, uint32_t height,
                        uint32_t format);
void encoder_open(m_encoder*);
void encoder_encode(m_encoder* encoder, uint8_t **output_buf, uint32_t *output_size);
void encoder_close(m_encoder*);

#endif
