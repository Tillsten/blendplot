#include <array>
#include <iterator>
#include <map>
#include "blend2d.h"
#include "turbo_cmap.h"


using namespace std;

class ColorMapper {
public:
  double min{0};
  double max{1};
  std::array<BLRgba32, 256> lut;
  BLRgba32 under_color{0, 0, 0, 0};
  BLRgba32 over_color{0, 0, 0, 0};

  ColorMapper() { lut = make_turbo(); };

  BLRgba32 get_color(double val) {
    int idx = (int) (255.5 * (val - min) / (max - min));
    if ((idx <= 255) & (0 <= idx)) {
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
  ColorMapper cm = ColorMapper();

  REQUIRE(cm.get_color(-1) == cm.under_color);
  REQUIRE(cm.get_color(0) == cm.lut[0]);
  REQUIRE(cm.get_color(1) == cm.lut[255]);
  REQUIRE(cm.get_color(3) == cm.over_color);
}



class ColorDict {

private:
  std::map<std::string, BLRgba32> dict;
  BLRandom r{1223};

public:
  BLRgba32 get_color(string name) { return BLRgba32(dict.at(name)); };

  BLRgba32 get_random_color(void) {
    auto item = dict.begin();
    auto idx = r.nextUInt32() % dict.size();
    std::advance(item, idx);
    return item->second;
  }

  ColorDict(void) {
    string line;
    ifstream myfile("xkcd_colors.txt");
    if (myfile.is_open()) {
      getline(myfile, line); // skip first line
      while (getline(myfile, line)) {
        cout << line << '\n';
        auto color_pos = line.find("#");
        int val = std::stoi(line.substr(color_pos + 1), nullptr, 16);
        cout << line.substr(color_pos);
        auto name = line.substr(0, color_pos);
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        auto color = BLRgba32(val);
        color.a = 255;
        dict.insert(std::pair(name, color));
        // cout << dict[name].a;
      };
      myfile.close();
    } else
      cout << "Unable to open file";
  };
};