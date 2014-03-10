/** \copyright
 * Copyright (c) 2013, Balazs Racz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file AsyncIf.cxx
 *
 * Asynchronous NMRAnet interface.
 *
 * @author Balazs Racz
 * @date 4 Dec 2013
 */

#include "nmranet/AsyncIf.hxx"

namespace NMRAnet
{

Buffer* node_id_to_buffer(NodeID id)
{
    Buffer* ret = buffer_alloc(6);
    id = htobe64(id);
    uint8_t* src = reinterpret_cast<uint8_t*>(&id);
    memcpy(ret->start(), src + 2, 6);
    ret->advance(6);
    return ret;
}

NodeID buffer_to_node_id(Buffer* buf)
{
    HASSERT(buf);
    HASSERT(buf->used() == 6);
    uint64_t d = 0;
    memcpy(reinterpret_cast<uint8_t*>(&d) + 2, buf->start(), 6);
    return be64toh(d);
}

/// @TODO(balazs.racz): make the map size parametrizable.
AsyncIf::AsyncIf(Executor* executor, int local_nodes_count)
    : dispatcher_(executor), localNodes_(local_nodes_count)
{
}

} // namespace NMRAnet
