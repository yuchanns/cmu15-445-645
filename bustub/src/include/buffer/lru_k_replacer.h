//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.h
//
// Identification: src/include/buffer/lru_k_replacer.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <limits>
#include <list>
#include <mutex> // NOLINT
#include <unordered_map>
#include <vector>

#include "common/config.h"
#include "common/macros.h"

namespace bustub {

/**
 * LRUKReplacer implements the LRU-k replacement policy.
 *
 * The LRU-k algorithm evicts a frame whose backward k-distance is maximum
 * of all frames. Backward k-distance is computed as the difference in time
 * between current timestamp and the timestamp of kth previous access.
 *
 * A frame with less than k historical references is given
 * +inf as its backward k-distance. When multiple frames have +inf backward
 * k-distance, classical LRU algorithm is used to choose victim.
 */
class LRUKReplacer {
public:
  /**
   *
   * DONE(P1): Add implementation
   *
   * @brief a new LRUKReplacer.
   * @param num_frames the maximum number of frames the LRUReplacer will be
   * required to store
   */
  explicit LRUKReplacer(size_t num_frames, size_t k);

  DISALLOW_COPY_AND_MOVE(LRUKReplacer);

  /**
   * DONE(P1): Add implementation
   *
   * @brief Destroys the LRUReplacer.
   */
  ~LRUKReplacer() = default;

  /**
   * DONE(P1): Add implementation
   *
   * @brief Find the frame with largest backward k-distance and evict that
   * frame. Only frames that are marked as 'evictable' are candidates for
   * eviction.
   *
   * A frame with less than k historical references is given +inf as its
   * backward k-distance. If multiple frames have inf backward k-distance, then
   * evict the frame with the earliest timestamp overall.
   *
   * Successful eviction of a frame should decrement the size of replacer and
   * remove the frame's access history.
   *
   * @param[out] frame_id id of frame that is evicted.
   * @return true if a frame is evicted successfully, false if no frames can be
   * evicted.
   */
  auto Evict(frame_id_t *frame_id) -> bool;

  /**
   * DONE(P1): Add implementation
   *
   * @brief Record the event that the given frame id is accessed at current
   * timestamp. Create a new entry for access history if frame id has not been
   * seen before.
   *
   * If frame id is invalid (ie. larger than replacer_size_), throw an
   * exception. You can also use BUSTUB_ASSERT to abort the process if frame id
   * is invalid.
   *
   * @param frame_id id of frame that received a new access.
   */
  void RecordAccess(frame_id_t frame_id);

  /**
   * DONE(P1): Add implementation
   *
   * @brief Toggle whether a frame is evictable or non-evictable. This function
   * also controls replacer's size. Note that size is equal to number of
   * evictable entries.
   *
   * If a frame was previously evictable and is to be set to non-evictable, then
   * size should decrement. If a frame was previously non-evictable and is to be
   * set to evictable, then size should increment.
   *
   * If frame id is invalid, throw an exception or abort the process.
   *
   * For other scenarios, this function should terminate without modifying
   * anything.
   *
   * @param frame_id id of frame whose 'evictable' status will be modified
   * @param set_evictable whether the given frame is evictable or not
   */
  void SetEvictable(frame_id_t frame_id, bool set_evictable);

  /**
   * DONE(P1): Add implementation
   *
   * @brief Remove an evictable frame from replacer, along with its access
   * history. This function should also decrement replacer's size if removal is
   * successful.
   *
   * Note that this is different from evicting a frame, which always remove the
   * frame with largest backward k-distance. This function removes specified
   * frame id, no matter what its backward k-distance is.
   *
   * If Remove is called on a non-evictable frame, throw an exception or abort
   * the process.
   *
   * If specified frame is not found, directly return from this function.
   *
   * @param frame_id id of frame to be removed
   */
  void Remove(frame_id_t frame_id);

  /**
   * DONE(P1): Add implementation
   *
   * @brief Return replacer's size, which tracks the number of evictable frames.
   *
   * @return size_t
   */
  auto Size() -> size_t;

private:
  // DONE(student): implement me! You can replace these member variables as you
  // like. Remove maybe_unused if you start using them.
  size_t current_timestamp_{0};
  size_t curr_size_{0};
  size_t replacer_size_;
  size_t k_;
  std::mutex latch_;

  struct FrameInfo {
    frame_id_t frame_id;
    std::vector<size_t> accesses;
    bool evictable;
  };

  std::vector<FrameInfo> frames_;

  auto FindInternal(frame_id_t frame_id, size_t &index) -> bool {
    for (size_t i = 0; i < frames_.size(); i++) {
      if (frames_[i].frame_id == frame_id) {
        index = i;
        return true;
      }
    }
    return false;
  }

  void RemoveInternal(frame_id_t frame_id) {
    size_t index;
    if (!FindInternal(frame_id, index)) {
      return;
    }
    BUSTUB_ASSERT(frames_[index].evictable, "frame_id is non-evictable");
    frames_.erase(frames_.begin() + index);
    curr_size_--;
  }

  void SetEvictableInternal(frame_id_t frame_id, bool set_evictable) {
    size_t index = 0;
    BUSTUB_ASSERT(FindInternal(frame_id, index), "frame_id is invalid");
    if (!frames_[index].evictable && set_evictable) {
      curr_size_++;
    } else if (frames_[index].evictable && !set_evictable) {
      curr_size_--;
    }
    frames_[index].evictable = set_evictable;
  }

  void RecordAccessInternal(frame_id_t frame_id) {
    current_timestamp_++;
    size_t index = 0;
    if (!FindInternal(frame_id, index)) {
      BUSTUB_ASSERT(frames_.size() < replacer_size_,
                    "frame size exceeds the limit");

      frames_.push_back(FrameInfo{frame_id, std::vector<size_t>(), false});
      index = frames_.size() - 1;
    }
    frames_[index].accesses.push_back(current_timestamp_);
  }

  auto EvictInternal(frame_id_t *frame_id) -> bool {
    current_timestamp_++;
    size_t distance = 0;
    bool found = false;
    for (auto it = frames_.rbegin(); it != frames_.rend(); ++it) {
      if (!it->evictable) {
        continue;
      }
      found = true;
      if (it->accesses.size() < k_) {
        distance = SIZE_MAX;
        *frame_id = it->frame_id;
      } else if (current_timestamp_ - it->accesses[it->accesses.size() - k_] >
                 distance) {
        distance = current_timestamp_ - it->accesses[k_ - 1];
        *frame_id = it->frame_id;
      }
    }

    if (found) {
      RemoveInternal(*frame_id);
    }
    return found;
  }
};

} // namespace bustub
