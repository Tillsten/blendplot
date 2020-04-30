#pragma once
#include "./common.h"

class Canvas {
private:
  
  BLContext ctx;

public:
  double dpi = 144;
  oColor facecolor;
  BLSizeI size = BLSizeI();
  BLSize size_inch = BLSize();
  BLImage image;
  BLMatrix2D pixel2inch;
  BLMatrix2D inch2pixel;
  BLMatrix2D pixel2fig;
  BLMatrix2D fig2pixel;
  BLMatrix2D inch2fig;
  BLMatrix2D fig2inch;
  Simple::Signal<void(double, double)> sizeChanged;
  Simple::Signal<void(BLContext &)> drawRequested;

  void resize(BLImage &);
  void resize(int w, int h);
  void generate_transforms();

  Canvas(int w, int h) {
    image = BLImage();
    resize(w, h);
  };
  Canvas(BLImage &image) : image(image) { generate_transforms(); };

  int save_bmp(const char *fname) {
    printf("Saving");
    BLImageCodec codec;
    codec.findByName("BMP");
    BLResult res = image.writeToFile(fname, codec);
    printf("%d", res);
    if (res != BL_SUCCESS) {
      printf("%d", res);
    }
    return 1;
  }

  int draw() {
    
    BLContextCreateInfo cfg;
    cfg.threadCount = 4;

    ctx.begin(image, cfg);
    ctx.setCompOp(BL_COMP_OP_SRC_OVER);
    auto fc = facecolor.value_or(BLRgba32(255, 255, 255, 255));
    ctx.setFillStyle(fc);
    ctx.fillAll();
    ctx.setStrokeTransformOrder(
        BLStrokeTransformOrder::BL_STROKE_TRANSFORM_ORDER_BEFORE);
    ctx.transform(pixel2inch);

    drawRequested.emit(ctx);
    ctx.end();
    return 1;
  }
};

void Canvas::resize(BLImage &img) {

  image = std::move(img);
  size.reset(image.width(), image.height());
  ctx = BLContext(image);
  ctx.strokeLine(0, 0, size.w, size.h);
  ctx.end();
  generate_transforms();
  size_inch.reset(size.w / dpi, size.h / dpi);
  sizeChanged.emit(size_inch.w, size_inch.h);
}

void Canvas::resize(int w, int h) {

  size.reset(w, h);
  auto result = image.create(w, h, BL_FORMAT_PRGB32);

  if (result != BL_SUCCESS) {
    exit(-1);
  }
  generate_transforms();

  size_inch.reset((double)size.w / dpi, (double)size.h / dpi);
  sizeChanged.emit(size_inch.w, size_inch.h);
}

void Canvas::generate_transforms() {
  auto w = image.width();
  auto h = image.height();
  pixel2inch.resetToTranslation(0, h);
  pixel2inch.scale(1, -1);
  pixel2inch.scale(dpi);
  BLMatrix2D::invert(inch2pixel, pixel2inch);

  pixel2fig.resetToScaling(w, h);
  BLMatrix2D::invert(fig2pixel, pixel2fig);

  inch2fig.reset(inch2pixel);
  inch2fig.transform(pixel2fig);
  BLMatrix2D::invert(fig2inch, inch2fig);
}
