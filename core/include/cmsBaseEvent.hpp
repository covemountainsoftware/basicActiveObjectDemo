/*
MIT License

Copyright (c) <2019-2020> <Matthew Eshleman - https://covemountainsoftware.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CMSBASEEVENT_HPP
#define CMSBASEEVENT_HPP

#include <type_traits>

namespace cms
{

/**
 * @brief The BaseEvent POD struct - users should derive from
 *        this struct if additional event payload fields are
 *        desired.
 */
template<typename SignalType>
struct BaseEvent
{
    static_assert(std::is_integral<SignalType>::value, "The type SignalType must be an integral type, such as uint8_t, uint32_t, etc.");
    static_assert(std::is_unsigned<SignalType>::value, "The type SignalType must be an unsigned integral type.");

    BaseEvent() = default;
    constexpr explicit BaseEvent(SignalType sig) : signal(sig) {}

    SignalType signal;
};

} //namespace cms

#endif // CMSEVENT_HPP
