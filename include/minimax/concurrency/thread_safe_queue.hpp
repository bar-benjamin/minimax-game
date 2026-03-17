#ifndef MINIMAX_CONCURRENCY_THREAD_SAFE_QUEUE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_CONCURRENCY_THREAD_SAFE_QUEUE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>
#include <cstddef>

namespace minimax
{
    namespace concurrency
    {

        template <typename T>
        class ThreadSafeQueue
        {
        public:
            explicit ThreadSafeQueue(std::size_t max_size = 0)
                : max_size_(max_size), closed_(false) {}

            ~ThreadSafeQueue() { close(); }

            ThreadSafeQueue(const ThreadSafeQueue &) = delete;
            ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

            bool push(T item)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                not_full_.wait(lock, [this]
                               { return closed_ || max_size_ == 0 || queue_.size() < max_size_; });
                if (closed_)
                    return false;
                queue_.push(std::move(item));
                not_empty_.notify_one();
                return true;
            }

            bool try_push(T item)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (closed_)
                    return false;
                if (max_size_ != 0 && queue_.size() >= max_size_)
                    return false;
                queue_.push(std::move(item));
                not_empty_.notify_one();
                return true;
            }

            std::optional<T> pop()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                not_empty_.wait(lock, [this]
                                { return !queue_.empty() || closed_; });
                if (queue_.empty())
                    return std::nullopt;
                T item = std::move(queue_.front());
                queue_.pop();
                not_full_.notify_one();
                return item;
            }

            std::optional<T> try_pop()
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (queue_.empty())
                    return std::nullopt;
                T item = std::move(queue_.front());
                queue_.pop();
                not_full_.notify_one();
                return item;
            }

            template <typename Rep, typename Period>
            std::optional<T> pop_for(const std::chrono::duration<Rep, Period> &timeout)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                bool signaled = not_empty_.wait_for(lock, timeout, [this]
                                                    { return !queue_.empty() || closed_; });
                if (!signaled || queue_.empty())
                    return std::nullopt;
                T item = std::move(queue_.front());
                queue_.pop();
                not_full_.notify_one();
                return item;
            }

            void close()
            {
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    closed_ = true;
                }
                not_empty_.notify_all();
                not_full_.notify_all();
            }

            // --- Queries ---

            bool empty() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return queue_.empty();
            }

            std::size_t size() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return queue_.size();
            }

            bool is_closed() const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                return closed_;
            }

        private:
            std::queue<T> queue_;
            mutable std::mutex mutex_;
            std::condition_variable not_empty_;
            std::condition_variable not_full_;
            std::size_t max_size_;
            bool closed_;
        };

    } // namespace concurrency
} // namespace minimax

#endif // MINIMAX_CONCURRENCY_THREAD_SAFE_QUEUE_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e