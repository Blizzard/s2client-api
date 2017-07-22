#pragma once

#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_common.h"
#include "sc2api/sc2_interfaces.h"

#include <vector>

namespace sc2 {

namespace search {

// Clusters units within some distance of each other and returns a list of them and their center of mass.
std::vector<std::pair<Point3D, std::vector<Unit> > > Cluster(const Units& units, float distance_apart);

struct ExpansionParameters {
    // Some nice parameters that generally work but may require tuning for certain maps.
    ExpansionParameters() :
        radiuses_({ 6.4f, 5.3f }),
        circle_step_size_(0.5f),
        cluster_distance_(15.0f),
        debug_(nullptr) {
    }

    // The various radius to check at from the center of an expansion.
    std::vector<float> radiuses_;

    // With what granularity to step the circumference of the circle.
    float circle_step_size_;

    // With what distance to cluster mineral/vespene in, this will be used for center of mass calulcation.
    float cluster_distance_;

    // If filled out CalculateExpansionLocations will render spheres to show what it calculated.
    DebugInterface* debug_;
};

// Calculates expansion locations, this call can take on the order of 100ms since it makes blocking queries to SC2 so call it once and cache the reults.
std::vector<Point3D> CalculateExpansionLocations(const ObservationInterface* observation, QueryInterface* query, ExpansionParameters parameters=ExpansionParameters());

}

}
