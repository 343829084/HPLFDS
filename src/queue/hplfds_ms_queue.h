#ifndef HPLFDS_MS_QUEUE
#define HPLFDS_MS_QUEUE
#include "../hazardpointer/hplfds_hazard_pointer.h"
#define ACQUIRE_POINTER(p)\
  HazardPointerRecord record##p(memory_manager_, p, thread_id);

namespace hplfds_sync
{
  template<class T, class MemoryAllocator>
  class HplfdsMSQueue
  {
  private:
    struct QueueCell
    {
      T *p;
      QueueCell *next;
    };
    struct HazardPointerRecord
    {
      HazardPointerRecord(HplfdsHazardPointer<MemoryAllocator> &manager,
                          void *p, int thread_id): manager_(manager)
      {
        p_ = p;
        thread_id_ = thread_id;
        manager.acquire(p_, thread_id_);
      }
      ~HazardPointerRecord()
      {
        manager_.release(p_, thread_id_);
      }
      HplfdsHazardPointer<MemoryAllocator> &manager_;
      void *p_;
      int thread_id_;
    };
  public:
    HplfdsMSQueue();
    int enqueue(T *p, int thread_id);
    int dequeue(T *&p, int thread_id);
    bool empty(int thread_id);
  private:
    void spin();
  private:
    QueueCell *volatile head_;
    QueueCell *volatile tail_;
    QueueCell *dummy_node_;
    HplfdsHazardPointer<MemoryAllocator> memory_manager_;
  };
  template<class T, class MemoryAllocator>
  HplfdsMSQueue<T, MemoryAllocator>::HplfdsMSQueue()
  {
    dummy_node_ = (QueueCell*)(MemoryAllocator::allocate(sizeof(QueueCell)));
    MY_ASSERT(dummy_node_ != NULL);
    dummy_node_->next = NULL;
    dummy_node_->p = NULL;
    head_ = dummy_node_;
    tail_ = dummy_node_;
  }
  template<class T, class MemoryAllocator>
  int HplfdsMSQueue<T, MemoryAllocator>::enqueue(T *p, int thread_id)
  {
    QueueCell *cell = (QueueCell*)(MemoryAllocator::allocate(sizeof(QueueCell)));
    cell->p = p;
    cell->next = NULL;
    QueueCell *volatile tail = NULL;
    while(true) {
      tail = tail_;
      ACQUIRE_POINTER(tail);
      if (tail == tail_) {
        QueueCell *next = tail->next;
        if (next == NULL) {
          if (CAS(&tail->next, next, cell)) {
            CAS(&tail_, tail, cell);
            break;
          } else {
            spin();
          }
        } else {
          CAS(&tail_, tail, next);
        }
      } else {
        spin();
      }
    }
    return SUCCESS;
  }
  template<class T, class MemoryAllocator>
  int HplfdsMSQueue<T, MemoryAllocator>::dequeue(T *&p, int thread_id)
  {
    p = NULL;
    QueueCell *volatile tail = NULL;
    QueueCell *volatile head = NULL;
    QueueCell *next = NULL;
    while(true) {
      tail = tail_;
      ACQUIRE_POINTER(tail);
      if (tail != tail_) {
        continue;
      }
      head = head_;
      ACQUIRE_POINTER(head);
      if (head != head_) {
        continue;
      } else {
        next = head->next;
        if (head == tail) {
          if (next == NULL) {
            return ERROR_EMPTY;
          } else {
            CAS(&tail_, tail, next);
          }
        } else {
          ACQUIRE_POINTER(next);
          if (next != head->next) {
            continue;
          }
          p = next->p;
          if (CAS(&head_, head, next)) {
            break;
          } else {
            spin();
          }
        }
      }
    }
    memory_manager_.retire(head, thread_id);
    memory_manager_.reclaim(thread_id);
    return SUCCESS;
  }
  template<class T, class MemoryAllocator>
  void HplfdsMSQueue<T, MemoryAllocator>::spin()
  {
    static const int64_t INIT_LOOP = 1000000;
    static const int64_t MAX_LOOP = 1000000000;
    static __thread int64_t delay = 0;
    if (delay <= 0) {
      delay = INIT_LOOP;
    }
    for (int64_t i = 0; i < delay; i++) {
      CPU_RELAX();
    }
    int64_t new_delay = delay << 1LL;
    if (new_delay <= 0 || new_delay >= MAX_LOOP) {
      new_delay = INIT_LOOP;
    }
    delay = new_delay;
  }
}
#endif //HPLFDS_MS_QUEUE
