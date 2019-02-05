#include "xmutexlockguard.h"
#include <QMutex>
XMutexLockGuard::XMutexLockGuard(QMutex& mutex) : mutex(mutex)
{
    mutex.lock();
}

XMutexLockGuard::~XMutexLockGuard() {
    mutex.unlock();
}
