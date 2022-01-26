#pragma once
#include "./common.h"

std::array<BLRgba32, 256> make_turbo() {
  std::array<BLRgba32, 256> out;
#include "turbo_cmap.h"
  for (int i{0}; i < 256; i++) {

    out[i] = BLRgba32((int)turbo_srgb_bytes[i][0], (int)turbo_srgb_bytes[i][1],
                      (int)turbo_srgb_bytes[i][2], 255);
  }
  return out;
}
class ColorMapper {
public:
  float min{0};
  float max{1};
  std::array<BLRgba32, 256> lut = make_turbo();
  BLRgba32 under_color{0, 0, 0, 0};
  BLRgba32 over_color{0, 0, 0, 0};

  BLRgba32 get_color(float val) {
    int idx = (int)(255.5 * (val - min) / (max - min));
    if ((idx <= 255) and (0 <= idx)) {
      return lut[idx];
    } else {
      if (idx > 255) {
        return over_color;
      } else {
        return under_color;
      }
    }
  }
};

TEST_CASE("ColoMapper", "[color]") {
  ColorMapper cm_test = ColorMapper();
  REQUIRE(cm_test.get_color(-1) == cm_test.under_color);
  REQUIRE(cm_test.get_color(0) == cm_test.lut[0]);
  REQUIRE(cm_test.get_color(1) == cm_test.lut[255]);
  REQUIRE(cm_test.get_color(3) == cm_test.over_color);
}

class ColorDict {

private:
  std::map<std::string, BLRgba32> dict;
  BLRandom r{1223};

public:
  BLRgba32 get_color(std::string name) { return BLRgba32(dict.at(name)); };

  BLRgba32 get_random_color(void) {
    auto item = dict.begin();
    auto idx = r.nextUInt32() % dict.size();
    std::advance(item, idx);
    return item->second;
  }

  ColorDict(void) {
    std::string line;
    std::ifstream myfile("xkcd_colors.txt");
    if (myfile.is_open()) {
      std::getline(myfile, line); // skip first line
      while (std::getline(myfile, line)) {
        auto color_pos = line.find("#");
        int val = std::stoi(line.substr(color_pos + 1), nullptr, 16);
        auto name = line.substr(0, color_pos);
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        auto color = BLRgba32(val);
        color.a = 255;
        dict.insert(std::pair(name, color));
        // cout << dict[name].a;
      };
      myfile.close();
    } else
      std::cout << "Unable to open file";
  };
};