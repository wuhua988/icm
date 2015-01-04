/* -*- C++ -*- */

//=============================================================================
/**
 *  @file Unbounded_Queue.h
 *
 *  Unbounded_Queue.h,v 4.3 2001/05/03 17:24:01 schmidt Exp
 *
 *  @author Doug Schmidt
 */
//=============================================================================


#ifndef ACE_UNBOUNDED_QUEUE_H
#define ACE_UNBOUNDED_QUEUE_H

#include "icc/Node.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// For size_t under Chorus
#include "os/OS_Memory.h"

//class ACE_Allocator;

template <class T>
class UnboundedQueue;

/**
 * @class UnboundedQueueIterator
 *
 * @brief Implement an iterator over an unbounded queue.
 */
template <class T>
class UnboundedQueueIterator
{
public:
  // = Initialization method.
  UnboundedQueueIterator (UnboundedQueue<T> &q, int end = 0);

  // = Iteration methods.

  /// Pass back the <next_item> that hasn't been seen in the queue.
  /// Returns 0 when all items have been seen, else 1.
  int next (T *&next_item);

  /// Move forward by one element in the set.  Returns 0 when all the
  /// items in the queue have been seen, else 1.
  int advance (void);

  /// Move to the first element in the queue.  Returns 0 if the
  /// queue is empty, else 1.
  int first (void);

  /// Returns 1 when all items have been seen, else 0.
  int done (void) const;

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

private:
  /// Pointer to the current node in the iteration.
  ACE_Node<T> *current_;

  /// Pointer to the queue we're iterating over.
  UnboundedQueue<T> &queue_;
};

/**
 * @class UnboundedQueue
 *
 * @brief A Queue of "infinite" length.
 *
 * This implementation of an unbounded queue uses a circular
 * linked list with a dummy node.
 */
template <class T>
class UnboundedQueue
{
public:
  friend class UnboundedQueueIterator<T>;

  // Trait definition.
  typedef UnboundedQueueIterator<T> ITERATOR;

  // = Initialization and termination methods.
  /// construction.  Use user specified allocation strategy
  /// if specified.
  UnboundedQueue ();

  /// Copy constructor.
  UnboundedQueue (const UnboundedQueue<T> &);

  /// Assignment operator.
  void operator= (const UnboundedQueue<T> &);

  /// Destructor.
  ~UnboundedQueue (void);

  // = Check boundary conditions.

  /// Returns 1 if the container is empty, otherwise returns 0.
  int is_empty (void) const;

  /// Returns 1 if the container is full, otherwise returns 0.
  int is_full (void) const;

  // = Classic queue operations.

  /// Adds <new_item> to the tail of the queue.  Returns 0 on success,
  /// -1 on failure.
  int enqueue_tail (const T &new_item);

  /// Adds <new_item> to the head of the queue.  Returns 0 on success,
  /// -1 on failure.
  int enqueue_head (const T &new_item);

  /// Removes and returns the first <item> on the queue.  Returns 0 on
  /// success, -1 if the queue was empty.
  int dequeue_head (T &item);

  // = Additional utility methods.

  /// Reset the <UnboundedQueue> to be empty and release all its
  /// dynamically allocated resources.
  void reset (void);

  /// Get the <slot>th element in the set.  Returns -1 if the element
  /// isn't in the range {0..<size> - 1}, else 0.
  int get (T *&item, size_t slot = 0) const;

  /**
   * Set the <slot>th element in the set.  Will pad out the set with
   * empty nodes if <slot> is beyond the range {0..<size> - 1}.
   * Returns -1 on failure, 0 if <slot> isn't initially in range, and
   * 0 otherwise.
   */
  int set (const T &item, size_t slot);

  /// The number of items in the queue.
  size_t size (void) const;

  /// Dump the state of an object.
  void dump (void) const;

  // = STL-styled unidirectional iterator factory.
  UnboundedQueueIterator<T> begin (void);
  UnboundedQueueIterator<T> end (void);

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

protected:
  /// Delete all the nodes in the queue.
  void delete_nodes (void);

  /// Copy nodes into this queue.
  void copy_nodes (const UnboundedQueue<T> &);

  /// Pointer to the dummy node in the circular linked Queue.
  ACE_Node<T> *head_;

  /// Current size of the queue.
  size_t cur_size_;

  /// Allocation Strategy of the queue.
  //ACE_Allocator *allocator_;
};


#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "icc/UnboundedQueue.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("Unbounded_Queue.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */


#endif /* ACE_UNBOUNDED_QUEUE_H */
