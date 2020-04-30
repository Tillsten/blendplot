#include <blend2d.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>



class Font {
    
public:  
  BLFont font;
  float size{0};
  std::unordered_map<std::wstring, BLPath> cache;
  // string font_name;

  Font() {}

  Font(BLFont font_) {
    font.assign(font_);
    size = font.size();
  }

  BLPath get_path(const std::wstring &txt) {
    BLPath path;
    // TODO: Limit size of cache
    // TODO: Make cache cofigureable
    if (cache.count(txt) == 0) {
      BLGlyphBuffer gb;
      gb.setWCharText(txt.c_str());
      font.shape(gb);
      auto gb_run = gb.glyphRun();
      font.getGlyphRunOutlines(gb_run, path);
      cache.insert_or_assign(txt, path);
      return path;
    } else {
      return cache.at(txt);
    }
  }
};

typedef std::pair<BLFontFace *, int> font_key;
typedef std::pair<int, string> path_key;


class FontManager {
private:
  std::vector<BLFontFace> face_list;
  std::map<string, BLFontFace *> face_cache;
  std::map<font_key, Font> font_cache;

public:
  Font std_font;
  BLFontFace get_face(const string name);
  Font get_font(const string name, const float size);
  // BLPath get_text_path(BLFont, string);

  FontManager() {
    auto face = get_face("C:\\Windows\\Fonts\\calibril.ttf");
    BLFont font;
    font.createFromFace(face, 16);
    std_font = Font(font);
  }
};

BLFontFace FontManager::get_face(const string name) {
  size_t cnt = face_cache.count(name);
  BLFontFace face;
  if (cnt == 0) {
    BLResult ret = face.createFromFile(name.c_str());
    if (ret != BL_SUCCESS) {
      throw;
    };
    face_list.push_back(face);
    face_cache.insert(make_pair(name, &face));
  } else {
    face = *face_cache.at(name);
  };
  return face;
}

Font FontManager::get_font(const string name, const float size) {
  BLFontFace face = get_face(name);
  BLFont font;

  font_key key = {&face, size};
  size_t cnt = font_cache.count(key);
  Font my_font = Font(font);
  if (cnt == 0) {
    BLResult ret = font.createFromFace(face, size);
    if (ret != BL_SUCCESS) {
      throw;
    };
    Font my_font = Font(font);
    font_cache.insert(make_pair(key, my_font));
  } else {
    Font my_font = font_cache.at(key);
  };
  return my_font;
};

/*
BLPath FontManager::get_text_path(BLFont font, string txt) {
    font_key key = { font, txt };
    int cnt = path_cache.count(key);
    BLPath path;
    if (cnt == 0) {
        BLGlyphBuffer gb;
        BLResult ret = font.shape(gb);
        auto gb_run = gb.glyphRun();

        font.getGlyphRunOutlines(gb_run, path);
        if (ret != BL_SUCCESS) {
            throw;
        };
        path_cache.insert_or_assign(key, &path);
    } else {
        path = path_cache.at(key);
    };
    return path;
};    */
