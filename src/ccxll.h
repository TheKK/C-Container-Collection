#ifndef _CC_XLL_H_
#define _CC_XLL_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "mem.h"
#include "misc.h"


/* ccxll create */


#define _S_CCXLL        ADDID(_CCXLL_)
#define _S_CCXLL_NODE   ADDID(_CCXLL_NODE_ )
#define _S_CCXLL_BLOCK  ADDID(_CCXLL_BLOCK_)
#define _S_CCXLL_ITER   ADDID(_CCXLL_ITER_ )

#define ccxll(elem_t)                                                          \
                                                                               \
        ccxll_extd(elem_t, 0, NORMAL)

#define ccxll_pckd(elem_t)                                                     \
                                                                               \
        ccxll_extd(elem_t, 0, PACKED)

#define ccxll_extd(elem_t, n_iter, _ALIGN_)                                    \
                                                                               \
    struct _S_CCXLL                                                            \
    {                                                                          \
        int size,  used,  vcnt;                   /* size and node record */   \
        int start, ratio, thrsh;                  /* block increment info */   \
                                                                               \
        struct _S_CCXLL_NODE                                                   \
        {   link_t lnk;                                                        \
            elem_t val;                           /* val with an xor link */   \
        }   *avsp, *pnode;                        /* available space list */   \
                                                                               \
        union                                                                  \
        {   link_t lnk;                                                        \
            struct _S_CCXLL_NODE *stnl;           /* points to same addr. */   \
        }   head, tail;                           /* two pseudo sentinels */   \
                                                                               \
        struct _S_CCXLL_BLOCK                                                  \
        {   struct _S_CCXLL_BLOCK *next;          /* points to next block */   \
            PRAGMA_##_ALIGN_##_BGN                /* packed pragma starts */   \
            struct _S_CCXLL_NODE nodes[1];        /* node structure array */   \
            PRAGMA_##_ALIGN_##_END                /* the pragma ends here */   \
        }   *pool, *pblock;                       /* points to 1-st block */   \
                                                                               \
        struct _S_CCXLL_ITER                                                   \
        {   union                                                              \
            {   link_t *lnk;                                                   \
                struct _S_CCXLL_NODE *pnode;                                   \
            }   prev, curr, next;                 /* adjacent ptr to node */   \
            struct _S_CCXLL *pccxll;              /* points to ccxll body */   \
        }   **_it, it[(n_iter) + 1];                                           \
                                                                               \
        struct _S_CCXLL **_xl;                    /* internal use _it/_xl */   \
    }                                                                          \



/* ccxll initialize */


#define ccxll_init(_ccxll)                                                     \
                                                                               \
        ccxll_init_extd(_ccxll, 1 << 4, 1 << 1, 1 << 16)

#define ccxll_init_from(_ccxll_dst, _ccxll_src)                                \
                                                                               \
        ccxll_init_extd(_ccxll_dst,        (_ccxll_src).start,                 \
                       (_ccxll_src).ratio, (_ccxll_src).thrsh)

#define ccxll_init_extd(_ccxll, _start, _ratio, _thrsh)                        \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    _ccxll_init_info((_ccxll), (_start), (_ratio), (_thrsh)),                  \
    _ccxll_init_core((_ccxll)),                                                \
     ccxll_iter_init((_ccxll).it[0], (_ccxll))                                 \
)


#define ccxll_init_full(_ccxll)                                                \
                                                                               \
        ccxll_init_full_extd(_ccxll, 1 << 4, 1 << 1, 1 << 16)

#define ccxll_init_full_extd(_ccxll, _start, _ratio, _thrsh)                   \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    ccxll_init_extd((_ccxll), (_start), (_ratio), (_thrsh));                   \
                                                                               \
    for (int _idx = 1; _idx < ELEMOF_ARR((_ccxll).it); _idx++)                 \
        ccxll_iter_init((_ccxll).it[_idx], (_ccxll));                          \
)


#define _ccxll_init_info(_ccxll, _start, _ratio, _thrsh)                       \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_ccxll).start = ((_start) >= 1) ? (_start) : 1,                           \
    (_ccxll).ratio = ((_ratio) >= 1) ? (_ratio) : 1,                           \
    (_ccxll).thrsh = ((_thrsh) >= (_ccxll).start) ? (_thrsh) : (_ccxll).start  \
)


#define _ccxll_init_core(_ccxll)                                               \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    _ccxll_init_base((_ccxll)),                                                \
    (_ccxll).used =  (_ccxll).vcnt   =  0,                                     \
    (_ccxll).avsp =  (_ccxll).pnode  =  NULL,                                  \
    (_ccxll).pool =  (_ccxll).pblock =  NULL,                                  \
    (_ccxll)._it  =  NULL,                                                     \
    (_ccxll)._xl  =  NULL                                                      \
)


#define _ccxll_init_base(_ccxll)                                               \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_ccxll).size = 0,                                                         \
    (_ccxll).head.lnk = &((_ccxll).tail),                                      \
    (_ccxll).tail.lnk = &((_ccxll).head)                                       \
)


#define ccxll_iter_init(_iter, _ccxll)                                         \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter).prev.lnk = NULL,                                                   \
    (_iter).curr.lnk = NULL,                                                   \
    (_iter).next.lnk = NULL,                                                   \
    (_iter).pccxll = &(_ccxll)                                                 \
)



/* ccxll destroy */


#define ccxll_free(_ccxll)                                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    _block_free((_ccxll));                                                     \
    _ccxll_init_core((_ccxll));                                                \
)



/* exclusive or */


#define XOR_2(_addr_a, _addr_b)                                                \
(                                                                              \
    (void*)( (uintptr_t)(void*)(_addr_a) ^                                     \
             (uintptr_t)(void*)(_addr_b) )                                     \
)


#define XOR_3(_addr_a, _addr_b, _addr_c)                                       \
(                                                                              \
    (void*)( (uintptr_t)(void*)(_addr_a) ^                                     \
             (uintptr_t)(void*)(_addr_b) ^                                     \
             (uintptr_t)(void*)(_addr_c) )                                     \
)


/* ccxll access */


#define ccxll_front(_ccxll)  ((_ccxll).head.stnl->val)

#define ccxll_back(_ccxll)   ((_ccxll).tail.stnl->val)



/* ccxll capacity */


#define ccxll_size(_ccxll)   ((_ccxll).size)

#define ccxll_empty(_ccxll)  (!(ccxll_size((_ccxll))))



/* ccxll modifiers */


#define  ccxll_push_front(_ccxll, _val)  _ccxll_push(_ccxll, _val, head)

#define  ccxll_push_back(_ccxll, _val)   _ccxll_push(_ccxll, _val, tail)

#define _ccxll_push(_ccxll, _val, _hdtl_)                                      \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    _node_alloc((_ccxll).pnode, (_ccxll));                                     \
                                                                               \
    (_ccxll).pnode->val = (_val);                                              \
                                                                               \
    (_ccxll).pnode->lnk       = XOR_2(&((_ccxll)._hdtl_.lnk),                  \
                                        (_ccxll)._hdtl_.lnk);                  \
                                                                               \
    (_ccxll)._hdtl_.stnl->lnk = XOR_3(&((_ccxll)._hdtl_.lnk),                  \
                                        (_ccxll)._hdtl_.stnl->lnk,             \
                                      &((_ccxll).pnode->lnk));                 \
                                                                               \
    (_ccxll)._hdtl_.lnk = &((_ccxll).pnode->lnk);                              \
                                                                               \
    (_ccxll).size++;                                                           \
)


#define  ccxll_pop_front(_ccxll)  _ccxll_pop(_ccxll, head)

#define  ccxll_pop_back(_ccxll)   _ccxll_pop(_ccxll, tail)

#define _ccxll_pop(_ccxll, _hdtl_)                                             \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccxll_empty((_ccxll)))  break;                                         \
                                                                               \
    (_ccxll).pnode = (_ccxll)._hdtl_.stnl;                                     \
                                                                               \
    (_ccxll)._hdtl_.lnk       = XOR_2(&((_ccxll)._hdtl_.lnk),                  \
                                        (_ccxll)._hdtl_.stnl->lnk);            \
                                                                               \
    (_ccxll)._hdtl_.stnl->lnk = XOR_3(&((_ccxll)._hdtl_.lnk),                  \
                                        (_ccxll)._hdtl_.stnl->lnk,             \
                                      &((_ccxll).pnode->lnk));                 \
                                                                               \
    _node_clear((_ccxll).pnode, (_ccxll));                                     \
                                                                               \
    (_ccxll).size--;                                                           \
)


#define ccxll_insert(_iter, _val)                                              \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccxll_iter_at_head(_iter))  break;                                     \
                                                                               \
    _node_alloc((_iter).pccxll->pnode, *(_iter).pccxll);                       \
                                                                               \
    (_iter).pccxll->pnode->val = (_val);                                       \
                                                                               \
    (_iter).next.pnode = (_iter).curr.pnode;                                   \
    (_iter).curr.pnode = (_iter).pccxll->pnode;                                \
                                                                               \
    (_iter).curr.pnode->lnk = XOR_2(&((_iter).prev.pnode->lnk),                \
                                    &((_iter).next.pnode->lnk));               \
    (_iter).prev.pnode->lnk = XOR_3(  (_iter).prev.pnode->lnk ,                \
                                    &((_iter).next.pnode->lnk),                \
                                    &((_iter).pccxll->pnode->lnk));            \
    (_iter).next.pnode->lnk = XOR_3(  (_iter).next.pnode->lnk ,                \
                                    &((_iter).prev.pnode->lnk),                \
                                    &((_iter).pccxll->pnode->lnk));            \
    (_iter).pccxll->size++;                                                    \
)


#define ccxll_erase(_iter)                                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccxll_iter_at_head(_iter) || ccxll_iter_at_tail(_iter))  break;        \
                                                                               \
    (_iter).prev.pnode->lnk = XOR_3(  (_iter).prev.pnode->lnk ,                \
                                    &((_iter).next.pnode->lnk),                \
                                    &((_iter).curr.pnode->lnk));               \
    (_iter).next.pnode->lnk = XOR_3(  (_iter).next.pnode->lnk ,                \
                                    &((_iter).prev.pnode->lnk),                \
                                    &((_iter).curr.pnode->lnk));               \
                                                                               \
    _node_clear((_iter).curr.pnode, *(_iter).pccxll);                          \
                                                                               \
    (_iter).curr.lnk =         (_iter).next.lnk;                               \
    (_iter).next.lnk = XOR_2(  (_iter).curr.pnode->lnk,                        \
                             &((_iter).prev.pnode->lnk));                      \
    (_iter).pccxll->size--;                                                    \
)


#define ccxll_swap(_ccxll_u, _ccxll_l)                                         \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _base_w;                                                               \
    _xl_alloc((_ccxll_u), 1, &_base_w);                                        \
                                                                               \
    *(_ccxll_u)._xl[_base_w] = (_ccxll_u);                                     \
                                                                               \
    (_ccxll_u)._xl[_base_w]->_xl = (_ccxll_l)._xl;                             \
    (_ccxll_l)._xl = (_ccxll_u)._xl;                                           \
                                                                               \
    (_ccxll_u) =  (_ccxll_l);                                                  \
    (_ccxll_l) = *(_ccxll_u)._xl[_base_w];                                     \
                                                                               \
    link_t _ln_hd = XOR_2(&((_ccxll_u).head.lnk), &((_ccxll_l).head.lnk));     \
    link_t _ln_tl = XOR_2(&((_ccxll_u).tail.lnk), &((_ccxll_l).tail.lnk));     \
                                                                               \
    (_ccxll_u).head.stnl->lnk = XOR_2((_ccxll_u).head.stnl->lnk, _ln_hd);      \
    (_ccxll_u).tail.stnl->lnk = XOR_2((_ccxll_u).tail.stnl->lnk, _ln_tl);      \
    (_ccxll_l).head.stnl->lnk = XOR_2((_ccxll_l).head.stnl->lnk, _ln_hd);      \
    (_ccxll_l).tail.stnl->lnk = XOR_2((_ccxll_l).tail.stnl->lnk, _ln_tl);      \
                                                                               \
    _xl_free((_ccxll_u), 1);                                                   \
)


#define ccxll_resize(_ccxll, _items, _val)                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _r = ccxll_size((_ccxll)) - (_items);                                  \
                                                                               \
    if (_r > 0)       {  while(_r--)  ccxll_pop_back ((_ccxll));          }    \
    else if (_r < 0)  {  while(_r++)  ccxll_push_back((_ccxll), (_val));  }    \
)


#define ccxll_clear(_ccxll)                                                    \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    while (!(ccxll_empty((_ccxll))))  {  ccxll_pop_back((_ccxll));  }          \
)



/* ccxll operations */


#define ccxll_move_range(_iter_p, _iter_l, _iter_r)                            \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    link_t *_p_c = &((_iter_p).curr.pnode->lnk);                               \
    link_t *_l_c = &((_iter_l).curr.pnode->lnk);                               \
    link_t *_r_c = &((_iter_r).curr.pnode->lnk);                               \
    link_t *_p_p = &((_iter_p).prev.pnode->lnk);                               \
    link_t *_l_p = &((_iter_l).prev.pnode->lnk);                               \
    link_t *_r_p = &((_iter_r).prev.pnode->lnk);                               \
                                                                               \
    (_iter_p).prev.pnode->lnk = XOR_3((_iter_p).prev.pnode->lnk, _p_c, _l_c);  \
    (_iter_l).prev.pnode->lnk = XOR_3((_iter_l).prev.pnode->lnk, _l_c, _r_c);  \
    (_iter_r).prev.pnode->lnk = XOR_3((_iter_r).prev.pnode->lnk, _r_c, _p_c);  \
                                                                               \
    (_iter_p).curr.pnode->lnk = XOR_3((_iter_p).curr.pnode->lnk, _p_p, _r_p);  \
    (_iter_r).curr.pnode->lnk = XOR_3((_iter_r).curr.pnode->lnk, _r_p, _l_p);  \
    (_iter_l).curr.pnode->lnk = XOR_3((_iter_l).curr.pnode->lnk, _l_p, _p_p);  \
                                                                               \
    (_iter_p).next.lnk = (  (_iter_p).next.lnk != _l_c) ?                      \
                         (&((_iter_p).next.pnode->lnk)) : (_r_c);              \
    (_iter_l).next.lnk = (  (_iter_l).next.lnk != _r_c) ?                      \
                         (&((_iter_l).next.pnode->lnk)) : (_p_c);              \
    (_iter_r).next.lnk = (  (_iter_r).next.lnk != _p_c) ?                      \
                         (&((_iter_r).next.pnode->lnk)) : (_l_c);              \
                                                                               \
    (_iter_p).prev.lnk = XOR_2((_iter_p).curr.pnode->lnk, (_iter_p).next.lnk); \
    (_iter_l).prev.lnk = XOR_2((_iter_l).curr.pnode->lnk, (_iter_l).next.lnk); \
    (_iter_r).prev.lnk = XOR_2((_iter_r).curr.pnode->lnk, (_iter_r).next.lnk); \
)


#define ccxll_merge_range(_iter_l, _iter_m, _iter_r, _iter_x)                  \
                                                                               \
        ccxll_merge_range_extd(_iter_l, _iter_m, _iter_r, _iter_x, XLEQ)

#define ccxll_merge_range_extd(_iter_l, _iter_m, _iter_r, _iter_x, _leq)       \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccxll_iter_at_tail(_iter_m))  break;                                   \
                                                                               \
    ccxll_iter_copy((_iter_x), (_iter_m));                                     \
                                                                               \
    while (1)                                                                  \
    {                                                                          \
        while ((_iter_l).curr.lnk != (_iter_m).curr.lnk &&                     \
               _leq((_iter_l), (_iter_m)))                                     \
            ccxll_iter_incr((_iter_l));                                        \
                                                                               \
        if ((_iter_l).curr.lnk == (_iter_m).curr.lnk)                          \
        {                                                                      \
            ccxll_iter_copy((_iter_l), (_iter_r));                             \
            ccxll_iter_copy((_iter_m), (_iter_r));  break;                     \
        }                                                                      \
                                                                               \
        ccxll_iter_incr((_iter_x));                                            \
                                                                               \
        while ((_iter_x).curr.lnk != (_iter_r).curr.lnk &&                     \
               _leq((_iter_x), (_iter_l)))                                     \
            ccxll_iter_incr((_iter_x));                                        \
                                                                               \
        ccxll_move_range((_iter_l), (_iter_m), (_iter_x));                     \
        ccxll_iter_copy ((_iter_m), (_iter_x));                                \
                                                                               \
        if ((_iter_x).curr.lnk == (_iter_r).curr.lnk)                          \
        {                                                                      \
            ccxll_iter_copy((_iter_l), (_iter_x));                             \
            ccxll_iter_copy((_iter_r), (_iter_x));  break;                     \
        }                                                                      \
    }                                                                          \
)


#define ccxll_sort(_ccxll)                                                     \
                                                                               \
        ccxll_sort_extd(_ccxll,  1, XLEQ)

#define ccxll_sort_extd(_ccxll, _g, _leq)    /* TODO : Optimization */         \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    if (ccxll_empty(_ccxll))  break;                                           \
                                                                               \
    int _base_s;                                                               \
    _it_alloc((_ccxll), 4, &_base_s);                                          \
                                                                               \
    for (int _cmg = 0, _gap = (_g); _cmg != 1 && !(_cmg = 0); _gap <<= 1)      \
    {                                                                          \
        ccxll_iter_begin(*(_ccxll)._it[_base_s + 0], (_ccxll));                \
        ccxll_iter_begin(*(_ccxll)._it[_base_s + 1], (_ccxll));                \
        ccxll_iter_begin(*(_ccxll)._it[_base_s + 2], (_ccxll));                \
                                                                               \
        while (!(ccxll_iter_at_tail(*(_ccxll)._it[_base_s + 1])) && ++_cmg)    \
        {                                                                      \
            ccxll_iter_advance(*(_ccxll)._it[_base_s + 1], _gap);              \
            ccxll_iter_copy   (*(_ccxll)._it[_base_s + 2],                     \
                               *(_ccxll)._it[_base_s + 1]);                    \
            ccxll_iter_advance(*(_ccxll)._it[_base_s + 2], _gap);              \
                                                                               \
            ccxll_merge_range_extd(*(_ccxll)._it[_base_s + 0],                 \
                                   *(_ccxll)._it[_base_s + 1],                 \
                                   *(_ccxll)._it[_base_s + 2],                 \
                                   *(_ccxll)._it[_base_s + 3], _leq);          \
        }                                                                      \
    }                                                                          \
                                                                               \
    _it_free((_ccxll), 4);                                                     \
)


#define ccxll_reverse_range(_iter_l, _iter_r)                                  \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    link_t *_l_c = &((_iter_l).curr.pnode->lnk);                               \
    link_t *_r_c = &((_iter_r).curr.pnode->lnk);                               \
    link_t *_l_p = &((_iter_l).prev.pnode->lnk);                               \
    link_t *_r_p = &((_iter_r).prev.pnode->lnk);                               \
    link_t *_l_n = &((_iter_l).next.pnode->lnk);                               \
                                                                               \
    (_iter_l).next.pnode->lnk = XOR_3((_iter_l).next.pnode->lnk, _l_c, _r_c);  \
    (_iter_r).prev.pnode->lnk = XOR_3((_iter_r).prev.pnode->lnk, _r_c, _l_c);  \
                                                                               \
    (_iter_l).curr.pnode->lnk = XOR_3((_iter_l).curr.pnode->lnk, _l_n, _r_p);  \
    (_iter_r).curr.pnode->lnk = XOR_3((_iter_r).curr.pnode->lnk, _l_n, _r_p);  \
                                                                               \
    (_iter_l).next.lnk = XOR_2(_l_p, _l_c);                                    \
    (_iter_r).next.lnk = XOR_2(_r_p, _r_c);                                    \
)



/* default comparators */


#define XLEQ  CCXLL_LEQ_COMPAR

#define CCXLL_LEQ_COMPAR(_iter_a, _iter_b)                                     \
(                                                                              \
    dref((_iter_a)) <= dref((_iter_b))                                         \
)



/* ccxll iterators */


#define dref       ccxll_iter_dref
#define dref_prev  ccxll_iter_dref_prev
#define dref_next  ccxll_iter_dref_next

#define ccxll_iter_dref(_iter)       ((_iter).curr.pnode->val)

#define ccxll_iter_dref_prev(_iter)  ((_iter).prev.pnode->val)

#define ccxll_iter_dref_next(_iter)  ((_iter).next.pnode->val)


#define ccxll_iter_copy(_iter_dst, _iter_src)                                  \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter_dst) = (_iter_src)                                                  \
)


#define ccxll_iter_head(_iter, _ccxll)                                         \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter).prev.lnk = NULL,                                                   \
    (_iter).curr.lnk = &((_ccxll).head.lnk),                                   \
    (_iter).next.lnk = &((_ccxll).head.stnl->lnk)                              \
)


#define ccxll_iter_tail(_iter, _ccxll)                                         \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter).next.lnk = NULL,                                                   \
    (_iter).curr.lnk = &((_ccxll).tail.lnk),                                   \
    (_iter).prev.lnk = &((_ccxll).tail.stnl->lnk)                              \
)


#define ccxll_iter_begin(_iter, _ccxll)                                        \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter).prev.lnk =       &((_ccxll).head.lnk),                             \
    (_iter).curr.lnk =       &((_ccxll).head.stnl->lnk),                       \
    (_iter).next.lnk = XOR_2(&((_ccxll).head.lnk), (_ccxll).head.stnl->lnk)    \
)


#define ccxll_iter_end(_iter, _ccxll)                                          \
                                                                               \
VOID_EXPR_                                                                     \
(                                                                              \
    (_iter).next.lnk =       &((_ccxll).tail.lnk),                             \
    (_iter).curr.lnk =       &((_ccxll).tail.stnl->lnk),                       \
    (_iter).prev.lnk = XOR_2(&((_ccxll).tail.lnk), (_ccxll).tail.stnl->lnk)    \
)


#define ccxll_iter_at_head(_iter)           ( (_iter).prev.lnk  == NULL )

#define ccxll_iter_at_tail(_iter)           ( (_iter).next.lnk  == NULL )

#define ccxll_iter_at_begin(_iter, _ccxll)  ( (_iter).curr.node ==             \
                                             (_ccxll).head.stnl )

#define ccxll_iter_at_end(_iter, _ccxll)    ( (_iter).curr.node ==             \
                                             (_ccxll).tail.stnl )


#define ccxll_iter_incr(_iter)                                                 \
(                                                                              \
    (ccxll_iter_at_tail(_iter)) ? (NULL) :                                     \
    (                                                                          \
        (_iter).prev.lnk =       (_iter).curr.lnk,                             \
        (_iter).curr.lnk =       (_iter).next.lnk,                             \
        (_iter).next.lnk = XOR_2((_iter).prev.lnk, (_iter).curr.pnode->lnk)    \
    )                                                                          \
)


#define ccxll_iter_decr(_iter)                                                 \
(                                                                              \
    (ccxll_iter_at_head(_iter)) ? (NULL) :                                     \
    (                                                                          \
        (_iter).next.lnk =       (_iter).curr.lnk,                             \
        (_iter).curr.lnk =       (_iter).prev.lnk,                             \
        (_iter).prev.lnk = XOR_2((_iter).next.lnk, (_iter).curr.pnode->lnk)    \
    )                                                                          \
)


#define ccxll_iter_advance(_iter, _diff)                                       \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _a = (_diff);                                                          \
                                                                               \
    if (_a > 0)       {  while (ccxll_iter_incr((_iter)) && --_a);  }          \
    else if (_a < 0)  {  while (ccxll_iter_decr((_iter)) && ++_a);  }          \
)



/* ccxll traversal */


#define  CCXLL_INCR(_ccxll, _th_iter)                                          \
                                                                               \
        _CCXLL_INCR((_ccxll), (_ccxll).it[(_th_iter)])

#define _CCXLL_INCR(_ccxll, _iter)                                             \
                                                                               \
    for (ccxll_iter_head((_iter), (_ccxll)); ccxll_iter_incr((_iter)); )


#define  CCXLL_DECR(_ccxll, _th_iter)                                          \
                                                                               \
        _CCXLL_DECR((_ccxll), (_ccxll).it[(_th_iter)])

#define _CCXLL_DECR(_ccxll, _iter)                                             \
                                                                               \
    for (ccxll_iter_tail((_iter), (_ccxll)); ccxll_iter_decr((_iter)); )



/* ccxll extensions */


#define ccxll_copy(_ccxll_dst, _ccxll_src)                                     \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    ccxll_free((_ccxll_dst));                                                  \
                                                                               \
    int _base_c;                                                               \
    _it_alloc((_ccxll_src), 1, &_base_c);                                      \
                                                                               \
    _CCXLL_INCR((_ccxll_src), *(_ccxll_src)._it[_base_c])                      \
        ccxll_push_back((_ccxll_dst), dref(*(_ccxll_src)._it[_base_c]));       \
                                                                               \
    _it_free((_ccxll_src), 1);                                                 \
)


#define ccxll_rearrange(_ccxll)                                                \
                                                                               \
STATEMENT_                                                                     \
(                                                                              \
    int _base_r;                                                               \
    _xl_alloc((_ccxll), 1, &_base_r);                                          \
                                                                               \
    ccxll_copy(*(_ccxll)._xl[_base_r],  (_ccxll));                             \
    ccxll_swap( (_ccxll), *(_ccxll)._xl[_base_r]);                             \
    ccxll_free(*(_ccxll)._xl[_base_r]);                                        \
                                                                               \
    _xl_free((_ccxll), 1);                                                     \
)



#endif
