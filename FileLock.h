#ifndef FILE_LOCK_H
#define FILE_LOCK_H

#include <stdexcept>
#include <string>

class FileLockException : public std::logic_error {
public:
  FileLockException(const std::string &what_arg) : std::logic_error(what_arg) {}
};

class FileLock {
public:
  FileLock(std::string lockPath);
  ~FileLock();
  void lock();
  void unlock();
  bool tryLock();

private:
  std::string lockPath_;
  int lockFD_;
};

#endif // FILE_LOCK_H
