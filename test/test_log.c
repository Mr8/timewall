#include "../src/all.h"

int main(){
    log_handler * LOG = TXTLOGHANDLER("/tmp/timewall.log");
    LOG_DEBUG(LOG, "This is debug level log");
    LOG_INFO(LOG, "This is info level log");
    LOG_ERROR(LOG, "This is error level log");
    LOG_FATAL(LOG, "This is fatal level log");
    return 0;
}
