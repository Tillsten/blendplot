
#include "axis.h"
#include "canvas.h"
#include <SDL.h>
#include <array>
#include <vector>

struct RandomWalk {};

int main(int argc, char **argv) {
  auto c = Canvas(800, 800);
  auto ax = Axis(c);
  auto cmap = ColorMapper();

  auto init_lims = ax.view_lim;

  int num_lines = 10;
  vector<shared_ptr<Line>> lines;

  auto arr3 = xt::linspace<double>(0, 20, 600);
  std::vector<std::string> clist = {"red", "blue", "green", "olive", "crimson"};

  for (int i = 0; i < num_lines; i++) {

    lines.emplace_back(make_shared<Line>());
    auto l = lines.at(i).get();
    l->linecolor = colors.get_random_color(); //.get_color(clist[i]);
    l->linewidth = 5;
    std::vector<double> arr;
    std::vector<double> arr2;
    auto y = xt::sin(arr3 * 10) + i;

    arr = std::vector<double>(arr3.begin(), arr3.end());
    arr2 = std::vector<double>(y.begin(), y.end());

    lines.at(i)->set_data(arr, arr2);

    ax.artists.push_back(lines.at(i));
  }
  auto x = std::vector<double>(arr3.begin(), arr3.end());
  auto txt = std::make_shared<Text>(L"BlendPlot", 5, 5);
  txt->ha = TextHA::center;
  ax.artists.push_back(txt);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
  } else {
    auto win = SDL_CreateWindow("SDL2 Demo", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, c.size.w, c.size.h,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    BLImage img;

    int j = 0, k = 0;
    auto last_tick = SDL_GetTicks() / 1000;
    bool running = true;
    while (running) {

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        }

        if (event.type == SDL_WINDOWEVENT) {
          switch (event.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
            SDL_Log("Window %d resized to %dx%d", event.window.windowID,
                    event.window.data1, event.window.data2);
          }
        }
      }

      auto ms = SDL_GetTicks() / 1000.;
      k++;
      if (k % 1000 == 0) {
        int fps = k / (ms - last_tick);
        txt->set_text(to_wstring(fps) + L" fps");
      }
      auto surf2 = SDL_GetWindowSurface(win);
      SDL_LockSurface(surf2);
      img.createFromData(surf2->w, surf2->h, BL_FORMAT_XRGB32, surf2->pixels,
                         surf2->pitch);
      c.resize(img);

      double offset = 0;
      auto l0 = lines.at(4);

      // l0.set_data(vector<double> {5, 5, 5.}, vector<double> {5, 5, 5.});
      for (auto line : lines) {
        auto y = xt::eval(
            0.2 * offset *
                xt::sin(arr3 * 10 + 10 * ms * (num_lines - offset) / 10.0) *
                sin(arr3) +
            1.0 + offset + 0.1);
        // auto y = xt::ones_like(arr3) + offset;
        auto y2 = std::vector<double>(y.begin(), y.end());
        line->set_data(x, y2);
        offset = offset + 1;
      };

      ax.view_lim =
          init_lims + BLPoint(sin(0.00003 + j / 300.), cos(0.003 + j / 300.));
      c.draw();
      SDL_UnlockSurface(surf2);
      SDL_UpdateWindowSurface(win);
      j = j + 1;
    }
    SDL_Quit();
  }
  return 0;
};
