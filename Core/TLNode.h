#ifndef TLNODE_H
#define TLNODE_H

#include "Core/Version.h"

namespace rapid
{

template<typename T>
struct NodeBase
{
public:
    alignas(__alignof__(T)) unsigned char __Data[sizeof(T)];

    NodeBase() { }

    template<typename ... Args>
    NodeBase(Args ... args)
    { construct(args...); }

    template<typename ... Args>
    void construct(Args ... args)
    { ::new(address()) T(args...); }

    T* address()
    { return reinterpret_cast<T *>(&__Data[0]); }

    T content()
    { return *address(); }

    T& ref_content()
    { return *address(); }

    const T const_content()
    { return *address(); }

    const T& const_ref_content()
    { return *address(); }

//    void clear()
//    { mem_clear(address(), sizeof(NodeBase<T>)); }
};

};

#endif // TLNODE_H
