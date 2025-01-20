#include "OpenDriveMap.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <iostream>

TEST_CASE("basic map check", "[xodr]")
{
    odr::OpenDriveMap odr_map("test.xodr");
    REQUIRE(odr_map.xml_parse_result);
    REQUIRE(!odr_map.get_roads().empty());

    for (const odr::Road& road : odr_map.get_roads())
    {
        INFO("road: " << road.id << ", length: " << road.length);
        REQUIRE(road.length >= 0.0);
        REQUIRE(!road.s_to_lanesection.empty());
        for (const odr::LaneSection& lanesection : road.get_lanesections())
        {
            const double s_start = lanesection.s0;
            const double s_end = road.get_lanesection_end(lanesection);
            REQUIRE(s_start >= 0.0);
            REQUIRE(s_end > s_start);
            for (const odr::Lane& lane : lanesection.get_lanes())
            {
                std::vector<odr::RoadMark> roadmarks = lane.get_roadmarks(s_start, s_end);
            }
        }
    }
}

TEST_CASE("routing check", "[routing]")
{
    odr::OpenDriveMap odr_map("test.xodr");

    const odr::RoutingGraph graph = odr_map.get_routing_graph();

    std::vector<odr::LaneKey> path, expected_path;
    const odr::LaneKey        posA("43", 0.0, 1);
    const odr::LaneKey        posB("41", 0.0, 1);
    const odr::LaneKey        posC("13", 0.0, -1);
    const odr::LaneKey        posNaN("no-exists", 0.0, 1);

    path = graph.shortest_path(posA, posB);
    REQUIRE(path.size() == 15);
    REQUIRE(path.front() == posA);
    REQUIRE(path.back() == posB);

    path = graph.shortest_path(posA, posA);
    REQUIRE(path.size() == 1);
    REQUIRE(path.front() == posA);

    path = graph.shortest_path(posC, posB);
    expected_path = {posC,
                     odr::LaneKey("405", 0.0, -1),
                     odr::LaneKey("405", 14.0521019230889283591068306122906506061553955078125, -1),
                     odr::LaneKey("16", 0.0, -1),
                     odr::LaneKey("17", 0.0, -1),
                     odr::LaneKey("18", 0.0, -1),
                     odr::LaneKey("747", 0.0, -1),
                     odr::LaneKey("747", 1.76237248299736393164494074881076812744140625, -1),
                     posB};
    REQUIRE_THAT(path, Catch::Matchers::Equals(expected_path));
}