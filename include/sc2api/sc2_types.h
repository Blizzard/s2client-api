/*! \file sc2_types.h
    \brief Common types used by this API.

Any basic types used throughout are found here.
*/

#pragma once

#include <stdint.h>
#include <string>

namespace sc2 {

//! Template class for defining common game enum types.
//! The template parameter is the enum. This class allows for seamless conversion between enum types and integers,
//! while maintaining strong typing. This means, for example, that a unit type ID can be converted back and forth
//! from an integer, but can't be used when another type ID, e.g., an ability ID, is required as a parameter.
template<class T>
class SC2Type {
public:
    //! Default constructor.
    SC2Type() :
        type_id_(0) {
    }
    //! Construct from an integer, corresponds to the enum value.
    SC2Type(uint32_t type_id) :
        type_id_(type_id) {
    }
    //! Construct from the enum.
    SC2Type(T type_id) :
        type_id_(static_cast<uint32_t>(type_id)) {
    }

    //! Test equivalence.
    //!< \return 'true' if the values are equal.
    bool operator ==(SC2Type<T> type_id) const { return type_id_ == type_id.type_id_; }
    bool operator ==(T type_id) const { return type_id_ == static_cast<uint32_t>(type_id); }
    bool operator ==(uint32_t type_id) const { return type_id_ == type_id; }
    bool operator ==(int type_id) const { return static_cast<uint32_t>(type_id_) == type_id; }

    //! Test non-equivalence.
    //!< \return 'true' if the values are not equal.
    bool operator !=(SC2Type<T> type_id) const { return type_id_ != type_id.type_id_; }
    bool operator !=(T type_id) const { return type_id_ != static_cast<uint32_t>(type_id); }
    bool operator !=(uint32_t type_id) const { return type_id_ != type_id; }
    bool operator !=(int type_id) const { return static_cast<uint32_t>(type_id_) != type_id; }

	//! Test comparison.
	bool operator<(const SC2Type<T>& other) const { return type_id_ < other.type_id_; }

    //! Cast to integer.
    operator uint32_t() const { return type_id_; }
    //! Cast to enum type.
    operator T() const { return static_cast<T>(type_id_); }

    //! Determines if the value contained is valid.
    //!< \return 'true' if the value is valid.
    bool IsValid() const { return type_id_ != 0; }

    //! String of the integer value.
    //!< \return The string of the value.
    std::string to_string() const { return std::to_string(type_id_); }

    //! Explicit conversion to the enum type.
    //!< \return The enum.
    T ToType() const { return static_cast<T>(type_id_); }

private:
    uint32_t type_id_;
};

}