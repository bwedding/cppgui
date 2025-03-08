#pragma once
#include <algorithm>
#include <plog/Util.h>
#include <plog/Converters/UTF8Converter.h>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>

namespace plog
{
    template<class Formatter, class Converter = UTF8Converter>
    class AsyncRollingFileAppender : public IAppender
    {
    private:
        // We'll use a custom wrapper around Record to capture its state
        // without needing to copy the Record itself
        class RecordWrapper
        {
        public:
            RecordWrapper() = default;

            // Capture the record state when this wrapper is created
            explicit RecordWrapper(const Record& record)
            {
                // Save the formatted string right away - this preserves the exact state
                // of the Record at the time write() was called
                m_formattedMessage = Converter::convert(Formatter::format(record));
            }

            // Get the formatted message
            const std::string& getMessage() const
            {
                return m_formattedMessage;
            }

        private:
            std::string m_formattedMessage;
        };

    public:
        AsyncRollingFileAppender(const char* fileName, size_t maxFileSize = 0, int maxFiles = 0, size_t queueSize = 1000)
            : m_fileSize()
            , m_maxFileSize((std::max)(maxFileSize, static_cast<size_t>(1000))) // set a lower limit for the maxFileSize
            , m_lastFileNumber((std::max)(maxFiles - 1, 0))
            , m_firstWrite(true)
            , m_maxQueueSize(queueSize)
            , m_running(true)
        {
            util::splitFileName(fileName, m_fileNameNoExt, m_fileExt);

            // Start the writer thread
            m_writerThread = std::thread(&AsyncRollingFileAppender::writerThreadFunc, this);
        }

        virtual ~AsyncRollingFileAppender()
        {
            // Signal the writer thread to stop and wait for it
            m_running = false;
            m_queueCV.notify_one();

            if (m_writerThread.joinable())
            {
                m_writerThread.join();
            }
        }

        virtual void write(const Record& record)
        {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);

                // If queue is full, drop the message
                if (m_queue.size() < m_maxQueueSize)
                {
                    // Create a wrapper that formats the message right away
                    // This is an optimization - we do the format work upfront
                    // to eliminate thread contention on the Record object
                    m_queue.emplace(record);
                    m_queueCV.notify_one();
                }
                // else: message dropped
            }
        }

    private:
        void writerThreadFunc()
        {
            while (m_running)
            {
                bool hasRecord = false;
                RecordWrapper recordWrapper;

                // Get a record wrapper from the queue
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);

                    // Wait for a record or shutdown signal
                    m_queueCV.wait(lock, [this] {
                        return !m_queue.empty() || !m_running;
                        });

                    if (!m_queue.empty())
                    {
                        recordWrapper = std::move(m_queue.front());
                        m_queue.pop();
                        hasRecord = true;
                    }
                }

                // Process the record if we have one
                if (hasRecord)
                {
                    // Use the pre-formatted message
                    const std::string& line = recordWrapper.getMessage();

                    util::MutexLock fileLock(m_fileMutex);

                    if (m_firstWrite)
                    {
                        openLogFile();
                        m_firstWrite = false;
                    }
                    else if (m_lastFileNumber > 0 && m_fileSize > m_maxFileSize && static_cast<size_t>(-1) != m_fileSize)
                    {
                        rollLogFiles();
                    }

                    int bytesWritten = m_file.write(line);
                    if (bytesWritten > 0)
                    {
                        m_fileSize += bytesWritten;
                    }
                }
            }

            // Flush any remaining records in the queue before exiting
            flushQueue();
        }

        void flushQueue()
        {
            std::unique_lock<std::mutex> queueLock(m_queueMutex);

            while (!m_queue.empty())
            {
                RecordWrapper recordWrapper = std::move(m_queue.front());
                m_queue.pop();
                queueLock.unlock();

                const std::string& line = recordWrapper.getMessage();

                {
                    util::MutexLock fileLock(m_fileMutex);

                    if (m_firstWrite)
                    {
                        openLogFile();
                        m_firstWrite = false;
                    }
                    else if (m_lastFileNumber > 0 && m_fileSize > m_maxFileSize && static_cast<size_t>(-1) != m_fileSize)
                    {
                        rollLogFiles();
                    }

                    int bytesWritten = m_file.write(line);
                    if (bytesWritten > 0)
                    {
                        m_fileSize += bytesWritten;
                    }
                }

                queueLock.lock();
            }
        }

        void rollLogFiles()
        {
            m_file.close();
            std::string lastFileName = buildFileName(m_lastFileNumber);
            util::File::unlink(lastFileName.c_str());
            for (int fileNumber = m_lastFileNumber - 1; fileNumber >= 0; --fileNumber)
            {
                std::string currentFileName = buildFileName(fileNumber);
                std::string nextFileName = buildFileName(fileNumber + 1);
                util::File::rename(currentFileName.c_str(), nextFileName.c_str());
            }
            openLogFile();
        }

        void openLogFile()
        {
            std::string fileName = buildFileName();
            m_fileSize = m_file.open(fileName.c_str());
            if (0 == m_fileSize)
            {
                int bytesWritten = m_file.write(Converter::header(Formatter::header()));
                if (bytesWritten > 0)
                {
                    m_fileSize += bytesWritten;
                }
            }
        }

        std::string buildFileName(int fileNumber = 0)
        {
            std::stringstream ss;
            ss << m_fileNameNoExt;
            if (fileNumber > 0)
            {
                ss << '.' << fileNumber;
            }
            if (!m_fileExt.empty())
            {
                ss << '.' << m_fileExt;
            }
            return ss.str();
        }

    private:
        // File-related members
        util::Mutex     m_fileMutex;
        util::File      m_file;
        size_t          m_fileSize;
        const size_t    m_maxFileSize;
        const int       m_lastFileNumber;
        std::string     m_fileExt;
        std::string     m_fileNameNoExt;
        bool            m_firstWrite;

        // Queue and thread-related members
        std::queue<RecordWrapper> m_queue;
        std::mutex m_queueMutex;
        std::condition_variable m_queueCV;
        std::thread m_writerThread;
        std::atomic<bool> m_running;
        const size_t m_maxQueueSize;
    };
}