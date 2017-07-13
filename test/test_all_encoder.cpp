
#include <all_encoder/all_encoder.h>
#include <errno.h>

m_encoder *encoder;

int main(int argc, char **argv)
{
    if (argc < 5) {
        printf("usage, [exec] [in_file] [encoder] [width] [height]...\n");
        return -1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (NULL == fp) {
        printf("open file failed %s.\n", strerror(errno));
        return -1;
    }
    uint32_t width = atoi(argv[3]);
    uint32_t height = atoi(argv[4]);
    uint32_t input_size = width * height * 3 / 2;
    uint8_t *input_buf = (uint8_t*)malloc(input_size);

    int i, j;
    fseek(fp, 0, SEEK_END);
    int frame_num = ftell(fp) / input_size;
    fseek(fp, 0, SEEK_SET);

    uint8_t *output_buf = NULL;
    uint32_t output_size = 0;

    printf("fame_num: %d: argv[0]: %s, argv[1]: %s.\n", frame_num, argv[0], argv[1]);

    switch(atoi(argv[2])) {
        case 1:
            encoder = encoder_init(ENCODE_X264, width, height, X264_CSP_I420);
            break;
        case 2:
            encoder = encoder_init(ENCODE_X265, width, height, X264_CSP_I420);
            break;
    }
    encoder_open(encoder);

    for (i = 0; i < frame_num; i++) {
        fread(input_buf, 1, input_size, fp);
        all_fill_input_buffer(encoder, input_buf, input_size);
        if (encoder->codec_type == ENCODE_X264)
            encoder->m_x264_pic_in->i_pts = i;

        encoder_encode(encoder, &output_buf, &output_size);
//printf("[%s]: frame_num : %d encode_size: %d, output_buf: %p.\n\n", __FUNCTION__, i, output_size, output_buf);
        free(output_buf);
        if (encoder->codec_type == ENCODE_X265)
            output_size = 0;
    }

    encoder_close(encoder);
    return 0;
}

