#include "HotReload/filewatcher.h"

#include <errno.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <iostream>

FileWatcher::FileWatcher(const char *fpath, std::function<void()> onChange) {
  int fd, wd;
  char buffer[sizeof(inotify_event)];

  // Initialize inotify
  fd = inotify_init();
  if (fd < 0) {
    std::cerr << "inotify_init failed: " << strerror(errno) << std::endl;
    return;
  }

  // wd = inotify_add_watch(fd, fpath, IN_MODIFY | IN_MOVED_FROM);
  wd = inotify_add_watch(fd, fpath, IN_CLOSE_WRITE);
  if (wd < 0) {
    std::cerr << "inotify_add_watch failed: " << strerror(errno) << std::endl;
    close(fd);
    return;
  }

  while (true) {
    int len = read(fd, buffer, sizeof(buffer));
    if (len < 0) {
      std::cerr << "read failed: " << strerror(errno) << std::endl;
      close(fd);
      return;
    }

    // Process events
    for (char *ptr = buffer; ptr < buffer + len;
         ptr += sizeof(struct inotify_event)) {
      const struct inotify_event *event = (const struct inotify_event *)ptr;
      if (event->mask & IN_CLOSE_WRITE) {
        onChange();
      }
    }
  }
}
