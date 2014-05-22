/** \copyright
 * Copyright (c) 2013, Balazs Racz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are  permitted provided that the following conditions are met:
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
 * \file gc_pipe.hxx
 * Interface for creating gridconnect parser/renderer pipe components.
 *
 * @author Balazs Racz
 * @date 20 May 2013
 */

#ifndef _gc_pipe_hxx_
#define _gc_pipe_hxx_

#include <memory>

class Pipe;
class HubFlow;
template <class T> class FlowInterface;
template <class T, int N> class DispatchFlow;
class CanHubFlow;

class GCAdapterBase
{
public:
    virtual ~GCAdapterBase()
    {
    }

    /**
       This function connects an ASCII (GridConnect-format) CAN adapter to a
       binary CAN adapter, performing the necessary format conversions
       inbetween.

       Specifically, it takes two Hub flows as input, one carrying CAN frames
       in GridConnect protocol, and the other carrying CAN frames in the binary
       protocol.

       @param gc_side is the Hub that has the ASCII GridConnect traffic.

       @param can_side is the Hub that has the binary CAN traffic.

       @param double_bytes if true, any frame rendered into the GC protocol
       will have their characters doubled.

       @return a pointer to the created object. It can be deleted, which will
       terminate the link and unregister the link members from both pipes.
    */
    static GCAdapterBase *CreateGridConnectAdapter(HubFlow *gc_side,
                                                   CanHubFlow *can_side,
                                                   bool double_bytes);
    /** Creates a gridconnect-CAN bridge with separate pipes for reading
     * (parsing) from the GC side and writing (formatting) to the GC side. */
    static GCAdapterBase *CreateGridConnectAdapter(HubFlow *gc_side_read,
                                                   HubFlow *gc_side_write,
                                                   CanHubFlow *can_side,
                                                   bool double_bytes);
};

/** Create this port for a CAN hub and all packets will be written to stdout in
 * gridconnect format. */
class GcPacketPrinter {
public:
    GcPacketPrinter(CanHubFlow* can_hub);
    ~GcPacketPrinter();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/** Creates a new port on a CAN hub in gridconnect format. The port will
 * automatically be closed and deleted when the fd encounters an error.
 *
 * NOTE(balazs.racz): this cound be expanded to return an object pointer via
 * which the port can be closed. Also a method for notifying the caller about a
 * closedown would be helpful. */
void create_gc_port_for_can_hub(CanHubFlow* can_hub, int fd);

#endif //_gc_pipe_hxx_