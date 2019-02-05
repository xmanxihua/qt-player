#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H


class noncopyable
{
public:
    noncopyable();
private:
    noncopyable(const noncopyable&);
    noncopyable operator = (const noncopyable&);
};

#endif // NONCOPYABLE_H
