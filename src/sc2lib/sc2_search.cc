#include "sc2lib/sc2_search.h"

namespace sc2 {

namespace search {

static const float PI = 3.1415927f;

size_t CalculateQueries(float radius, float step_size, const Point2D& center, std::vector<QueryInterface::PlacementQuery>& queries) {
    Point2D current_grid, previous_grid(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    size_t valid_queries = 0;
    // Find a buildable location on the circumference of the sphere
    float loc = 0.0f;
    while (loc < 360.0f) {
        Point2D point = Point2D(
            (radius * std::cos((loc * PI) / 180.0f)) + center.x,
            (radius * std::sin((loc * PI) / 180.0f)) + center.y);

        QueryInterface::PlacementQuery query(ABILITY_ID::BUILD_COMMANDCENTER, point);

        current_grid = Point2D(floor(point.x), floor(point.y));

        if (previous_grid != current_grid) {
            queries.push_back(query);
            ++valid_queries;
        }

        previous_grid = current_grid;
        loc += step_size;
    }

    return valid_queries;
}

std::vector<std::pair<Point3D, std::vector<Unit> > > Cluster(const Units& units, float distance_apart) {
    float squared_distance_apart = distance_apart * distance_apart;
    std::vector<std::pair<Point3D, std::vector<Unit> > > clusters;
    for (size_t i = 0, e = units.size(); i < e; ++i) {
        const Unit& u = units[i];

        float distance = std::numeric_limits<float>::max();
        std::pair<Point3D, std::vector<Unit> >* target_cluster = nullptr;
        // Find the cluster this mineral patch is closest to.
        for (auto& cluster : clusters) {
            float d = DistanceSquared3D(u.pos, cluster.first);
            if (d < distance) {
                distance = d;
                target_cluster = &cluster;
            }
        }

        // If the target cluster is some distance away don't use it.
        if (distance > squared_distance_apart) {
            clusters.push_back(std::pair<Point3D, std::vector<Unit> >(u.pos, std::vector<Unit>{u}));
            continue;
        }

        // Otherwise append to that cluster and update it's center of mass.
        target_cluster->second.push_back(u);
        size_t size = target_cluster->second.size();
        target_cluster->first = ((target_cluster->first * (float(size) - 1)) + u.pos) / float(size);
    }

    return clusters;
}


std::vector<Point3D> CalculateExpansionLocations(const ObservationInterface* observation, QueryInterface* query, ExpansionParameters parameters) {
    Units resources = observation->GetUnits(
        [](const Unit& unit) {
            return unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD || unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
                unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD || unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750 ||
                unit.unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER || unit.unit_type == UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER;
        }
    );

    std::vector<Point3D> expansion_locations;
    std::vector<std::pair<Point3D, std::vector<Unit> > > clusters = Cluster(resources, parameters.cluster_distance_);

    std::vector<size_t> query_size;
    std::vector<QueryInterface::PlacementQuery> queries;
    for (size_t i = 0; i < clusters.size(); ++i) {
        std::pair<Point3D, std::vector<Unit> >& cluster = clusters[i];
        if (parameters.debug_) {
            for (auto r : parameters.radiuses_) {
                parameters.debug_->DebugSphereOut(cluster.first, r, Colors::Green);
            }
        }

        // Get the required queries for this cluster.
        size_t query_count = 0;
        for (auto r : parameters.radiuses_) {
            query_count += CalculateQueries(r, parameters.circle_step_size_, cluster.first, queries);
        }

        query_size.push_back(query_count);
    }

    std::vector<bool> results = query->Placement(queries);
    size_t start_index = 0;
    for (int i = 0; i < clusters.size(); ++i) {
        std::pair<Point3D, std::vector<Unit> >& cluster = clusters[i];
        float distance = std::numeric_limits<float>::max();
        Point2D closest;

        // For each query for the cluster minimum distance location that is valid.
        for (size_t j = start_index, e = start_index + query_size[i]; j < e; ++j) {
            if (!results[j]) {
                continue;
            }

            Point2D& p = queries[j].target_pos;

            float d = Distance2D(p, cluster.first);
            if (d < distance) {
                distance = d;
                closest = p;
            }
        }

        Point3D expansion(closest.x, closest.y, cluster.second.begin()->pos.z);

        if (parameters.debug_) {
            parameters.debug_->DebugSphereOut(expansion, 0.35f, Colors::Red);
        }

        expansion_locations.push_back(expansion);
        start_index += query_size[i];
    }

    return expansion_locations;
}


}

}
