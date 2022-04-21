#pragma once
#include "Geometries/CubicSpline.h"
#include "RoadMark.h"
#include "XmlNode.h"

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <stdexcept>

namespace odr
{

struct HeightOffset
{
    HeightOffset(double inner, double outer);

    double inner = 0;
    double outer = 0;
};

struct LaneKey
{
    LaneKey(std::string road_id, double lanesection_s0, int lane_id);

    std::string road_id = "";
    double      lanesection_s0 = 0;
    int         lane_id = 0;
};

struct Lane : public XmlNode
{
    Lane(std::string road_id, double lanesection_s0, int id, bool level, std::string type);

    std::vector<RoadMark> get_roadmarks(double s_start, double s_end) const;

    LaneKey     key;
    int         id;
    bool        level = false;
    int         predecessor = 0;
    int         successor = 0;
    std::string type = "";

    CubicSpline lane_width;
    CubicSpline outer_border;
    CubicSpline inner_border;

    std::map<double, HeightOffset> s_to_height_offset;
    std::set<RoadMarkGroup>        roadmark_groups;
};

} // namespace odr

template<>
struct std::hash<odr::LaneKey>
{
    std::size_t operator()(const odr::LaneKey& key) const
    {
        return ((std::hash<string>()(key.road_id) ^ (std::hash<double>()(key.lanesection_s0) << 1)) >> 1) ^ (std::hash<int>()(key.lane_id) << 1);
    }
};

template<>
struct std::equal_to<odr::LaneKey>
{
    bool operator()(const odr::LaneKey& lhs, const odr::LaneKey& rhs) const
    {
        return (lhs.road_id == rhs.road_id) && (lhs.lanesection_s0 == rhs.lanesection_s0) && (lhs.lane_id == rhs.lane_id);
    }
};