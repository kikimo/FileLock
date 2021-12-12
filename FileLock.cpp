#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "FileLock.h"

FileLock::FileLock(std::string lockPath) : lockPath_(lockPath) {
  int fd = open(this->lockPath_.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
  if (fd < 0) {
    std::string what = "Failed openning file lock " + this->lockPath_ + ": " +
                       std::strerror(errno);
    throw FileLockException(what);
  }

  this->lockFD_ = fd;
}

FileLock::~FileLock() {
  if (this->lockFD_ >= 0) {
    if (close(this->lockFD_) < 0) {
      // TODO log error
    }
  }
}

void FileLock::lock() {
  if (flock(this->lockFD_, LOCK_EX) != 0) {
    std::string what = "Failed acquiring file lock " + this->lockPath_ + ": " +
                       std::strerror(errno);
    throw FileLockException(what);
  }
  // log std::cout << "lock acuireed\n";
}

bool FileLock::tryLock() {
  int ret = flock(this->lockFD_, LOCK_EX | LOCK_NB);
  if (ret == 0) {
    return true;
  }

  if (errno == EWOULDBLOCK) {
    return false;
  }

  std::string what = "Failed try acquiring file lock " + this->lockPath_ +
                     ": " + std::strerror(errno);
  throw FileLockException(what);
}

void FileLock::unlock() {
  if (flock(this->lockFD_, LOCK_UN) != 0) {
    std::string what = "Failed releasing file lock " + this->lockPath_ + ": " +
                       std::strerror(errno);
    throw FileLockException(what);
  }
  // log std::cout << "file unlocked\n";
}
