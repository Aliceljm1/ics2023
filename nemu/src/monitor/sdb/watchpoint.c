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

#include "sdb.h"

#define NR_WP 5


/* use static to define the struct's domin and initialize it with '\0' */
static WP wp_pool[NR_WP] = {};   
//static WP *free_head = NULL, *work_head = NULL;
static WP *free_head = NULL, *work_head = NULL;//带头链表，头本身不存储信息

static char break_address[16] = {0};

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;
  /*
   *   wp_pool[0] as the head of the free list
   *   wp_pool[1] as the head of the busy list
   *   so the number of available nodes are (NR_WP - 2)
   */
  free_head = wp_pool; 
  work_head = free_head->next;
  free_head->next = work_head->next;
  work_head->next = NULL;
}


WP* new_wp(char *args){
  WP *tmp = free_head->next;
  bool *success = (bool*)true;
  if(tmp != NULL){
    strcpy(tmp->addr, args);
    tmp->value = expr(args, success);
    free_head->next = tmp->next;
    tmp->next = work_head->next;
    work_head->next = tmp;
  } /* fetch the free watchpoint */
  else{
    printf("no enough free point!\n");
    assert(0);
  } 
  return tmp;
};

void free_wp(int NO){
  WP *match_point = work_head;
  WP *wp = NULL;
  if(NO == match_point->NO){
    wp = match_point->next;
    match_point->next = free_head->next;
    free_head->next = match_point;
    match_point = wp;
    printf("free head watchpoint NO.%d\n",NO);
    return;
  }

  while(match_point->next){
    if(NO == match_point->next->NO){
      printf("free not head watchpoint NO.%d\n",NO);
      wp = match_point->next;
      match_point->next = wp->next;
      wp->next = free_head->next;
      free_head->next = wp;
      return;
    }
    match_point = match_point->next; 
  }
  printf("the watchpoint doesn't exit!\n");
};

void wp_dis(){
  WP *wp = work_head->next;
  while(wp){
    printf("    NO.%d watchpoint : %s\n", wp->NO, wp->addr);
    wp = wp->next;
  }
}

#include "utils.h"
void wp_check(vaddr_t pc, int *nemu_state){
  char *args ;
  int new_value;
  bool *success = (bool*)true;
  WP *wp = work_head;
  while(wp->next){
    wp = wp->next;
    args = wp->addr;
    new_value = expr(args, success);
    if(wp->value != new_value){
      printf("the value of %s changed from 0x%08x to 0x%08x\n", args, wp->value, new_value);
      printf("pc is 0x%x now\n", pc);
      *nemu_state = NEMU_STOP;
      wp->value = new_value;
    }    
  }
}

void new_bp(char *args) {
  strcpy(break_address, args);
}

void bp_check(vaddr_t pc, int *nemu_state) {
  char pc_value[16] = {0};
  snprintf(pc_value, 16, "%x", pc);
  if(strcmp(pc_value, break_address) == 0) {
    printf("stop at address 0x%x now.\n", pc);
    *nemu_state = 1;
  }
}