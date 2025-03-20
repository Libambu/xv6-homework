#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>

//初始化互斥锁,该锁仅让条件变量使用
static pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
//初始化条件变量
static pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;

#define SOL
#define NBUCKET 5
#define NKEYS 100000

//性能优化
static pthread_mutex_t bucket_mute[NBUCKET];

struct entry {
  int key;
  int value;
  struct entry *next;
};
//这玩意应该是个哈希表
struct entry *table[NBUCKET];

int keys[NKEYS];
int nthread = 1;

volatile int done;
/*
  volatile
    在C语言中，编译器会对代码进行优化，以提高程序的性能。这种优化可能包括重新排序变量的读写操作，或者在认为变量的值不会再被改变时，将变量的值缓存到寄存器中，而不是每次都从内存中读取。
  当一个变量被声明为volatile时，编译器不会对这个变量进行某些优化操作。具体来说：
  防止读写优化：编译器不会假设volatile修饰的变量的值不会改变，因此每次使用这个变量时，都会直接从内存中读取其值，而不会使用可能存在的寄存器中的缓存值。
  防止重排序：编译器不会对volatile变量的读写操作进行重排序，即不会改变volatile变量的读写顺序。

  volatile int done = 0;

  // 线程1
  void thread1() {
    // 执行一些操作
    done = 1; // 标记操作完成
  }

  // 线程2
  void thread2() {
    while (!done) {
        // 等待操作完成
    }
    // 操作完成后的处理
  }
*/

double now()
{
 struct timeval tv;//将当前时间存储到tv中
 gettimeofday(&tv, 0);
 return tv.tv_sec + tv.tv_usec / 1000000.0;
 //将时间中秒部分和微妙部分转换为一个以s为单位的浮点数
}

static void print(void)
{
  int i;
  struct entry *e;
  for (i = 0; i < NBUCKET; i++) {
    printf("%d: ", i);
    for (e = table[i]; e != 0; e = e->next) {
      printf("%d ", e->key);
    }
    printf("\n");
  }
}
//输出哈希表数组里的所有值

static void insert(int key, int value, struct entry **p, struct entry *n)
{
  struct entry *e = malloc(sizeof(struct entry));
  e->key = key;
  e->value = value;
  e->next = n;
  *p = e;
}


static void put(int key, int value)
{
  int i = key % NBUCKET;
  pthread_mutex_lock(&bucket_mute[i]);
  insert(key, value, &table[i], table[i]);
  pthread_mutex_unlock(&bucket_mute[i]);
}
//向哈希表桶中插入数据

static struct entry* get(int key)
{
  int i = key % NBUCKET;
  pthread_mutex_lock(&bucket_mute[i]);
  struct entry *e = 0;
  for (e = table[i]; e != 0; e = e->next) {
    if (e->key == key) break;
  }
  pthread_mutex_unlock(&bucket_mute[i]);
  return e;
}
//通过哈希计算确定键所在的桶，然后在该桶的链表中查找匹配的键，返回对应的节点指针或NULL。

//多线程环境下的哈希表插入和查找操作
static void *thread(void *xa)
{
    long n = (long) xa;
    int i;
    int b = NKEYS/nthread;
    int k = 0;
    double t1, t0;

    //  printf("b = %d\n", b);
    t0 = now();
    for (i = 0; i < b; i++) {
      // printf("%d: put %d\n", n, b*n+i);
      put(keys[b*n + i], n);
    }
    //插入操作中，多个线程可能同时修改同一个桶的链表。
    //如果线程A正在插入一个新节点，而线程B也在同一时刻修改同一个桶的头指针，就可能导致链表结构被破坏。
    t1 = now();
    printf("%ld: put time = %f\n", n, t1-t0);

    // Should use pthread_barrier, but MacOS doesn't support it ...
    //忙等待性能太差
    //__sync_fetch_and_add(&done, 1);：使用原子操作将全局变量done增加1，表示当前线程的插入操作已完成。
    //while (done < nthread) ;忙等待，直到所有线程的插入操作都完成（done达到nthread）
    //使用条件变量
    pthread_mutex_lock(&sync_mutex);
    done++;
    if(done==nthread){
      pthread_cond_broadcast(&sync_cond);
    }else{
      pthread_cond_wait(&sync_cond,&sync_mutex);
    }
    pthread_mutex_unlock(&sync_mutex);

    t0 = now();
    for (i = 0; i < NKEYS; i++) {
      struct entry *e = get(keys[i]);
      if (e == 0) k++;
    }
    t1 = now();
    printf("%ld: get time = %f\n", n, t1-t0);
    printf("%ld: %d keys missing\n", n, k);
    return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  long i;
  double t1, t0;

  //命令行参数>2
  if (argc < 2) {
    fprintf(stderr, "%s: %s nthread\n", argv[0], argv[0]);
    exit(-1);
  }
  //转换为整数
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  //初始化随机数生成器，设置种子为0。这确保了每次运行程序时生成的随机数序列是相同的，便于调试
  srandom(0);
  //确保总键数NKEYS能被线程数nthread整除。如果不是，程序将终止并显示断言失败的错误信息。
  assert(NKEYS % nthread == 0);
  for (i = 0; i < NKEYS; i++) {
    keys[i] = random();
  }
  t0 = now();
  for(i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void *) i) == 0);
  }
  for(i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0);
  }
  t1 = now();
  printf("completion time = %f\n", t1-t0);
}
