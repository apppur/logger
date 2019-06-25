#include <thread>
#include <iostream>
#include "logger.h"

// a callable object
class callable 
{
    public:
        void operator()(int x)
        {
            for (int i = 0; i < x; i++)
            {
                LOG_DEBUG("debug:{} count{}", "I love you applepurple and gauss and nuonuo!", i);
                LOG_INFO("info:{} count{}", "I love you applepurple and gauss and nuonuo!", i);
                LOG_ERROR("error:{} count{}", "I love you applepurple and gauss and nuonuo!", i);
            }
        }
};

int main(int argc, char** argv)
{
    //std::thread thread1(callable(), 100000);
    //std::thread thread2(callable(), 100000);
    std::thread thread1(callable(), 1);
    std::thread thread2(callable(), 1);

    thread1.join();
    thread2.join();

    std::string homeDir(getHomeDirectory());
    homeDir.append("/logs/rocketmq-cpp/");
    std::string fileName = to_string(getpid()) + "_" + "rocketmq-cpp.log";
    std::string logFile = homeDir + fileName;

    LOG_DEBUG("debug %s, %d", "I love you!", 1000);
    LOG_ERROR("error log file:{}", logFile);
    LOG_DEBUG("debug log file:{}", logFile);

    ALOG_LOGGER.SetErrorLevel();
    LOG_ERROR("error log file:{}", logFile);
    LOG_DEBUG("debug log file:{}", logFile);

    ALOG_LOGGER.SetInfoLevel();
    LOG_ERROR("error log file:{}", logFile);
    LOG_DEBUG("debug log file:{}", logFile);

    return 0;
}
