#include "../CMakeProject1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../unistd.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
#include "libavutil/dict.h"

//获取流，解析流

int main(int argc, char** argv) {
	AVFormatContext* fmt_ctx = NULL;
	//AVCodecContext* dec_ctx = NULL;
	AVCodecParameters* dec_ctx = nullptr;
	const AVCodec* dec = nullptr;
	int video_stream_index = -1;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input>\n", argv[0]);
		exit(1);
	}
	// 打开输入文件并读取流信息
	if (argc < 2) {
		cout << "参数错误!\n";
		return -1;
	}
	string url = utils::GetRedirectUrl2(argv[1], 2);
	cout << "redirect url is" << url << endl;

	cout << "read input hls" << endl;
	//打开文件，即uri
	if (avformat_open_input(&fmt_ctx, url.c_str(), NULL, NULL) < 0) {
		fprintf(stderr, "Could not open input file '%s'\n", argv[1]);
		exit(1);
	}
	cout << "input fmt_ctx info is" << endl;
	cout << "audio_codec" << fmt_ctx->audio_codec << endl;
	cout << "data_codec" << fmt_ctx->data_codec << endl;
	cout << "url is " << fmt_ctx->url << endl;
	//获取文件的流信息
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		exit(1);
	}
	// 寻找视频流并打开解码器
	for (int i = 0; i < fmt_ctx->nb_streams; i++) {
		AVStream* stream = fmt_ctx->streams[i];
		cout << "stream info is[" << i << "]" << fmt_ctx->streams[i] << endl;
		dec_ctx = stream->codecpar;
		cout << "dec_ctx" << dec_ctx << endl;
		if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO && video_stream_index == -1) {
			dec = avcodec_find_decoder(dec_ctx->codec_id);
			if (!dec) {
				fprintf(stderr, "Failed to find decoder for stream #%u\n", i);
				exit(1);
			}
			video_stream_index = i;
		}
	}
	if (video_stream_index == -1) {
		fprintf(stderr, "Could not find video stream\n");
		exit(1);
	}
	// 打开解码器
	AVCodecContext* av_cc = nullptr;
	av_cc = avcodec_alloc_context3(dec);
	if (!dec_ctx) {
		fprintf(stderr, "Failed to allocate codec context\n");
		exit(1);
	}
	if (avcodec_parameters_to_context(av_cc, fmt_ctx->streams[video_stream_index]->codecpar) < 0) {
		fprintf(stderr, "Failed to copy codec parameters to decoder context\n");
		exit(1);
	}
	if (avcodec_open2(av_cc, dec, NULL) < 0) {
		fprintf(stderr, "Failed to open decoder for stream #%u\n", video_stream_index);
		exit(1);
	}
	// 查找视频流的HLS地址
	AVDictionaryEntry* opt = NULL;
	while ((opt = av_dict_get(fmt_ctx->streams[video_stream_index]->metadata, "", opt, AV_DICT_IGNORE_SUFFIX))) {
		cout << "opt->value1 is" << opt->value << "key is" << opt->key << endl;
		if (strcmp(opt->key, "uri") == 0 && strstr(opt->value, ".m3u8")) { // HLS地址以.m3u8结尾
			printf("Found HLS URL: %s\n", opt->value);
			// 使用FFmpeg播放HLS地址
			cout << "opt->value2 is" << opt->value << "key is" << opt->key << endl;
			char command[1024];
			snprintf(command, sizeof(command), "ffmpeg -i \"%s\" -c:v copy -c:a copy -f mpegts - | ffplay -i -", opt->value);
			system(command);
			break;
		}
	}
	// 清理资源并退出程序
	avcodec_free_context(&av_cc);
	avformat_close_input(&fmt_ctx);
	return 0;
}