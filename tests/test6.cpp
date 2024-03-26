
//
// Created by andrew on 2020/11/1.
//
#include <iostream>
#include <string>

extern "C" {
#include <libavutil/log.h>
#include <libavformat/avformat.h>
}
// aacÿ֡��ͷ��Ҫ��д��Ӧ�ĸ�ʽ��Ϣ
//��ȡ��Ƶ
void adts_header(char* szAdtsHeader, int dataLen) {

    int audio_object_type = 2;
    int sampling_frequency_index = 7;
    int channel_config = 2;

    int adtsLen = dataLen + 7;

    szAdtsHeader[0] = 0xff;         //syncword:0xfff                          ��8bits
    szAdtsHeader[1] = 0xf0;         //syncword:0xfff                          ��4bits
    szAdtsHeader[1] |= (0 << 3);    //MPEG Version:0 for MPEG-4,1 for MPEG-2  1bit
    szAdtsHeader[1] |= (0 << 1);    //Layer:0                                 2bits
    szAdtsHeader[1] |= 1;           //protection absent:1                     1bit

    szAdtsHeader[2] = (audio_object_type - 1) << 6;            //profile:audio_object_type - 1                      2bits
    szAdtsHeader[2] |= (sampling_frequency_index & 0x0f) << 2; //sampling frequency index:sampling_frequency_index  4bits
    szAdtsHeader[2] |= (0 << 1);                             //private bit:0                                      1bit
    szAdtsHeader[2] |= (channel_config & 0x04) >> 2;           //channel configuration:channel_config               ��1bit

    szAdtsHeader[3] = (channel_config & 0x03) << 6;     //channel configuration:channel_config      ��2bits
    szAdtsHeader[3] |= (0 << 5);                      //original��0                               1bit
    szAdtsHeader[3] |= (0 << 4);                      //home��0                                   1bit
    szAdtsHeader[3] |= (0 << 3);                      //copyright id bit��0                       1bit
    szAdtsHeader[3] |= (0 << 2);                      //copyright id start��0                     1bit
    szAdtsHeader[3] |= ((adtsLen & 0x1800) >> 11);           //frame length��value   ��2bits

    szAdtsHeader[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    �м�8bits
    szAdtsHeader[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    ��3bits
    szAdtsHeader[5] |= 0x1f;                                 //buffer fullness:0x7ff ��5bits
    szAdtsHeader[6] = 0xfc;
}
/*
 * �Ӷ�ý���ļ��г�ȡý����Ϣ
 * */
using namespace std;
int main(int argc, char* argv[]) {

    AVFormatContext* fmt_ctx = NULL;
    av_log_set_level(AV_LOG_INFO);
    /*���н��в���ǰ����ִ�����£�������Ҫ�Լ��ƶ�����*/
    //av_register_all();
    string video = "./1708314632-1-1695377756.hls.ts";
    string cmd = "ffmpeg -i "+ video +" -c:v libx264 -c:a aac -strict experimental -b:a 128k output.mp4";
    system(cmd.c_str());
    // 1. ��ȡ��ý���ļ�
    char* pSrc = NULL;
    char* pDst = NULL;
    pSrc = (char*)"./output.mp4";
    pDst = (char*)"test.aac";

    /*Open an input stream and read the header*/
    int ret = avformat_open_input(&fmt_ctx, "./output.mp4", NULL, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "can't open file.\n");
        return -1;
    }
    //  write audio data to AAC file
    FILE* dst_fd = fopen(pDst, "wb");
    if (dst_fd == NULL) {
        av_log(NULL, AV_LOG_ERROR, "open dst_fd failed.\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }
    /*
     * Print detailed information about the input or output format
     * */
    av_dump_format(fmt_ctx, 0, "./output.mp4", 0);

    // 2. get stream
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "ret = %d\n", ret);
        avformat_close_input(&fmt_ctx);
        fclose(dst_fd);
        return -1;
    }
    int audio_index = -1;
    audio_index = ret;
    AVPacket pkt;
    /*Initialize optional fields of a packet with default values.*/
    av_init_packet(&pkt);

    int len = -1;
    /*����ԭʼ���ݣ�����ʱ��Ҫ���AAC����Ƶ��ʽ˵����ͷ*/
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == audio_index) {
            /*ÿ֡��ͷ��Ҫд*/
            char adts_header_buf[7];
            adts_header(adts_header_buf, pkt.size);
            fwrite(adts_header_buf, 1, 7, dst_fd);
            len = fwrite(pkt.data, 1, pkt.size, dst_fd);
            if (len != pkt.size) {
                av_log(NULL, AV_LOG_ERROR, "waning, length is not equl size of pkt.\n");
                return -1;
            }
        }
        /*Wipe the packet.*/
        av_packet_unref(&pkt);
    }

    /**/

    /*Close an opened input AVFormatContext*/
    avformat_close_input(&fmt_ctx);
    if (dst_fd != NULL)
        fclose(dst_fd);

    return 0;
}

