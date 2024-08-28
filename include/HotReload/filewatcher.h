#ifndef HOTRELOAD_FILEWATCHER_H
#define HOTRELOAD_FILEWATCHER_H

#include <functional>

class FileWatcher {
 public:
  FileWatcher(const char* fpath, std::function<void()> onChange);
};

#endif
