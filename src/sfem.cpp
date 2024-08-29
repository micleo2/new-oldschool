#include <fstream>
#include <iostream>
#include <iterator>
#include <thread>

#include "6502/InstructionSet/address_space.h"
#include "6502/processor.h"
#include "HotReload/filewatcher.h"
#include "raylib.h"

void draw_loop(const Processor &proc) {
  SetTraceLogLevel(LOG_ERROR);
  auto scaleFac = 16;
  InitWindow(Display::width * scaleFac, Display::height * scaleFac, "[6502]");
  const uint8_t *begin_disp = proc.memory().data() + Regions::DISPLAY.begin;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (int x = 0; x < Display::width; x++) {
      for (int y = 0; y < Display::height; y++) {
        if (Display::read_pix(begin_disp, x, y)) {
          if (x & 1) {
            DrawRectangle(x * scaleFac, y * scaleFac, scaleFac, scaleFac,
                          WHITE);
          } else {
            DrawRectangle(x * scaleFac, y * scaleFac, scaleFac, scaleFac,
                          WHITE);
          }
        } else {
          DrawRectangle(x * scaleFac, y * scaleFac, scaleFac, scaleFac, BLACK);
        }
      }
    }
    EndDrawing();
  }
  CloseWindow();
}

void reload_loop(Processor &proc, const char *fpath) {
  FileWatcher watcher(fpath, [fpath, &proc]() {
    std::ifstream input(fpath, std::ios::binary);
    std::vector<uint8_t> new_mem(std::istreambuf_iterator<char>(input), {});
    assert(new_mem.size() == ADDR_SPACE_SZ &&
           "given executable incorrect size");
    proc.set_reset(new_mem.data());
  });
}

int main(int argc, char *argv[]) {
  char *fpath = argv[1];
  std::ifstream input(fpath, std::ios::binary);
  std::vector<uint8_t> memory(std::istreambuf_iterator<char>(input), {});
  assert(memory.size() == ADDR_SPACE_SZ && "given executable incorrect size");

  Processor proc(memory);

  std::thread renderer(draw_loop, std::cref(proc));
  std::thread reloader(reload_loop, std::ref(proc), fpath);
  proc.run();

  renderer.join();
  reloader.join();
  return 0;
}
