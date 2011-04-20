#ifndef _ecore_link_h_
#define _ecore_link_h_ 1


#include "ecore_config.h"


// SLINK - 单向链表
//
//typedef struct _SLINK{
//   _SLINK* _next;
//} SLINK,*PSLINK;


//初始化单向链表
//template< typename PSLINK >
//inline void ecore_slink_initialize(PSLINK _head )
//{ (_head)->_next = NULL; }
#define ecore_slink_initialize(_head)           ((_head)->_next = NULL)

//检测单向链表是否为空
//template< typename PSLINK >
//inline bool ecore_slink_is_empty(PSLINK _head )
//{ return ((_head)->_next == NULL);}
#define ecore_slink_is_empty(_head)              ((_head)->_next == NULL)

//取出单向链表第一个项目
//template< typename PSLINK ,typename PVALUE >
//inline PVALUE ecore_slink_pop(PSLINK _head )
//{
//  PVALUE head_item = (_head)->_next;
//  (_head)->_next =  ((_head)->_next->_next);
//  return head_item;
//}
#define ecore_slink_pop(_head)                  (_head)->_next;\
                                          (_head)->_next =  (_head)->_next->_next;

//将项目放入单向链表头部
//template< typename PSLINK ,typename PVALUE >
//inline void ecore_slink_push(PSLINK  _head, PVALUE  _link)
//{
// (_link)->_next =  (_head)->_next;
// (_head)->_next =  (_link);
//}
#define ecore_slink_push(_head, _link)          (_link)->_next =  (_head)->_next; \
                                          (_head)->_next =  (_link)

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////DOUBLE///LINK/////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//DLINK - 双向链表
//
//typedef struct _DLINK{
//   _DLINK* _prev;
//   _DLINK* _next;
//} DLINK,*PDLINK;

//初始化双向链表
//template< typename PDLINK >
//void ecore_dlink_initialize(PDLINK  _head)
//{
// (_head)->_next = (_head)->_prev = (_head);
//}
#define ecore_dlink_initialize(_head)            ((_head)->_next = (_head)->_prev = (_head))

//检测双向链表是否为空
//template< typename PDLINK >
//bool ecore_dlink_is_empty(PDLINK)
//{
//  return ((_head)->_next == (_head));
//}
#define ecore_dlink_is_empty(_head)               ((_head)->_next == (_head))

//将项目放入双向链表头部之后
//template< typename PDLINK ,typename PVALUE >
//inline void ecore_dlink_insert_at_next(PDLINK _head,PVALUE _dlink)
//{
//	(_dlink)->_next = (_head)->_next;
//  (_dlink)->_prev = (_head);
//  (_head)->_next->_prev = (_dlink);
//  (_head)->_next = (_dlink);
//}
#define ecore_dlink_insert_at_next(_head,_dlink)     (_dlink)->_next = (_head)->_next;\
                                           (_dlink)->_prev = (_head);\
                                           (_head)->_next->_prev = (_dlink);\
                                           (_head)->_next = (_dlink)

//将项目放入双向链表头部之前
//template< typename PDLINK ,typename PVALUE >
//inline void ecore_dlink_insert_at_prev(PDLINK _head,PVALUE _dlink )
//{
//    (_dlink)->_prev = (_head)->_prev;
//    (_dlink)->_next = (_head);
//    (_head)->_prev->_next = (_dlink);
//    (_head)->_prev = (_dlink);
//}
#define ecore_dlink_insert_at_prev(_head,_dlink)     (_dlink)->_prev = (_head)->_prev;\
                                           (_dlink)->_next = (_head);\
                                           (_head)->_prev->_next = (_dlink);\
                                           (_head)->_prev = (_dlink)
//从双向链表中删除当前项目
//template< typename PDLINK >
//inline void ecore_dlink_remove(PDLINK _dlink)
//{
//	(_dlink)->_prev->_next = (_dlink)->_next;
//    (_dlink)->_next->_prev = (_dlink)->_prev;
//}
#define ecore_dlink_remove(_dlink)               (_dlink)->_prev->_next = (_dlink)->_next;\
                                           (_dlink)->_next->_prev = (_dlink)->_prev
//从双向链表中取出当前项目的前一个
//template< typename PDLINK ,typename PVALUE >
//inline PVALUE ecore_dlink_extruct_prev(PDLINK _head )
//{
//	PVALUE v = (_head)->_prev;
//     ecore_dlink_remove((_head)->_prev);
//	 return v;
//}
#define ecore_dlink_extruct_prev(_head)           (_head)->_prev;\
                                          ecore_dlink_remove((_head)->_prev)
//从双向链表中取出当前项目的下一个
//template< typename PDLINK ,typename PVALUE >
//inline PVALUE ecore_dlink_extruct_next(PDLINK _head)
//{
//	PVALUE v = (_head)->_next;
//    ecore_dlink_remove((_head)->_next);
//	return v;
//}
#define ecore_dlink_extruct_next(_head)           (_head)->_next;\
                                           ecore_dlink_remove((_head)->_next)


#endif // _ecore_link_h_
