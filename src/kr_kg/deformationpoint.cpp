#include "deformationpoint.h"
#include <algorithm>
#include <iostream>

 
DeformationPoint::DeformationPoint()
    : position(0.0f, 0.0f, 0.0f)
    , displacement(0.0f, 0.0f, 0.0f)
    , influenceRadius(1.0f)
    , strength(1.0f)
    , attenuationFunction(GAUSSIAN)
    , enabled(true)
{
}

DeformationPoint::DeformationPoint(const Point3D& position, const Point3D& displacement,
                                   float influenceRadius, AttenuationFunction function)
    : position(position)
    , displacement(displacement)
    , influenceRadius(influenceRadius)
    , strength(1.0f)
    , attenuationFunction(function)
    , enabled(true)
{
    clampParameters();
}

DeformationPoint::DeformationPoint(const DeformationPoint& other)
    : position(other.position)
    , displacement(other.displacement)
    , influenceRadius(other.influenceRadius)
    , strength(other.strength)
    , attenuationFunction(other.attenuationFunction)
    , enabled(other.enabled)
{
}

 
DeformationPoint& DeformationPoint::operator=(const DeformationPoint& other)
{
    if (this != &other) {
        position = other.position;
        displacement = other.displacement;
        influenceRadius = other.influenceRadius;
        strength = other.strength;
        attenuationFunction = other.attenuationFunction;
        enabled = other.enabled;
    }
    return *this;
}

 
float DeformationPoint::calculateWeight(float distance) const
{
    if (!enabled || distance < 0.0f) {
        return 0.0f;
    }

    if (distance > getMaxInfluenceDistance()) {
        return 0.0f;
    }

    float weight = 0.0f;

    switch (attenuationFunction) {
    case GAUSSIAN:
        weight = gaussianAttenuation(distance, influenceRadius);
        break;
    case EXPONENTIAL:
        weight = exponentialAttenuation(distance, influenceRadius);
        break;
    case LINEAR:
        weight = linearAttenuation(distance, influenceRadius);
        break;
    case QUADRATIC:
        weight = quadraticAttenuation(distance, influenceRadius);
        break;
    default:
        weight = gaussianAttenuation(distance, influenceRadius);
        break;
    }

    return weight * strength;
}

float DeformationPoint::calculateWeight(const Point3D& point) const
{
    float distance = getDistanceTo(point);
    return calculateWeight(distance);
}

bool DeformationPoint::isPointInInfluence(const Point3D& point) const
{
    if (!enabled) {
        return false;
    }

    float distance = getDistanceTo(point);
    return distance <= getMaxInfluenceDistance();
}

Point3D DeformationPoint::calculateDisplacementAt(const Point3D& point) const
{
    float weight = calculateWeight(point);
    return Point3D(displacement.x * weight,
                   displacement.y * weight,
                   displacement.z * weight);
}

 
void DeformationPoint::setPosition(const Point3D& pos)
{
    position = pos;
}

void DeformationPoint::setDisplacement(const Point3D& disp)
{
    displacement = disp;
}

void DeformationPoint::setInfluenceRadius(float radius)
{
    if (validateRadius(radius)) {
        influenceRadius = radius;
    } else {
        influenceRadius = std::clamp(radius, MIN_RADIUS, MAX_RADIUS);
    }
}

void DeformationPoint::setStrength(float str)
{
    if (validateStrength(str)) {
        strength = str;
    } else {
        strength = std::clamp(str, MIN_STRENGTH, MAX_STRENGTH);
    }
}

void DeformationPoint::setAttenuationFunction(AttenuationFunction function)
{
    attenuationFunction = function;
}

void DeformationPoint::setEnabled(bool enable)
{
    enabled = enable;
}

 
float DeformationPoint::getDistanceTo(const Point3D& point) const
{
    float dx = point.x - position.x;
    float dy = point.y - position.y;
    float dz = point.z - position.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float DeformationPoint::getMaxInfluenceDistance() const
{
    switch (attenuationFunction) {
    case GAUSSIAN:
        return influenceRadius * 3.0f;  
    case EXPONENTIAL:
        return influenceRadius * 5.0f;  
    case LINEAR:
    case QUADRATIC:
        return influenceRadius;
    default:
        return influenceRadius * 3.0f;
    }
}

bool DeformationPoint::isValid() const
{
    return validateRadius(influenceRadius) &&
           validateStrength(strength) &&
           std::isfinite(position.x) && std::isfinite(position.y) && std::isfinite(position.z) &&
           std::isfinite(displacement.x) && std::isfinite(displacement.y) && std::isfinite(displacement.z);
}

void DeformationPoint::reset()
{
    position = Point3D(0.0f, 0.0f, 0.0f);
    displacement = Point3D(0.0f, 0.0f, 0.0f);
    influenceRadius = 1.0f;
    strength = 1.0f;
    attenuationFunction = GAUSSIAN;
    enabled = true;
}

void DeformationPoint::printInfo() const
{
    std::cout << "DeformationPoint Info:" << std::endl;
    std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    std::cout << "  Displacement: (" << displacement.x << ", " << displacement.y << ", " << displacement.z << ")" << std::endl;
    std::cout << "  Influence Radius: " << influenceRadius << std::endl;
    std::cout << "  Strength: " << strength << std::endl;
    std::cout << "  Function: " << static_cast<int>(attenuationFunction) << std::endl;
    std::cout << "  Enabled: " << (enabled ? "true" : "false") << std::endl;
    std::cout << "  Valid: " << (isValid() ? "true" : "false") << std::endl;
}

float DeformationPoint::gaussianAttenuation(float distance, float sigma)
{
    if (sigma <= EPSILON) {
        return (distance <= EPSILON) ? 1.0f : 0.0f;
    }

     
    float normalized = distance / (sigma * 0.5f);   
    float result = std::exp(-0.5f * normalized * normalized);

    return result;
}

float DeformationPoint::exponentialAttenuation(float distance, float sigma)
{
    if (sigma <= EPSILON) {
        return (distance <= EPSILON) ? 1.0f : 0.0f;
    }

    return std::exp(-distance / sigma);
}

float DeformationPoint::linearAttenuation(float distance, float radius)
{
    if (radius <= EPSILON) {
        return (distance <= EPSILON) ? 1.0f : 0.0f;
    }

    if (distance >= radius) {
        return 0.0f;
    }

    return 1.0f - (distance / radius);
}

float DeformationPoint::quadraticAttenuation(float distance, float radius)
{
    if (radius <= EPSILON) {
        return (distance <= EPSILON) ? 1.0f : 0.0f;
    }

    if (distance >= radius) {
        return 0.0f;
    }

    float normalized = distance / radius;
    return 1.0f - normalized * normalized;
}

 
bool DeformationPoint::validateRadius(float radius) const
{
    return radius >= MIN_RADIUS && radius <= MAX_RADIUS && std::isfinite(radius);
}

bool DeformationPoint::validateStrength(float str) const
{
    return str >= MIN_STRENGTH && str <= MAX_STRENGTH && std::isfinite(str);
}

void DeformationPoint::clampParameters()
{
    influenceRadius = std::clamp(influenceRadius, MIN_RADIUS, MAX_RADIUS);
    strength = std::clamp(strength, MIN_STRENGTH, MAX_STRENGTH);

     
    if (!std::isfinite(position.x)) position.x = 0.0f;
    if (!std::isfinite(position.y)) position.y = 0.0f;
    if (!std::isfinite(position.z)) position.z = 0.0f;

    if (!std::isfinite(displacement.x)) displacement.x = 0.0f;
    if (!std::isfinite(displacement.y)) displacement.y = 0.0f;
    if (!std::isfinite(displacement.z)) displacement.z = 0.0f;
}

 
float DeformationPoint::calculateInfluenceWeight(float distance) const
{
    if (!enabled || distance < 0.0f) {
        return 0.0f;
    }

     
    if (distance < EPSILON) {
        return strength;
    }

    float weight = 0.0f;

    switch (attenuationFunction) {
    case GAUSSIAN:
        weight = gaussianAttenuation(distance, influenceRadius);
        break;
    case EXPONENTIAL:
        weight = exponentialAttenuation(distance, influenceRadius);
        break;
    case LINEAR:
        weight = linearAttenuation(distance, influenceRadius);
        break;
    case QUADRATIC:
        weight = quadraticAttenuation(distance, influenceRadius);
        break;
    default:
        weight = 0.0f;
        break;
    }

    return weight * strength;
}
