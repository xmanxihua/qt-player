#include "utils.h"
#include <string>
extern "C" {
#include <libavformat/avformat.h>
}
Utils::Utils()
{

}

double Utils::r2d(AVRational rational) {
    if (rational.den == 0) {
        return 0;
    }

    return (double)rational.num / (double)rational.den;
}

void Utils::av_perr(std::string usage, int ret) {
    char buf[1024] = {0};
    av_strerror(ret,buf,sizeof(buf));
    printf("%s: %s\n", usage.c_str(), buf);
}
