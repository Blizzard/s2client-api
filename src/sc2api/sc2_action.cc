#include "sc2api/sc2_action.h"

#include <iostream>
#include <cassert>

#include "s2clientprotocol/sc2api.pb.h"

namespace sc2 {

ActionRaw::ActionRaw() :
    ability_id(0),
    target_type(TargetNone),
    target_tag(NullTag) {
}

}