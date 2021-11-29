#include <filesystem>
#include <chrono>
#include <stdlib.h>
#include <cstring>
#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <thread>
#include "FileLock.h"

TEST(FileLockTest, ThreadsAcquireLock) {
  char tempDir[] = "/tmp/filelock.XXXXXX";
  ASSERT_TRUE(mkdtemp(tempDir) != NULL) << "Failed creating tempdir:" << strerror(errno);
  using std::filesystem::path;
  auto lockFile = path(tempDir) / "lock";
  LOG(INFO) << "lock file: " << lockFile;

  std::thread t1([lockFile]() {
    FileLock lock(lockFile);
    auto tid = std::this_thread::get_id();
    LOG(INFO) << "thread " << tid << " acquiring file lock...";
    lock.lock();
    LOG(INFO) << "thread " << tid << " accquired file lock, and release now...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    lock.unlock();
  });

  std::thread t2([lockFile]() {
    FileLock lock(lockFile);
    auto tid = std::this_thread::get_id();
    LOG(INFO) << "thread " << tid << " acquiring file lock...";
    lock.lock();
    LOG(INFO) << "thread " << tid << " accquired file lock, and release now...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    lock.unlock();
  });

  t1.join();
  t2.join();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  google::SetStderrLogging(google::INFO);

  return RUN_ALL_TESTS();
}