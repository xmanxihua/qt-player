#ifndef UTILS_H
#define UTILS_H
#include "noncopyable.h"


#include <string>
extern "C" {
#include <libavformat/avformat.h>
}
class Utils : public noncopyable
{
public:
    static double r2d(AVRational rational);
    static void av_perr(std::string usage, int ret);
private:
    Utils();
    ~Utils();

};

#endif // UTILS_H
