#ifndef HPLFDS_EPOCH_BASED_RECLA
#define HPLFDS_EPOCH_BASED_RECLA
#include <vector>
#include "../common/hplfds_define.h"
using std::vector;
using std::size_t;
namespace hplfds_sync
{
template<typename T, typename Allocator>
class HplfdsEBRManager
{
private:
  typedef vector<T*> RetireListType;
public:
  HplfdsEBRManager()
  {
    global_epoch_.epoch_ = 0;
    for (int i = 0; i < MAX_THREAD_NUM; ++i) {
      local_epoches_[i].epoch_ = 0;
      active_flags_[i].active_ = false;
    }
    thread_count_ = 0;
  }
  void reader_enter()
  {
    int thread_id = get_thread_id();
    active_flags_[thread_id].active_ = true;
    CPU_BARRIER();
    local_epoches_[thread_id].epoch_ = global_epoch_.epoch_;
    CPU_BARRIER();
  }
  void reader_leave()
  {
    CPU_BARRIER();
    int thread_id = get_thread_id();
    active_flags_[thread_id].active_ = false;
  }
  bool writer_del(T *p)
  {
    RetireListType &retire_list = retire_lists_[global_epoch_.epoch_];
    for (size_t i = 0; i < retire_list.size(); ++i) {
      if (retire_list[i] == NULL) {
        retire_list[i] = p;
        return false;
      }
    }
    retire_list.push_back(p);
    return true;
  }
  bool writer_gc()
  {
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
      if (active_flags_[i].active_
          && local_epoches_[i].epoch_ != global_epoch_.epoch_) {
        writer_free((global_epoch_.epoch_ + 1) % 3);
        return false;
      }
    }
    global_epoch_.epoch_ = (global_epoch_.epoch_ + 1) % 3;
    writer_free((global_epoch_.epoch_ + 1) % 3);
    return true;
  }
private:
  void writer_free(int epoch)
  {
    RetireListType &retire_list = retire_lists_[epoch];
    for (size_t i = 0; i < retire_list.size(); ++i) {
      if (retire_list[i] != NULL) {
        Allocator::free(retire_list[i]);
        retire_list[i] = NULL;
      }
    }
  }
  int get_thread_id()
  {
    static __thread int tid = -1;
    if (tid == -1) {
      tid = FETCH_AND_ADD(&thread_count_, 1);
    }
    return tid;
  }
private:
  struct EpochType
  {
    volatile int epoch_;
  }CACHE_ALIGNED;
  struct ActiveFlagType
  {
    volatile bool active_;
  }CACHE_ALIGNED;
private:
  RetireListType retire_lists_[3];
  EpochType global_epoch_;
  EpochType local_epoches_[MAX_THREAD_NUM];
  ActiveFlagType active_flags_[MAX_THREAD_NUM];
  int thread_count_;
};
}
#endif
