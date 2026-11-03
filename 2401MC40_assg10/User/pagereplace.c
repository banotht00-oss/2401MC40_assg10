#include "types.h"
#include "user.h"

#define MAXFRAMES 5
#define NREFS 20

static int references[NREFS] = {
  7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
  0, 3, 2, 1, 2, 0, 1, 7, 0, 1
};

enum algorithm { FIFO, LRU, OPTIMAL };

struct result {
  int faults;
  int hits;
};

static char *names[] = { "FIFO", "LRU", "Optimal" };

static void
printframes(int *frames, int nframes)
{
  int j;

  printf(1, "[");
  for(j = 0; j < nframes; j++){
    if(j)
      printf(1, " ");
    if(frames[j] < 0)
      printf(1, "-");
    else
      printf(1, "%d", frames[j]);
  }
  printf(1, "]");
}

static int
victimfor(enum algorithm algorithm, int *frames, int *lastused,
          int nframes, int nextfifo, int refindex)
{
  int j, k, next, farthest, victim;

  if(algorithm == FIFO)
    return nextfifo;
  if(algorithm == LRU){
    victim = 0;
    for(j = 1; j < nframes; j++)
      if(lastused[j] < lastused[victim])
        victim = j;
    return victim;
  }

  victim = 0;
  farthest = -1;
  for(j = 0; j < nframes; j++){
    next = NREFS;
    for(k = refindex + 1; k < NREFS; k++)
      if(references[k] == frames[j]){
        next = k;
        break;
      }
    if(next > farthest){
      farthest = next;
      victim = j;
    }
  }
  return victim;
}

static struct result
simulate(enum algorithm algorithm, int nframes, int verbose)
{
  int frames[MAXFRAMES], lastused[MAXFRAMES];
  int i, j, found, victim, nextfifo;
  struct result result;

  for(i = 0; i < nframes; i++){
    frames[i] = -1;
    lastused[i] = -1;
  }
  nextfifo = 0;
  result.faults = 0;
  result.hits = 0;

  if(verbose)
    printf(1, "\n%s, %d frames:\n", names[algorithm], nframes);
  for(i = 0; i < NREFS; i++){
    found = -1;
    for(j = 0; j < nframes; j++)
      if(frames[j] == references[i]){
        found = j;
        break;
      }

    if(found >= 0){
      result.hits++;
      lastused[found] = i;
      if(verbose){
        printf(1, "%d ", references[i]);
        printframes(frames, nframes);
        printf(1, " Hit\n");
      }
      continue;
    }

    result.faults++;
    victim = -1;
    for(j = 0; j < nframes; j++)
      if(frames[j] < 0){
        victim = j;
        break;
      }
    if(victim < 0)
      victim = victimfor(algorithm, frames, lastused, nframes,
                        nextfifo, i);
    frames[victim] = references[i];
    lastused[victim] = i;
    if(algorithm == FIFO)
      nextfifo = (victim + 1) % nframes;
    if(verbose){
      printf(1, "%d ", references[i]);
      printframes(frames, nframes);
      printf(1, " Fault\n");
    }
  }
  return result;
}

int
main(void)
{
  int nframes, algorithm;
  struct result results[3][3];

  printf(1, "Reference string: "
            "7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1\n");
  for(nframes = 3; nframes <= 5; nframes++)
    for(algorithm = FIFO; algorithm <= OPTIMAL; algorithm++)
      results[nframes - 3][algorithm] =
        simulate(algorithm, nframes, 1);

  printf(1, "\nComparison (20 references):\n");
  printf(1, "Frames Algorithm Faults Hits Hit ratio Fault ratio\n");
  for(nframes = 3; nframes <= 5; nframes++)
    for(algorithm = FIFO; algorithm <= OPTIMAL; algorithm++){
      struct result r = results[nframes - 3][algorithm];
      printf(1, "%d      %s        %d      %d    %d%%       %d%%\n",
             nframes, names[algorithm], r.faults, r.hits,
             r.hits * 100 / NREFS, r.faults * 100 / NREFS);
    }
  exit();
}
