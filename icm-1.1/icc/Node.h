/* -*- C++ -*- */

//=============================================================================
/**
 *  @file Node.h
 *
 *  Node.h,v 4.2 2001/03/27 09:20:17 doccvs Exp
 *
 *  @author Doug Schmidt
 */
//=============================================================================


#ifndef ACE_NODE_H
#define ACE_NODE_H

#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// Forward declarations.
template <class T> class ACE_Unbounded_Set;
template <class T> class ACE_Unbounded_Set_Iterator;
template <class T> class ACE_Unbounded_Set_Const_Iterator;
template <class T> class UnboundedQueue;
template <class T> class UnboundedQueueIterator;
template <class T> class ACE_Unbounded_Stack;
template <class T> class ACE_Unbounded_Stack_Iterator;

/**
 * @class ACE_Node
 *
 * @brief Implementation element in a Queue, Set, and Stack.
 */
template<class T>
class ACE_Node
{
public:
  friend class UnboundedQueue<T>;
  friend class UnboundedQueueIterator<T>;
  friend class ACE_Unbounded_Set<T>;
  friend class ACE_Unbounded_Set_Iterator<T>;
  friend class ACE_Unbounded_Set_Const_Iterator<T>;
  friend class ACE_Unbounded_Stack<T>;
  friend class ACE_Unbounded_Stack_Iterator<T>;

# if ! defined (ACE_HAS_BROKEN_NOOP_DTORS)
  /// This isn't necessary, but it keeps some compilers happy.
  ~ACE_Node (void);
# endif /* ! defined (ACE_HAS_BROKEN_NOOP_DTORS) */

private:
  // = Initialization methods
  ACE_Node (const T &i, ACE_Node<T> *n);
  ACE_Node (ACE_Node<T> *n = 0, int = 0);
  ACE_Node (const ACE_Node<T> &n);

  /// Pointer to next element in the list of <ACE_Node>s.
  ACE_Node<T> *next_;

  /// Current value of the item in this node.
  T item_;
};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "icc/Node.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("Node.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */


#endif /* ACE_NODE_H */
