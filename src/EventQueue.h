#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <condition_variable>
#include "UIEvent.h"
#include "EventDispatcher.h"

namespace HeartControl {

    // Usage Notes:
    // Lifetime Management : Ensure EventDispatcher outlives the EventQueue
    class EventQueue
    {
    public:
        EventQueue() = default;

        ~EventQueue() 
        {
            stopProcessing();
            if (processor_thread_.joinable())
            {
                processor_thread_.join();
            }
        }

        EventQueue(const EventQueue&) = delete;
        EventQueue& operator=(const EventQueue&) = delete;

        void enqueue(UIEvent&& event)
        {
            std::lock_guard lock(mutex_);
            events_.emplace(std::move(event));
            cv_.notify_one();
        }

        void stopProcessing()
        {
            {
                std::lock_guard lock(mutex_);
                running_ = false;
                // Optional: Clear queue if needed
                // events_ = std::queue<UIEvent>();
            }
            cv_.notify_all();
        }

        void startProcessing(EventDispatcher& dispatcher)
        {
            std::lock_guard lock(mutex_);
            if (processor_thread_.joinable())
            {
                throw std::runtime_error("Thread already running");
            }
            running_ = true;
            processor_thread_ = std::thread([this, &dispatcher]
            {
                    std::unique_lock innerlock(mutex_);
                    while (running_) {
                        cv_.wait(innerlock, [this]
                        {
                                return !events_.empty() || !running_;
                            });

                        // If we're not running anymore, exit the loop
                        if (!running_) {
                            break;
                        }

                        // Process all available events
                        while (!events_.empty() && running_)  // Added running_ check
                        {
                            auto event = std::move(events_.front());
                            events_.pop();
                            innerlock.unlock();
                            try {
                                spdlog::info("Processing event from queue: '{}'", event.type);
                                dispatcher.dispatch(event);
                            }
                            catch (const std::exception& e) {
                                spdlog::error("Error dispatching event: {}", e.what());
                            }
                            innerlock.lock();
                        }
                    }
                });
        }

    private:
        mutable std::mutex mutex_;
        std::queue<UIEvent> events_{};
        std::condition_variable cv_;
        bool running_ = true;
        std::thread processor_thread_;
    };
}