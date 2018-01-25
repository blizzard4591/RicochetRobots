//
// Created by harold on 25.01.18.
//

#pragma once

#include <initializer_list>
#include <ostream>
#include <type_traits>

namespace ricochet {

    /**
     * Class to handle bit flag values based on a (class) enum
     * @tparam FlagEnum Type of enum containing flag values. Members should
     * have a singel bit set
     * @tparam FlagType Store type for combined flags
     */
    template<typename FlagEnum, typename FlagType = typename std::underlying_type<FlagEnum>::type>
    struct Flags {
        typedef FlagType ValueType;
        FlagType value;

        explicit constexpr Flags() : value(FlagType(0)) {}
        explicit /*constexpr*/ Flags(FlagType val) : value(val) {}
        explicit constexpr Flags(std::initializer_list<FlagEnum> vals) : value(0) {
            for(auto val: vals) {
                value |= static_cast<FlagType>(val);
            }
        }

        explicit operator bool() const {
            return value != 0;
        }

        Flags& operator=(FlagType val) {
            value = val;
            return *this;
        }

        /**
         * Check if flag is set
         * @param flag Flag to test
         * @return true iff set
         */
        bool isset(FlagEnum flag) const {
            return (value & static_cast<FlagType>(flag)) != 0;
        }

        /**
         * Check if flag is not set
         * @param flag Flag to test
         * @return true iff not set
         */
        bool isnset(FlagEnum flag) const {
            return (value & static_cast<FlagType>(flag)) == 0;
        }

        /**
         * Set flag
         * @param flag Flag to set
         */
        void set(FlagEnum flag) {
            value |= static_cast<FlagType>(flag);
        }

        /**
         * Clear flag
         * @param flag Flag to clear
         */
        void unset(FlagEnum flag) {
            value &= ~static_cast<FlagType>(flag);
        }
    };

    template<typename E, typename T>
    Flags<E,T> operator|(Flags<E,T> flags, E const& flag) {
        flags.set(flag);
        return flags;
    }

    template<typename E, typename T>
    Flags<E,T> operator|(Flags<E,T> flags, Flags<E,T> const& other) {
        flags.value |= other.value;
        return flags;
    }

    template<typename Flag, typename Type>
    inline std::ostream& operator<<(std::ostream& os, Flags<Flag, Type> flags) {
        if (!flags.value) {
            return os << "0";
        }
        bool first = true;
        for(unsigned i = 0; i < sizeof(Type)*8; ++i) {
            if ((flags.value >> i) & 0x01) {
                if (!first) {
                    os << " | ";
                }
                os << static_cast<Flag>(0x01 << i);
                first = false;
            }
        }
        return os;
    }
}

