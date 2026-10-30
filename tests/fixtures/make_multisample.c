#include <tiffio.h>
#include <stdint.h>
int main(int argc, char **argv) {
  if (argc != 2) return 2;
  TIFF *t = TIFFOpen(argv[1], "w");
  if (!t) return 3;
  uint32_t width = 1, height = 1;
  uint16_t bps = 8, spp = 9, photo = PHOTOMETRIC_RGB;
  uint16_t planar = PLANARCONFIG_SEPARATE, compression = COMPRESSION_NONE;
  uint16_t extra_samples = spp - 3;
  uint16_t extra_types[6] = {
    EXTRASAMPLE_UNSPECIFIED, EXTRASAMPLE_UNSPECIFIED,
    EXTRASAMPLE_UNSPECIFIED, EXTRASAMPLE_UNSPECIFIED,
    EXTRASAMPLE_UNSPECIFIED, EXTRASAMPLE_UNSPECIFIED
  };
  TIFFSetField(t, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(t, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(t, TIFFTAG_BITSPERSAMPLE, bps);
  TIFFSetField(t, TIFFTAG_SAMPLESPERPIXEL, spp);
  TIFFSetField(t, TIFFTAG_PHOTOMETRIC, photo);
  TIFFSetField(t, TIFFTAG_EXTRASAMPLES, extra_samples, extra_types);
  TIFFSetField(t, TIFFTAG_PLANARCONFIG, planar);
  TIFFSetField(t, TIFFTAG_COMPRESSION, compression);
  TIFFSetField(t, TIFFTAG_ROWSPERSTRIP, height);
  for (uint16_t s = 0; s < spp; ++s) {
    uint8_t v = (uint8_t)(s + 1);
    if (TIFFWriteScanline(t, &v, 0, s) < 0) { TIFFClose(t); return 4; }
  }
  TIFFClose(t);
  return 0;
}
