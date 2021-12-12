#include "FileLock.h"
#include <chrono>
#include <cstring>
#include <filesystem>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <thread>

using std::filesystem::path;
void lockAndReleaseFileLock(const std::string &lockFile,
                            bool releaseLockAtExit = true) {
  FileLock lock(lockFile);
  auto tid = std::this_thread::get_id();
  LOG(INFO) << "thread " << tid << " acquiring file lock...";
  lock.lock();
  LOG(INFO) << "thread " << tid << " accquired file lock, and release now...";
  std::this_thread::sleep_for(std::chrono::seconds(2));
  if (releaseLockAtExit) {
    lock.unlock();
  }
}

TEST(FileLockTest, ThreadsAcquireLock) {
  char tempDir[] = "/tmp/filelock.XXXXXX";
  ASSERT_TRUE(mkdtemp(tempDir) != NULL)
      << "Failed creating tempdir:" << strerror(errno);
  auto lockFile = path(tempDir) / "lock";
  LOG(INFO) << "lock file: " << lockFile;

  std::thread t1([lockFile]() { lockAndReleaseFileLock(lockFile.string()); });

  std::thread t2([lockFile]() { lockAndReleaseFileLock(lockFile.string()); });

  t1.join();
  t2.join();
}

TEST(FileLockTest, TryLockTest) {
  char tempDir[] = "/tmp/filelock.XXXXXX";
  ASSERT_TRUE(mkdtemp(tempDir) != NULL)
      << "Failed creating tempdir:" << strerror(errno);
  using std::filesystem::path;
  auto lockFile = path(tempDir) / "lock";
  LOG(INFO) << "lock file: " << lockFile;

  FileLock lock1(lockFile);
  lock1.lock();

  FileLock lock2(lockFile);
  ASSERT_FALSE(lock2.tryLock());
}

TEST(FileLockTest, ReleaseAfterDestroy) {
  char tempDir[] = "/tmp/filelock.XXXXXX";
  ASSERT_TRUE(mkdtemp(tempDir) != NULL)
      << "Failed creating tempdir:" << strerror(errno);
  using std::filesystem::path;
  auto lockFile = path(tempDir) / "lock";
  LOG(INFO) << "lock file: " << lockFile;

  std::thread t1([lockFile]() { lockAndReleaseFileLock(lockFile, false); });

  std::thread t2([lockFile]() { lockAndReleaseFileLock(lockFile, false); });

  t1.join();
  t2.join();
}

TEST(FileLockTest, ProcessesAcquireLock) {
  // TODO
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  // address warning: WARNING: Logging before InitGoogleLogging() is written to
  // STDERR
  google::InitGoogleLogging("/tmp/");
  google::SetStderrLogging(google::INFO);

  return RUN_ALL_TESTS();
}
