/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_LOG_HPP
#define LD_BASE_LOG_HPP
#include "sg/defs.h"
namespace Base {

struct Log {
    /// Log an debug message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void debug(const char *msg, ...);

    /// Log an info message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void info(const char *msg, ...);

    /// Log a warning message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void warn(const char *msg, ...);

    /// Log an error message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void error(const char *msg, ...);

    /// Abort with the given message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void abort(const char *msg, ...);
};

}
#endif
