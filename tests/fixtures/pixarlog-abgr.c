#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 64

static int generate(const char *path)
{
    TIFF *tif = TIFFOpen(path, "w");
    unsigned char row[IMAGE_WIDTH * 3];
    if (!tif)
        return 2;

    if (!TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, IMAGE_WIDTH) ||
        !TIFFSetField(tif, TIFFTAG_IMAGELENGTH, IMAGE_HEIGHT) ||
        !TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8) ||
        !TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3) ||
        !TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB) ||
        !TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG) ||
        !TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_PIXARLOG) ||
        !TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, IMAGE_HEIGHT)) {
        TIFFClose(tif);
        return 2;
    }

    for (uint32_t y = 0; y < IMAGE_HEIGHT; ++y) {
        for (uint32_t x = 0; x < IMAGE_WIDTH; ++x) {
            row[x * 3] = (unsigned char)(x + y);
            row[x * 3 + 1] = (unsigned char)(x * 3 + y);
            row[x * 3 + 2] = (unsigned char)(x + y * 5);
        }
        if (TIFFWriteScanline(tif, row, y, 0) < 0) {
            TIFFClose(tif);
            return 2;
        }
    }
    TIFFClose(tif);
    return 0;
}

static int decode(const char *path)
{
    TIFF *tif = TIFFOpen(path, "r");
    if (!tif)
        return 2;

    uint16_t samples = 0;
    uint16_t compression = 0;
    if (!TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples) || samples != 3 ||
        !TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression) ||
        compression != COMPRESSION_PIXARLOG ||
        !TIFFSetField(tif, TIFFTAG_PIXARLOGDATAFMT, PIXARLOGDATAFMT_8BITABGR)) {
        TIFFClose(tif);
        return 2;
    }

    tmsize_t strip_size = TIFFStripSize(tif);
    if (strip_size != IMAGE_WIDTH * IMAGE_HEIGHT * 3 || strip_size > 1024 * 1024) {
        TIFFClose(tif);
        return 2;
    }
    unsigned char *buffer = (unsigned char *)malloc((size_t)strip_size);
    if (!buffer) {
        TIFFClose(tif);
        return 2;
    }
    memset(buffer, 0xa5, (size_t)strip_size);

    tmsize_t result = TIFFReadEncodedStrip(tif, 0, buffer, strip_size);
    printf("decode_called=1 result=%lld strip_size=%lld\n",
           (long long)result, (long long)strip_size);
    free(buffer);
    TIFFClose(tif);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    if (strcmp(argv[1], "generate") == 0)
        return generate(argv[2]);
    if (strcmp(argv[1], "decode") == 0)
        return decode(argv[2]);
    return 2;
}
