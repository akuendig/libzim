/*
 * Copyright (C) 2017 Matthieu Gautier <mgautier@kymeria.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "buffer.h"

#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#if !defined(_WIN32)
#include <sys/mman.h>
#endif

namespace zim {

std::shared_ptr<const Buffer> Buffer::sub_buffer(offset_t offset, zsize_t size) const
{
  return std::make_shared<SubBuffer>(shared_from_this(), offset, size);
}

#if !defined(_WIN32)
MMapBuffer::MMapBuffer(int fd, offset_t offset, zsize_t size):
  Buffer(size),
  _offset(0)
{
  offset_t pa_offset(offset.v & ~(sysconf(_SC_PAGE_SIZE) - 1));
  _offset = offset-pa_offset;
#if defined(__APPLE__)
  #define MAP_FLAGS MAP_PRIVATE
#else
  #define MAP_FLAGS MAP_PRIVATE|MAP_POPULATE
#endif
  _data = (char*)mmap(NULL, size.v + _offset.v, PROT_READ, MAP_FLAGS, fd, pa_offset.v);
  if (_data == MAP_FAILED )
  {
    throw std::runtime_error(std::string("Cannot mmap : ") + strerror(errno));
  }
#undef MAP_FLAGS
}

MMapBuffer::~MMapBuffer()
{
  munmap(_data, size_.v + _offset.v);
}

#endif

} //zim
