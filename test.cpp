#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>

#include "GContext.h"
#include "GBitmap.h"
#include "GColor.h"

static void inline assert_pixels(GContext* ctx, GPixel pixel) {
	GBitmap bm;
	ctx->getBitmap(&bm);
	for (int y = 0; y < bm.fHeight; ++y) {
		for (int x = 0; x < bm.fWidth; ++x)
			assert (bm.fPixels[y * bm.fRowBytes/4 + x] == pixel);
		for (int x = 0; x < bm.fRowBytes - bm.fWidth * 4; x++)
			assert(((char*)bm.fPixels)[y * bm.fRowBytes + bm.fWidth*4 + x] == 0);
	}
}

#define PIXELS_SET_PER_TEST 100000000
static void inline test_ctx(GContext* ctx, GPixel pixel, GColor color) {
	struct timespec start, end;
	GBitmap bm;
	ctx->getBitmap(&bm);
	memset(bm.fPixels, 0, bm.fRowBytes * bm.fHeight);
	int pixels = bm.fHeight * bm.fWidth;
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0; i < PIXELS_SET_PER_TEST/pixels; i++)
		ctx->clear(color);
	clock_gettime(CLOCK_MONOTONIC, &end);
	assert_pixels(ctx, pixel);
	printf("[%d, %d] took %lf ms per pixel.\n", bm.fWidth, bm.fHeight,
			((end.tv_sec - start.tv_sec) * 1000000000l + (end.tv_nsec - start.tv_nsec))/
			((PIXELS_SET_PER_TEST/pixels) * pixels * 1000.0));
}

int main(int argc, char** argv) {
    GPixel* storage = new GPixel[10000 * 10000];
    GBitmap bitmap;
    bitmap.fWidth = 100;
    bitmap.fHeight = 100;
    bitmap.fPixels = storage;
    bitmap.fRowBytes = bitmap.fWidth * sizeof(GPixel);

    GContext* ctx = GContext::Create(bitmap);
    if (!ctx) {
        fprintf(stderr, "GContext::Create failed\n");
        return -1;
    }
    
    GPixel pixel = 0xFFFF0000;
    GColor color = { 1, 1, 0, 0 };

	memset(storage, 0, sizeof(GPixel) * 10000 * 10000);
    ctx->clear(color);
	assert_pixels(ctx, pixel);
    delete ctx;

	ctx = GContext::Create(1, 1);
	color.fR = 0.502; color.fG = 0.26;
	pixel = 0xff804200;
	test_ctx(ctx, pixel, color);
	delete ctx;

	ctx = GContext::Create(1000, 1);
	test_ctx(ctx, pixel, color);
	delete ctx;

	ctx = GContext::Create(1, 1000);
	test_ctx(ctx, pixel, color);
	delete ctx;

	ctx = GContext::Create(100000, 1);
	test_ctx(ctx, pixel, color);
	delete ctx;

	ctx = GContext::Create(1, 100000);
	test_ctx(ctx, pixel, color);
	delete ctx;

	ctx = GContext::Create(10000, 10000);
	test_ctx(ctx, pixel, color);
	delete ctx;

	bitmap.fWidth = 1000;
	bitmap.fHeight = 10000;
	bitmap.fRowBytes = 10000*4;
	ctx = GContext::Create(bitmap);
	test_ctx(ctx, pixel, color);
	delete ctx;

	ctx = GContext::Create(1000000, 1000000);
	assert(!ctx);

	bitmap.fRowBytes = 1000;
	ctx = GContext::Create(bitmap);
	assert(!ctx);

	bitmap.fRowBytes = 4003;
	ctx = GContext::Create(bitmap);
	assert(!ctx);

	fprintf(stderr, "passed.\n");
	delete[] storage;
	return 0;
}

