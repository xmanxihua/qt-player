#ifndef XMUTEXLOCKGUARD_H
#define XMUTEXLOCKGUARD_H
#include "noncopyable.h"
#include <QMutex>
#include <assert.h>

class XMutexLockGuard : public noncopyable
{
public:
    explicit XMutexLockGuard(QMutex& mutex);
    ~XMutexLockGuard();
private:
    QMutex& mutex;
};
#endif // XMUTEXLOCKGUARD_H
