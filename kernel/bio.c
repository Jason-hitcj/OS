// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKETS 13

struct {
  struct spinlock lock[NBUCKETS];
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf hashnum[NBUCKETS];
} bcache;

void
binit(void)
{
  struct buf *b;

  for(int i = 0 ; i < NBUCKETS ; i++)
  {
    //为每个哈希桶初始化双向链表
    initlock(&bcache.lock[i], "bcache");

    bcache.hashnum[i].prev = &bcache.hashnum[i];
    bcache.hashnum[i].next = &bcache.hashnum[i];

    for(b = bcache.buf + i; b < bcache.buf + NBUF; b += NBUCKETS){

      b->next = bcache.hashnum[i].next;
      b->prev = &bcache.hashnum[i];
      initsleeplock(&b->lock, "buffer");
      bcache.hashnum[i].next->prev = b;
      bcache.hashnum[i].next = b;
    }
  }


  
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int hash = blockno % NBUCKETS;
  
  acquire(&bcache.lock[hash]);
  // Is the block already cached?
  for(b = bcache.hashnum[hash].next; b != &bcache.hashnum[hash]; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lock[hash]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  // 未命中，先在自身查找空闲块
  for(b = bcache.hashnum[hash].prev; b != &bcache.hashnum[hash]; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock[hash]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  //未找到，释放锁
  release(&bcache.lock[hash]);

  //遍历其他桶，从其他桶查找空闲块
  for(int i = 0 ;i < NBUCKETS;i++){
    if(i == hash) 
      continue; //跳过自身（已经找过，没必要再找）
    if(bcache.lock[i].locked)
      continue; //跳过锁住的桶，避免死锁
    acquire(&bcache.lock[i]); //为没上锁的桶加锁
    for(b = bcache.hashnum[i].prev; b != &bcache.hashnum[i]; b = b->prev){
      if(b->refcnt == 0) {
        
        //将b从原来的哈希桶删除
        b->next->prev = b->prev;
        b->prev->next = b->next;
        //加入目前的哈希桶
        b->next = bcache.hashnum[hash].next;
        b->prev = &bcache.hashnum[hash];
        bcache.hashnum[hash].next->prev = b;
        bcache.hashnum[hash].next = b;
        //修改参数
        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;
        //释放锁
        release(&bcache.lock[i]);
        // release(&bcache.lock[hash]);
        acquiresleep(&b->lock);
        return b;
      }
      
    }
    release(&bcache.lock[i]);
  }
  // release(&bcache.lock[hash]);
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);
  int hash = b->blockno % NBUCKETS;
  acquire(&bcache.lock[hash]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.hashnum[hash].next;
    b->prev = &bcache.hashnum[hash];
    bcache.hashnum[hash].next->prev = b;
    bcache.hashnum[hash].next = b;
  }
  
  release(&bcache.lock[hash]);
}

void
bpin(struct buf *b) {
  int hash = b->blockno % NBUCKETS;
  acquire(&bcache.lock[hash]);
  b->refcnt++;
  release(&bcache.lock[hash]);
}

void
bunpin(struct buf *b) {
  int hash = b->blockno % NBUCKETS;
  acquire(&bcache.lock[hash]);
  b->refcnt--;
  release(&bcache.lock[hash]);
}


