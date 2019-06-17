#include <thread>
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
    std::thread thread1(callable(), 100000);
    std::thread thread2(callable(), 100000);

    thread1.join();
    thread2.join();
    return 0;
}
