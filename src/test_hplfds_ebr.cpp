#include <iostream>
#include "common/hplfds_memory.h"
#include "EBR/hplfds_ebr.h"
#include <pthread.h>
#include "unistd.h"
using namespace std;
struct MyAllocator
{
  static void free(int *p) { delete p;}
};

int *p = new int(0);

hplfds_sync::HplfdsEBRManager<int, MyAllocator> g_manager;

void* reader(void *arg)
{
  int sum = 0;
  while(true) {
    g_manager.reader_enter();
    sum += ACCESS_ONCE(*p);
    g_manager.reader_leave();
  }
  return NULL;
}

void *writer(void *arg)
{
  while(true) {
    usleep(10000);
    int *q = new int(++(*p));
    int *tmp = p;
    CPU_BARRIER();
    p = q;
    g_manager.writer_del(tmp);
    g_manager.writer_gc();
  }
  return NULL;
}
int main()
{
  pthread_t pt1,pt2,pt3,pt4;
  pthread_create(&pt1,NULL, reader, NULL);
  pthread_create(&pt2,NULL, reader, NULL);
  pthread_create(&pt3,NULL, reader, NULL);
  pthread_create(&pt4,NULL, writer, NULL);
  pthread_join(pt1,NULL);
  pthread_join(pt2,NULL);
  pthread_join(pt3,NULL);
  pthread_join(pt4,NULL);
  return 0;
}
