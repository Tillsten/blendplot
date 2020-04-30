#pragma once

#define UNICODE
#include <map>
#include <vector>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xrandom.hpp"
#include "xtensor/xview.hpp"

#include "./SimpleSignal.h"

#include <blend2d.h>
#include <functional>
#include <optional>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

typedef std::optional<BLRgba32> oColor;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <fstream>
#include <iostream>


BLMatrix2D make_blended(const BLMatrix2D &x, const BLMatrix2D &y) {
  BLMatrix2D out = x;
  out.m01 = y.m01;
  out.m11 = y.m11;
  out.m21 = y.m21;
  return out;
}

// Global Objects
#include "./colors.h"
#include "./font_manager.h"

static ColorDict colors = ColorDict();
static FontManager font_manager = FontManager();



