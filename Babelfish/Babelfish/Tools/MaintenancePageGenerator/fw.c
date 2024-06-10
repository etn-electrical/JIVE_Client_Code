#include "fw.h"

void fw_www_init_search_ctx(const fw_www_header* www, fw_www_search_ctx* ctx){
	ctx->low = 0;
	ctx->high = www->file_count - 1;

	ctx->str_idx = 0;
}

unsigned fw_www_search(const fw_www_header* www, const char* path, unsigned pathlen,
	fw_www_search_ctx* ctx)
{
	const char* x = path;
	const char* end = path + pathlen;
	while(x != end){

		const char* low_str = fw_www_get_filename(www, ctx->low);
		const char* high_str = fw_www_get_filename(www, ctx->high);
		while(ctx->low <= ctx->high && *x > low_str[ctx->str_idx + x - path]){
			++ctx->low;
			low_str = fw_www_get_filename(www, ctx->low);
		}

		if(*x != low_str[ctx->str_idx + x - path])
			return 0;

		while(ctx->low <= ctx->high && *x < high_str[ctx->str_idx + x - path]){
			--ctx->high;
			high_str = fw_www_get_filename(www, ctx->high);
		}

		/* Go onto next character. */
		++x;
	}

	ctx->str_idx = pathlen;

	return ctx->high - ctx->low + 1;
}

unsigned fw_www_match_count(const fw_www_search_ctx* ctx){
	return ctx->high - ctx->low + 1;
}
