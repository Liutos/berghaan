#include "log.h"

#include <stdarg.h>
#include <syslog.h>

void
log_close(void)
{
    closelog();
}

void
log_lprintf(int priority, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsyslog(priority, format, ap);
    va_end(ap);
}

void
log_open(void)
{
    openlog("berghaan", 0, 0);
}
