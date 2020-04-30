#pragma once
#include "common.h"
#include "xtensor/xvectorize.hpp"
#include <blend2d.h>
#include <concepts>
#include <functional>
#include <iterator>
#include <vector>

class Artist {
public:
  virtual void draw(BLContext &ctx){};
  oColor facecolor;
  oColor strokeColor;
  float zOrder{0.};
};

//void generate_rects(xt::xarray<double> x, xt::xarray<double> y,
//                    std::function<double(double, double)> func) {
//  auto mg = xt::meshgrid(x, y);
//  auto f = xt::vectorize(func);
//  f(std::tie(mg));
//}

class Line : public Artist {
public:
  BLBox bbox;
  BLPath path;
  BLRgba32 linecolor = BLRgba32(255, 0, 0, 255);
  double linewidth = 3.0;
  void draw(BLContext &ctx) override;
  void set_data(const std::vector<double> &, const std::vector<double> &);
  void add_to_path(double, double);
};

void Line::set_data(const std::vector<double> &new_x,
                    const std::vector<double> &new_y) {
  path.reset();
  path.reserve(new_x.size());
  path.moveTo(new_x[0], new_y[0]);
  for (int i = 1; i < new_x.size(); i++) {
    path.lineTo(new_x[i], new_y[i]);
  };
};

void Line::add_to_path(double x, double y) { path.lineTo(x, y); }
void Line::draw(BLContext &ctx) {
  ctx.setStrokeStyle(linecolor);
  ctx.setStrokeWidth(linewidth);
  ctx.strokePath(path);
};

enum class TextVA {
  top,
  center,
  bottom,
};

enum class TextHA {
  left,
  center,
  right,
};

class Text : public Artist {
private:
  BLGlyphBuffer gb{};
  BLTextMetrics tm{};

public:
  BLFont font;
  std::wstring text;
  BLPoint pos;
  int fontsize;

  TextVA va = TextVA::top;
  TextHA ha = TextHA::center;

  Text(std::wstring txt, double x, double y) {
    pos.reset(x, y);

    BLFontFace face;
    BLResult ret = face.createFromFile("C:\\Windows\\Fonts\\calibril.ttf");
    if (ret != BL_SUCCESS) {
      throw;
    }
    font.createFromFace(face, 20);
    set_text(txt);
  }

  Text(const std::wstring &txt, BLPoint pos) : pos(pos) { set_text(txt); }

  void set_text(const std::wstring &text) {
    this->text = text;
    gb.setWCharText(text.data(), text.size());
    font.shape(gb);
    font.getTextMetrics(gb, tm);
  }

  void draw(BLContext &ctx) {
    BLPoint shifted_pos = pos;
    auto bbox = tm.boundingBox;

    // Handle alignment
    switch (va) {
    case TextVA::bottom:
      shifted_pos.y = -bbox.y1;
      break;
    case TextVA::center:
      shifted_pos.y = -bbox.y1 / 2.;
      break;
    case TextVA::top:
      shifted_pos.y = 0;
      break;
    default:
      break;
    }

    switch (ha) {
    case TextHA::left:
      shifted_pos.x = 0;
      break;
    case TextHA::center:
      shifted_pos.x = -bbox.x1 / 2;
      break;
    case TextHA::right:
      shifted_pos.x = -bbox.x1;
      break;
    }

    auto transform = ctx.userMatrix();
    // transform.invert();
    BLPoint pos_px = transform.mapPoint(pos);
    ctx.resetMatrix();
    auto fc = facecolor.value_or(colors.get_color("black"));
    ctx.setFillStyle(fc);
    ctx.fillGlyphRun(pos_px + shifted_pos, font, gb.glyphRun());
  }
};
