#ifndef COMMAND_QUEUE
#define COMMAND_QUEUE

#include "graphical_device.h"

#include <cstring>
#include <iostream>

template <typename... Args> class CommandQueue {
    unsigned char *m_data;
    unsigned int m_size;
    unsigned int m_capacity;

  public:
    CommandQueue() {
        m_capacity = 1024;
        m_data = new unsigned char[m_capacity];
        m_size = 0;
    }

    void reallocate() {
        std::cout << "New capacity: " << m_capacity << " " << m_size << " This: " << (size_t)this
                  << std::endl;
        m_capacity *= 2;
        unsigned char *new_ptr = new unsigned char[m_capacity];
        memcpy(new_ptr, m_data, m_size);
        delete[] m_data;
        m_data = new_ptr;
    }

    template <typename T> void post(const T &command) {
        if (m_capacity < m_size + sizeof(T)) {
            reallocate();
        }
        memcpy(m_data + m_size, &command, sizeof(T));
        m_size += sizeof(T);
    }

    void clear() { m_size = 0; }

    template <typename T> void submit(T &t) {
        bool command_valid = true;
        for (int ptr = 0; ptr < m_size && command_valid;) {
            // std::cout << "Command id: " << *((unsigned int *)(m_data + ptr)) << std::endl;
            command_valid = ((*(unsigned int *)(m_data + ptr) == Args::COMMAND_TYPE &&
                              (t.process((Args *)(m_data + ptr)), ptr += sizeof(Args), true)) ||
                             ...);
        }
        if (!command_valid) {
            // char error[512];
            // sprintf(error, "Invalid command id detected in render steam: %d", )
            throw std::runtime_error("Invalid command detected in render stream");
        }
    }
};

typedef CommandQueue<Commands::Draw, Commands::DrawInstanced, Commands::Clear,
                     Commands::SetViewport>
    BaseCommandQueue;

#endif /*COMMAND_QUEUE*/
