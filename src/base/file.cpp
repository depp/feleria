/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "sg/entry.h"
#include "file.hpp"
#include <cstdlib>
namespace Base {

void Data::read(const std::string &path, size_t maxsz,
                const char *extensions) {
    sg_filedata *data;
    int r = sg_file_load(&data, path.data(), path.size(), 0,
                         extensions, maxsz, nullptr, nullptr);
    if (r)
        sg_sys_abortf("could not read file: %s", path.c_str());
    if (m_data)
        sg_filedata_decref(m_data);
    m_data = data;
}

}
