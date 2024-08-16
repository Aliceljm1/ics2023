/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
#define blen strlen(buf)
#define bsize sizeof(buf)
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";

void g_num()
{
  int num = rand() % 100;
  if(num == 0)
    return g_num();
  snprintf(buf + blen, sizeof(buf)-blen , "%d", num);
}

void g_left()
{
  buf[blen] = '(';
}


void g_right()
{

  buf[blen]= ')';
}

void g_opt()
{
  char opt = "+-*/"[rand() % 4];
  buf[blen] = opt;
}

void g_space()
{
  int num = rand() % 3;
  memcpy(buf + blen, " ", num);
}

static void gen_rand_expr()
{
  int num = rand() % 3;
  switch (num)
  {
  case 0:
    g_num();
    break;
  case 1:
    g_left();gen_rand_expr();g_right();
    break;
  default:
    gen_rand_expr();
    g_space();g_opt();g_space();
    gen_rand_expr();
    break;
  }
}

int main(int argc, char *argv[])
{
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++)
  {
    memset(buf, 0, bsize);
    gen_rand_expr();
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
