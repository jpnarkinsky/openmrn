#ifndef _DEVICE_BUFFER_HXX_
#define _DEVICE_BUFFER_HXX_

#include <new>

#include "Devtab.hxx"


/** Implements a smart buffer specifically desinged for character
 * device drivers.  Technically, the private metadata for the size and index
 * counters should more properly be implemenented as size_t types.  The choice
 * of uint16_t types is simply to same memory as 2^16 is normally a reasonable
 * maximum size for this type of metadata.
 */
template <typename T> class DeviceBuffer
{
public:
    /** Create a DeviceBuffer instance.
     * @param size size in items for the DeviceBuffer
     */
    static DeviceBuffer *create(size_t size)
    {
        HASSERT(size <= UINT16_MAX);
        DeviceBuffer *device_buffer =
            (DeviceBuffer*)malloc(sizeof(DeviceBuffer) + (size * sizeof(T)));
        /* placement new allows for runtime ring buffer size */
        new (device_buffer) DeviceBuffer(size);
        
        return device_buffer;
    }

    /** Destroy an existing DeviceBuffer instance.
     */
    void destroy()
    {
        free(this);
    }

    /** Insert a number of items to the buffer.
     * @param buf reference to the first item to insert
     * @param items total number of items to insert
     * @return total number of items inserted
     */
    size_t put(const T *buf, size_t items)
    {
        size_t last_count = count;
        
        while (items && count < size)
        {
            data[writeIndex++] = *buf++;
            if (writeIndex == size)
            {
                writeIndex = 0;
            }
            ++count;
            --items;
        }
        
        return count - last_count;
    }

    /** remove a number of items from the buffer.
     * @param buf reference to the data removed
     * @param items total number of items to remove
     * @return total number of items removed
     */
    size_t get(T *buf, size_t items)
    {
        size_t last_count = count;
        
        while (items && count > 0)
        {
            *buf++ = data[readIndex++];            
            if (readIndex == size)
            {
                readIndex = 0;
            }
            --count;
            --items;
        }
        
        return last_count - count;
    }

    /** lock access to the buffer.
     */
    void lock()
    {
        Device::mutex.lock();
    }

    /** unlock access to the buffer.
     */
    void unlock()
    {
        Device::mutex.lock();
    }

    /** Block until the wait condition is true.  The condition is defined by
     * the user of the buffer and could be that their is data in the buffer or
     * it could be that there is room in the buffer.  In any case, this method
     * should be called only when the buffer is locked with the @ref lock
     * method.  Internally the lock is released before blocking to prevent
     * deadlock.  the lock is grabbed once again before the method returns.
     */
    void block_until_condition()
    {
        portENTER_CRITICAL(); // lock out interrupts
        for ( ; /* forever */ ; )
        {
            waiting = true;
            portEXIT_CRITICAL(); // release lock
            unlock(); // release lock

            sem->wait(); // wait for condition to become true.

            lock(); // lock out other threads
            portENTER_CRITICAL(); // lock out interrupts
            if (!waiting)
            {
                waiting = false;
                portEXIT_CRITICAL(); // release lock
                break;
            }
            /* another thread beat us, we need to continue waiting */
        }
    }

    /** Signal the wakeup condition.
     */
    void signal_condition()
    {
        portENTER_CRITICAL();
        if (waiting)
        {
            sem->post();
        }
        portEXIT_CRITICAL();
    }

    /** Signal the wakeup condition from an ISR context.
     */
    void signal_condition_from_isr()
    {
        if (waiting)
        {
            int woken = 0;
            sem->post_from_isr(&woken);
        }
    }

private:
    /** Constructor.
     * @param size size in items for the buffer.
     */
    DeviceBuffer(size_t size)
        : selectInfo()
        , sem(0)
        , waiting(false)
        , size(size)
        , count(0)
        , readIndex(0)
        , writeIndex(0)
    {
    }

    /** Destructor.
     */
    ~DeviceBuffer()
    {
    }

    DISALLOW_COPY_AND_ASSIGN(DeviceBuffer);

    /** Metadata for select() logic */
    Device::SelectInfo selectInfo;

    /** Semaphore that will be used for asynchonous wait/wakeup */
    OSSem *sem;

    /** is the buffer currently waiting */
    bool waiting;
    
    /** size in items of buffer */
    uint16_t size;
    
    /** total number of items in buffer */
    uint16_t count;
    
    /** read index */
    uint16_t readIndex;
    
    /** write index */
    uint16_t writeIndex;

    /** buffer data */
    T data[];
};

#endif /* _DEVICE_BUFFER_HXX_ */