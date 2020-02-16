/** \copyright
 * Copyright (c) 2020, Balazs Racz
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
 * \file DirectHub.hxx
 *
 * Optimized class for ingress and egress of write-once objects, aimed to
 * support multi-recipient messages with fast internal fan-out but low compute
 * overhead.
 *
 * @author Balazs Racz
 * @date 9 Feb 2020
 */

#ifndef _UTILS_DIRECTHUB_HXX_
#define _UTILS_DIRECTHUB_HXX_


/// Empty class that can be used as a pointer for identifying where a piece of
/// data came from.
class HubSource {};

/// Interface for a downstream port of a hub (aka a target to send data to).
template <class T> class DirectHubPort : public HubSource
{
public:
    /// Send some data out on this port. The callee is reqponsible for
    /// enqueueing the data that came in this call if the available buffer
    /// space allows that.
    /// @param data is a share of the payload. The callee must not modify this
    /// copy.
    /// @param done non-null. If the callee cannot accept the incoming data,
    /// they should take a share of this barrier. The callee is then
    /// responsible for notifying it, and then the caller will re-try the call.
    /// @param priority hint to the callee on where to enqueue this message, if
    /// reordering is supported.
    virtual void send(
        shared_ptr<const T> data, BarrierNotifiable *done, unsigned priority) = 0;
};


/// Interface for a the central part of a hub.
 

template<class T>
class DirectHubInterface : public HubSource {
public:
    /// Adds a port to this hub. This port will be receiving all further
    /// messages.
    /// @param port the downstream port.
    virtual void register_port(DirectHubPort<T>* port) = 0;
    /// Removes a port from this hub. This port must have been registered
    /// previously.
    /// @param port the downstream port.
    virtual void unregister_port(DirectHubPort<T>* port) = 0;

    /// Send a piece of data to the hub.
    /// @param source is the sender of the message. This must be equal of a
    /// registered hub port, otherwise the message will be returned to the
    /// caller as well.
    /// @param data is the payload to send.
    /// @param done non-null. If the hub cannot accept the incoming data, the hub will take a share of this barrier, and notify it 
    virtual void send(HubSource* source, shared_ptr<const T> data, BarrierNotifiable* done, unsigned priority) = 0;
}

#endif // _UTILS_DIRECTHUB_HXX_