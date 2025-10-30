#ifndef DEFORMATIONPOINT_H
#define DEFORMATIONPOINT_H

#include "point3d.h"
#include <cmath>

class DeformationPoint
{
public:
     
    enum AttenuationFunction {
        GAUSSIAN,
        EXPONENTIAL,
        LINEAR,
        QUADRATIC
    };

     
    DeformationPoint();
    DeformationPoint(const Point3D& position, const Point3D& displacement,
                     float influenceRadius, AttenuationFunction function = GAUSSIAN);
    DeformationPoint(const DeformationPoint& other);

     
    DeformationPoint& operator=(const DeformationPoint& other);

     
    ~DeformationPoint() = default;

     
    float calculateWeight(float distance) const;
    float calculateWeight(const Point3D& point) const;
    bool isPointInInfluence(const Point3D& point) const;
    Point3D calculateDisplacementAt(const Point3D& point) const;

     
    const Point3D& getPosition() const { return position; }
    const Point3D& getDisplacement() const { return displacement; }
    float getInfluenceRadius() const { return influenceRadius; }
    float getStrength() const { return strength; }
    AttenuationFunction getAttenuationFunction() const { return attenuationFunction; }
    bool isEnabled() const { return enabled; }

     
    void setPosition(const Point3D& pos);
    void setDisplacement(const Point3D& disp);
    void setInfluenceRadius(float radius);
    void setStrength(float str);
    void setAttenuationFunction(AttenuationFunction function);
    void setEnabled(bool enable);

     
    float getDistanceTo(const Point3D& point) const;
    float getMaxInfluenceDistance() const;
    bool isValid() const;
    void reset();

     
    void printInfo() const;

     
    static float gaussianAttenuation(float distance, float sigma);
    static float exponentialAttenuation(float distance, float sigma);
    static float linearAttenuation(float distance, float radius);
    static float quadraticAttenuation(float distance, float radius);

    float calculateInfluenceWeight(float distance) const;

private:
    Point3D position;                     
    Point3D displacement;                 
    float influenceRadius;                
    float strength;                       
    AttenuationFunction attenuationFunction;  
    bool enabled;                         

     
    static constexpr float MIN_RADIUS = 0.1f;
    static constexpr float MAX_RADIUS = 5000.0f;
    static constexpr float MIN_STRENGTH = 0.0f;
    static constexpr float MAX_STRENGTH = 10.0f;
    static constexpr float EPSILON = 1e-6f;

     
    bool validateRadius(float radius) const;
    bool validateStrength(float str) const;
    void clampParameters();
};

#endif  
