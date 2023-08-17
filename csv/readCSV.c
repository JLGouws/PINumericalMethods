#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int tokenize(char ***tok, char *chars, const char * const delim)
{
  char *last, **tok1 = *tok, **tok2 = *tok;
  for(last = chars; chars && *chars != '\0'; chars++)
  {
    if (*chars == *delim)
    {
      int i = 1;
      for(; delim[i] != '\0' && delim[i] == chars[i]; i++);
      if(delim[i] == '\0')
      {
        *chars = '\0';
        chars += i;
        *tok2 = last;
        last = chars;
        tok2++;
      }
    }
  }

  //printf("%d\n", tok2 - tok1);

  return tok2 - tok1;
}

void **readCSV(FILE* stream, const char * const delim, int item_size, void* (*fCaster(char**, void*)))
{
    char line[1024], *tokbuf[1024], **tok = tokbuf;
    void **out, *outBuffer, *item;

    outBuffer = malloc(64 * item_size);

    if(fgets(line, 1024, stream) && tokenize(&tok, line, delim))
    {
      item = fCaster(tok, outBuffer);
    }
    else
      return NULL;


    for(register int i = 0; i < 64 && fgets(line, 1024, stream) 
                               && tokenize(&tok, line, delim); i++)
    {
      item = fCaster(tok, outBuffer++);
    }

    //printf("bye\n");

    return out;
}

typedef struct point
{
  long row, x, y, sigma_y, sigma_x; 
  float rho_xy;
} point_t;

void copyPointVoid(void* source, void* dest)
{
  point_t *p1 = (point_t*) source, *p2 = (point_t*) dest;
  p2->row = p1->row;
  p2->x = p1->x ;
  p2->y= p1->y;
  p2->sigma_x = p1->sigma_x;
  p2->sigma_y = p1->sigma_y;
  p2->rho_xy=  p1->rho_xy;

}

point_t *createPointFromStringsToVoid(char** strings, void *p_in)
{

//  printf("hi: create pointFromStrings\n");
  point_t *p;
  if (p_in)
     p = (point_t*) p_in;
  else
    p = (point_t*) malloc(sizeof(point_t));


  char *end;

  errno = 0;
  long i = strtol(*strings, &end, 10);
  if (*strings == end || errno == ERANGE)
  {
    free(p);
    return NULL;
  }
  p->row = i;

  strings++;

  errno = 0;
  i = strtol(*strings, &end, 10);
  if (*strings == end || errno == ERANGE)
  {
    free(p);
    return NULL;
  }
  p->x = i;

  errno = 0;
  i = strtol(*strings, &end, 10);
  if (*strings == end || errno == ERANGE)
  {
    free(p);
    return NULL;
  }
  p->y = i;

  errno = 0;
  i = strtol(*strings, &end, 10);
  if (*strings == end || errno == ERANGE)
  {
    free(p);
    return NULL;
  }
  p->sigma_y = i;

  errno = 0;
  i = strtol(*strings, &end, 10);
  if (*strings == end || errno == ERANGE)
  {
    free(p);
    return NULL;
  }
  p->sigma_x = i;

  errno = 0;
  const float j = strtof(*strings, &end);
  if (*strings == end || errno == ERANGE)
  {
    free(p);
    return NULL;
  }
  p->rho_xy = j;

  strings++;

  //printf("Row %d: (%ld,%ld) rho_xy: %f\n", p->row, p->x, p->y, p->rho_xy);
}

int main()
{
    const char *delim = ",";
    FILE* stream = fopen("data.csv", "r");
    readCSV(stream, delim, sizeof(point_t), (void*) createPointFromStringsToVoid);
    return 0;
}

