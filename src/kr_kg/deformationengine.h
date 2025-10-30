#ifndef DEFORMATIONENGINE_H
#define DEFORMATIONENGINE_H

#include "deformationpoint.h"
#include "tube.h"
#include "point3d.h"
#include <vector>
#include <memory>

class DeformationEngine
{
public:
     
    DeformationEngine();
    ~DeformationEngine() = default;

     
    bool applyDeformation(Tube& tube);
    void addDeformationPoint(const DeformationPoint& point);
    void removeDeformationPoint(int index);
    void clearDeformationPoints();

     
    size_t getDeformationPointCount() const;
    DeformationPoint& getDeformationPoint(int index);
    const DeformationPoint& getDeformationPoint(int index) const;
    bool hasActiveDeformations() const;

     
    void setSmoothingEnabled(bool enabled) { smoothingEnabled = enabled; }
    bool isSmoothingEnabled() const { return smoothingEnabled; }

    void setSmoothingFactor(float factor);
    float getSmoothingFactor() const { return smoothingFactor; }

    void setMaxDeformationMagnitude(float magnitude);
    float getMaxDeformationMagnitude() const { return maxDeformationMagnitude; }

     
    bool isValid() const;
    void reset();

    std::vector<Point3D> applyDeformationToCurve(const std::vector<Point3D>& originalCurve);

     
    std::vector<Point3D> applyCurveDeformationWithSinglePoint(
        const std::vector<Point3D>& originalCurve,
        const Point3D& sourcePoint,
        const Point3D& targetPoint,
        float influenceRadius = 50.0f,
        DeformationPoint::AttenuationFunction function = DeformationPoint::GAUSSIAN
        );

     
    DeformationPoint createDeformationFromPoints(
        const Point3D& sourcePoint,
        const Point3D& targetPoint,
        float influenceRadius = 50.0f,
        DeformationPoint::AttenuationFunction function = DeformationPoint::GAUSSIAN
        ) const;

     
    bool isValidCurve(const std::vector<Point3D>& curve) const;

     
    Point3D interpolatePointOnCurve(const std::vector<Point3D>& curve, float zCoord) const;

    bool applyDeformationToTube(Tube& tube, int& currentVersionId);

private:
    std::vector<DeformationPoint> deformationPoints;

     
    bool smoothingEnabled;
    float smoothingFactor;
    float maxDeformationMagnitude;

     
    static constexpr float DEFAULT_SMOOTHING_FACTOR = 0.5f;
    static constexpr float MAX_DEFORMATION_MAGNITUDE = 10.0f;
    static constexpr float MIN_SMOOTHING_FACTOR = 0.0f;
    static constexpr float MAX_SMOOTHING_FACTOR = 1.0f;

     
    Point3D calculateNewCenterPosition(const Point3D& originalCenter) const;
    Point3D calculateTangentVector(int sectionIndex, const std::vector<Point3D>& newCenters) const;
    void updateSectionOrientation(Section& section, const Point3D& newCenter, const Point3D& tangent);

     
    std::vector<Point3D> applySmoothingToCenters(const std::vector<Point3D>& centers) const;
    Point3D smoothPoint(int index, const std::vector<Point3D>& points) const;

     
    bool validateDeformationMagnitude(const Point3D& displacement) const;
    void clampDeformation(Point3D& displacement) const;

     
    bool validateParameters() const;
    void normalizeVector(Point3D& vector) const;
    float vectorLength(const Point3D& vector) const;
    Point3D crossProduct(const Point3D& a, const Point3D& b) const;
    float dotProduct(const Point3D& a, const Point3D& b) const;

    std::vector<Point3D> interpolateMorePoints(const std::vector<Point3D>& curve, int targetPointCount) const;

    Point3D findNewCenterPosition(const Point3D& oldCenter, const std::vector<Point3D>& deformedCurve) const;
    void updateSectionPoints(Section& section,
                                                const Point3D& oldCenter,
                                                const Point3D& newCenter,
                                                const Point3D& tangent);

    Point3D calculateTangentAtPoint(const std::vector<Point3D>& curve, size_t index) const;

    void calculateOrthonormalBasis(const Point3D& tangent, Point3D& normal, Point3D& binormal);

    Point3D getTangentAtZ(const std::vector<Point3D>& deformedCurve, float zCoord) const;
};

#endif  
