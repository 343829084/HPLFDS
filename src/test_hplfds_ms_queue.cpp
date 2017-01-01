#include <iostream>
#include "common/hplfds_memory.h"
#include "queue/hplfds_ms_queue.h"
#include <pthread.h>
#include <time.h>
using namespace std;
using namespace hplfds_sync;
HplfdsMSQueue<int, HplfdsMemoryAllocator> queue;
#define NUM 50000000
void* f(void *arg)
{
  int id = *((int*)(arg));
  int ret = 0;
  int j = 0;
  for (int i = 0; i < NUM; i++) {
    ret = queue.enqueue(i, id);
    ret = queue.dequeue(j, id);
    //delete q;
  }
  return NULL;
}
void* enqueuer(void *arg)
{
  int id = *((int*)(arg));
  int ret = 0;
  for (int i = 0; i < NUM; i++) {
    ret = queue.enqueue(i, id);
    //delete p;
  }
  return NULL;
}
void* dequeuer(void *arg)
{
  int id = *((int*)(arg));
  int ret = 0;
  for (int i = 0; i < NUM * 4; i++) {
    ret = queue.dequeue(i, id);
    //delete p;
  }
  return NULL;
}
void test4()
{
  pthread_t t1;
  pthread_t t2;
  pthread_t t3;
  pthread_t t4;
  int id1 = 0;
  int id2 = 1;
  int id3 = 2;
  int id4 = 3;
  timespec ts1, ts2;
  clock_gettime(CLOCK_MONOTONIC, &ts1);
  pthread_create(&t1, NULL, f, &id1);
  pthread_create(&t2, NULL, f, &id2);
  pthread_create(&t3, NULL, f, &id3);
  pthread_create(&t4, NULL, f, &id4);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  int64_t takes = (ts2.tv_sec - ts1.tv_sec) *1000000000LL + (ts2.tv_nsec - ts1.tv_nsec);
  cout<<"test 4 =====>"<<takes<<endl;
}
void test2()
{
  pthread_t t1;
  pthread_t t2;
  int id1 = 0;
  int id2 = 1;
  timespec ts1, ts2;
  clock_gettime(CLOCK_MONOTONIC, &ts1);
  pthread_create(&t1, NULL, f, &id1);
  pthread_create(&t2, NULL, f, &id2);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  int64_t takes = (ts2.tv_sec - ts1.tv_sec) *1000000000LL + (ts2.tv_nsec - ts1.tv_nsec);
  cout<<"test 2 =====>"<<takes<<endl;
}
int main()
{
  srand(time(NULL));
  test2();
  return 0;
}
