#include "deformationengine.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <qdebug.h>
#include <qlogging.h>

 
DeformationEngine::DeformationEngine()
    : smoothingEnabled(true)
    , smoothingFactor(DEFAULT_SMOOTHING_FACTOR)
    , maxDeformationMagnitude(MAX_DEFORMATION_MAGNITUDE)
{
}

 
bool DeformationEngine::applyDeformation(Tube& tube)
{
    if (!isValid() || tube.getSectionCount() < 2) {
        return false;
    }

     
    if (!hasActiveDeformations()) {
        return true;
    }

    try {
         
        std::vector<Point3D> originalCenters;
        std::vector<Point3D> newCenters;

        for (size_t i = 0; i < tube.getSectionCount(); ++i) {
            const Section& section = tube.getSection(static_cast<int>(i + 1));
            Point3D center = section.getCenter();
            originalCenters.push_back(center);

             
            Point3D newCenter = calculateNewCenterPosition(center);
            newCenters.push_back(newCenter);
        }

         
        if (smoothingEnabled) {
            newCenters = applySmoothingToCenters(newCenters);
        }

         
        for (size_t i = 0; i < tube.getSectionCount(); ++i) {
            Section& section = tube.getSection(static_cast<int>(i + 1));

             
            Point3D tangent = calculateTangentVector(static_cast<int>(i), newCenters);

             
            updateSectionOrientation(section, newCenters[i], tangent);
        }

         
        tube.rebuildAllSegments();

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in DeformationEngine::applyDeformation: " << e.what() << std::endl;
        return false;
    }
}

void DeformationEngine::addDeformationPoint(const DeformationPoint& point)
{
    if (point.isValid()) {
        deformationPoints.push_back(point);
    }
}

void DeformationEngine::removeDeformationPoint(int index)
{
    if (index >= 0 && index < static_cast<int>(deformationPoints.size())) {
        deformationPoints.erase(deformationPoints.begin() + index);
    }
}

void DeformationEngine::clearDeformationPoints()
{
    deformationPoints.clear();
}

 
size_t DeformationEngine::getDeformationPointCount() const
{
    return deformationPoints.size();
}

DeformationPoint& DeformationEngine::getDeformationPoint(int index)
{
    return deformationPoints.at(index);
}

const DeformationPoint& DeformationEngine::getDeformationPoint(int index) const
{
    return deformationPoints.at(index);
}

bool DeformationEngine::hasActiveDeformations() const
{
    for (const auto& point : deformationPoints) {
        if (point.isEnabled() && point.isValid()) {
            return true;
        }
    }
    return false;
}

 
void DeformationEngine::setSmoothingFactor(float factor)
{
    smoothingFactor = std::clamp(factor, MIN_SMOOTHING_FACTOR, MAX_SMOOTHING_FACTOR);
}

void DeformationEngine::setMaxDeformationMagnitude(float magnitude)
{
    if (magnitude > 0.0f && std::isfinite(magnitude)) {
        maxDeformationMagnitude = magnitude;
    }
}

 
bool DeformationEngine::isValid() const
{
    return validateParameters();
}

void DeformationEngine::reset()
{
    clearDeformationPoints();
    smoothingEnabled = true;
    smoothingFactor = DEFAULT_SMOOTHING_FACTOR;
    maxDeformationMagnitude = MAX_DEFORMATION_MAGNITUDE;
}

 
Point3D DeformationEngine::calculateNewCenterPosition(const Point3D& originalCenter) const
{
    Point3D totalDisplacement(0.0f, 0.0f, 0.0f);

     
    for (const auto& deformPoint : deformationPoints) {
        if (!deformPoint.isEnabled() || !deformPoint.isValid()) {
            continue;
        }

         
        if (deformPoint.isPointInInfluence(originalCenter)) {
            Point3D displacement = deformPoint.calculateDisplacementAt(originalCenter);
            totalDisplacement.x += displacement.x;
            totalDisplacement.y += displacement.y;
            totalDisplacement.z += displacement.z;
        }
    }

     
    clampDeformation(totalDisplacement);

    return Point3D(originalCenter.x + totalDisplacement.x,
                   originalCenter.y + totalDisplacement.y,
                   originalCenter.z + totalDisplacement.z);
}

Point3D DeformationEngine::calculateTangentVector(int sectionIndex, const std::vector<Point3D>& newCenters) const
{
    if (newCenters.empty()) {
        return Point3D(0.0f, 0.0f, 1.0f);  
    }

    Point3D tangent(0.0f, 0.0f, 1.0f);

    if (sectionIndex == 0) {
         
        if (newCenters.size() > 1) {
            tangent.x = newCenters[1].x - newCenters[0].x;
            tangent.y = newCenters[1].y - newCenters[0].y;
            tangent.z = newCenters[1].z - newCenters[0].z;
        }
    }
    else if (sectionIndex == static_cast<int>(newCenters.size() - 1)) {
         
        tangent.x = newCenters[sectionIndex].x - newCenters[sectionIndex - 1].x;
        tangent.y = newCenters[sectionIndex].y - newCenters[sectionIndex - 1].y;
        tangent.z = newCenters[sectionIndex].z - newCenters[sectionIndex - 1].z;
    }
    else {
         
        tangent.x = newCenters[sectionIndex + 1].x - newCenters[sectionIndex - 1].x;
        tangent.y = newCenters[sectionIndex + 1].y - newCenters[sectionIndex - 1].y;
        tangent.z = newCenters[sectionIndex + 1].z - newCenters[sectionIndex - 1].z;
    }

    normalizeVector(tangent);
    return tangent;
}

void DeformationEngine::updateSectionOrientation(Section& section, const Point3D& newCenter, const Point3D& tangent)
{
     
    Point3D currentCenter = section.getCenter();
    Point3D centerDisplacement(newCenter.x - currentCenter.x,
                               newCenter.y - currentCenter.y,
                               newCenter.z - currentCenter.z);

     
    section.translate(centerDisplacement);

     
     
}

 
std::vector<Point3D> DeformationEngine::applySmoothingToCenters(const std::vector<Point3D>& centers) const
{
    if (centers.size() < 3 || smoothingFactor <= 0.0f) {
        return centers;  
    }

    std::vector<Point3D> smoothedCenters = centers;

     
    for (size_t i = 1; i < centers.size() - 1; ++i) {
        Point3D smoothed = smoothPoint(static_cast<int>(i), centers);

         
        smoothedCenters[i].x = centers[i].x + smoothingFactor * (smoothed.x - centers[i].x);
        smoothedCenters[i].y = centers[i].y + smoothingFactor * (smoothed.y - centers[i].y);
        smoothedCenters[i].z = centers[i].z + smoothingFactor * (smoothed.z - centers[i].z);
    }

    return smoothedCenters;
}

Point3D DeformationEngine::smoothPoint(int index, const std::vector<Point3D>& points) const
{
    if (index <= 0 || index >= static_cast<int>(points.size()) - 1) {
        return points[index];
    }

     
    const Point3D& prev = points[index - 1];
    const Point3D& curr = points[index];
    const Point3D& next = points[index + 1];

    return Point3D((prev.x + curr.x + next.x) / 3.0f,
                   (prev.y + curr.y + next.y) / 3.0f,
                   (prev.z + curr.z + next.z) / 3.0f);
}

 
bool DeformationEngine::validateDeformationMagnitude(const Point3D& displacement) const
{
    float magnitude = vectorLength(displacement);
    return magnitude <= maxDeformationMagnitude;
}

void DeformationEngine::clampDeformation(Point3D& displacement) const
{
    float magnitude = vectorLength(displacement);

    if (magnitude > maxDeformationMagnitude) {
        float scale = maxDeformationMagnitude / magnitude;
        displacement.x *= scale;
        displacement.y *= scale;
        displacement.z *= scale;
    }
}

bool DeformationEngine::validateParameters() const
{
    return smoothingFactor >= MIN_SMOOTHING_FACTOR &&
           smoothingFactor <= MAX_SMOOTHING_FACTOR &&
           maxDeformationMagnitude > 0.0f &&
           std::isfinite(smoothingFactor) &&
           std::isfinite(maxDeformationMagnitude);
}

 
void DeformationEngine::normalizeVector(Point3D& vector) const
{
    float length = vectorLength(vector);
    if (length > 1e-6f) {
        vector.x /= length;
        vector.y /= length;
        vector.z /= length;
    } else {
         
        vector.x = 0.0f;
        vector.y = 0.0f;
        vector.z = 1.0f;
    }
}

float DeformationEngine::vectorLength(const Point3D& vector) const
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

Point3D DeformationEngine::crossProduct(const Point3D& a, const Point3D& b) const
{
    return Point3D(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);
}

float DeformationEngine::dotProduct(const Point3D& a, const Point3D& b) const
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

 

 
std::vector<Point3D> DeformationEngine::applyCurveDeformationWithSinglePoint(
    const std::vector<Point3D>& originalCurve,
    const Point3D& sourcePoint,
    const Point3D& targetPoint,
    float influenceRadius,
    DeformationPoint::AttenuationFunction function)
{
    if (!isValidCurve(originalCurve)) {
        return originalCurve;
    }

    std::vector<Point3D> deformedCurve = originalCurve;

     
     
    float curveLength = 0.0f;
    for (size_t i = 1; i < originalCurve.size(); ++i) {
        float dx = originalCurve[i].x - originalCurve[i-1].x;
        float dy = originalCurve[i].y - originalCurve[i-1].y;
        float dz = originalCurve[i].z - originalCurve[i-1].z;
        curveLength += std::sqrt(dx*dx + dy*dy + dz*dz);
    }

     
     
    float adaptiveRadius = std::max(influenceRadius, curveLength * 0.3f);

     
    DeformationPoint tempDefPoint = createDeformationFromPoints(
        sourcePoint, targetPoint, adaptiveRadius, function);

    if (!tempDefPoint.isValid()) {
        return originalCurve;
    }

     
    for (size_t i = 0; i < deformedCurve.size(); ++i) {
        Point3D& currentPoint = deformedCurve[i];

         
        float dx = currentPoint.x - sourcePoint.x;
        float dy = currentPoint.y - sourcePoint.y;
        float dz = currentPoint.z - sourcePoint.z;
        float distance = std::sqrt(dx*dx + dy*dy + dz*dz);

         
        float weight = tempDefPoint.calculateWeight(distance);

        if (weight > 1e-6f) {
             
            Point3D displacement = tempDefPoint.getDisplacement();
            currentPoint.x += displacement.x * weight;
            currentPoint.y += displacement.y * weight;
            currentPoint.z += displacement.z * weight;
        }
    }

    return deformedCurve;
}

DeformationPoint DeformationEngine::createDeformationFromPoints(
    const Point3D& sourcePoint,
    const Point3D& targetPoint,
    float influenceRadius,
    DeformationPoint::AttenuationFunction function) const
{
     
    Point3D displacement(
        targetPoint.x - sourcePoint.x,
        targetPoint.y - sourcePoint.y,
        targetPoint.z - sourcePoint.z
        );

     
    DeformationPoint defPoint(sourcePoint, displacement, influenceRadius, function);

     
    defPoint.setStrength(1.0f);
    defPoint.setEnabled(true);

    return defPoint;
}

bool DeformationEngine::isValidCurve(const std::vector<Point3D>& curve) const
{
    if (curve.empty()) {
        return false;
    }

     
    for (const Point3D& point : curve) {
        if (!std::isfinite(point.x) || !std::isfinite(point.y) || !std::isfinite(point.z)) {
            return false;
        }
    }

     
    if (curve.size() < 2) {
        return false;
    }

    return true;
}

 
Point3D DeformationEngine::interpolatePointOnCurve(const std::vector<Point3D>& curve, float zCoord) const
{
    if (curve.empty()) {
        return Point3D(0.0f, 0.0f, zCoord);
    }

    if (curve.size() == 1) {
        return curve[0];
    }

     
    for (size_t i = 0; i < curve.size() - 1; ++i) {
        const Point3D& p1 = curve[i];
        const Point3D& p2 = curve[i + 1];

        float minZ = std::min(p1.z, p2.z);
        float maxZ = std::max(p1.z, p2.z);

        if (zCoord >= minZ && zCoord <= maxZ) {
             
            float deltaZ = maxZ - minZ;
            if (deltaZ < 1e-6f) {
                 
                return Point3D((p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f, zCoord);
            }

            float t = (zCoord - minZ) / deltaZ;

             
            const Point3D& startPoint = (p1.z <= p2.z) ? p1 : p2;
            const Point3D& endPoint = (p1.z <= p2.z) ? p2 : p1;

            return Point3D(
                startPoint.x + t * (endPoint.x - startPoint.x),
                startPoint.y + t * (endPoint.y - startPoint.y),
                zCoord
                );
        }
    }

     
    float minDist = std::abs(curve[0].z - zCoord);
    size_t closestIndex = 0;

    for (size_t i = 1; i < curve.size(); ++i) {
        float dist = std::abs(curve[i].z - zCoord);
        if (dist < minDist) {
            minDist = dist;
            closestIndex = i;
        }
    }

    return curve[closestIndex];
}

std::vector<Point3D> DeformationEngine::applyDeformationToCurve(const std::vector<Point3D>& originalCurve)
{
    if (!isValidCurve(originalCurve) || !hasActiveDeformations()) {
        return originalCurve;
    }

    std::vector<Point3D> workingCurve;
    if (originalCurve.size() < 10) {
        int pointsPerSegment = 100;
        int targetPoints = (originalCurve.size() - 1) * pointsPerSegment + 1;
        workingCurve = interpolateMorePoints(originalCurve, targetPoints);
    } else {
        workingCurve = originalCurve;
    }

    qDebug() << "Working curve size:" << workingCurve.size();

    for (const auto& defPoint : deformationPoints) {
        if (!defPoint.isEnabled()) {
            continue;
        }

        const Point3D& defPosition = defPoint.getPosition();
        const Point3D& defDisplacement = defPoint.getDisplacement();
        float radius = defPoint.getInfluenceRadius();
        float strength = defPoint.getStrength();

        qDebug() << "Applying deformation at:" << defPosition.x << defPosition.y << defPosition.z;
        qDebug() << "Displacement:" << defDisplacement.x << defDisplacement.y << defDisplacement.z;
        qDebug() << "Radius:" << radius << "Strength:" << strength;

        int pointsAffected = 0;

        for (size_t i = 0; i < workingCurve.size(); ++i) {
            Point3D& currentPoint = workingCurve[i];

            float dx = currentPoint.x - defPosition.x;
            float dy = currentPoint.y - defPosition.y;
            float dz = currentPoint.z - defPosition.z;
            float distance = std::sqrt(dx*dx + dy*dy + dz*dz);

            qDebug() << "Point" << i << "at (" << currentPoint.x << "," << currentPoint.y << "," << currentPoint.z
                     << ") distance:" << distance;

            float weight = defPoint.calculateWeight(distance);

            if (weight > 1e-6f) {
                pointsAffected++;

                Point3D oldPoint = currentPoint;
                currentPoint.x += defDisplacement.x * weight;
                currentPoint.y += defDisplacement.y * weight;
                currentPoint.z += defDisplacement.z * weight;
            }
        }

        qDebug() << "Total points affected:" << pointsAffected;
    }

    return workingCurve;
}

std::vector<Point3D> DeformationEngine::interpolateMorePoints(
    const std::vector<Point3D>& curve,
    int targetPointCount) const
{
    if (curve.size() < 2 || targetPointCount <= static_cast<int>(curve.size())) {
        return curve;
    }

    std::vector<Point3D> result;
    result.reserve(targetPointCount);

     
    std::vector<float> segmentLengths;
    float totalLength = 0.0f;

    for (size_t i = 0; i < curve.size() - 1; ++i) {
        float dx = curve[i+1].x - curve[i].x;
        float dy = curve[i+1].y - curve[i].y;
        float dz = curve[i+1].z - curve[i].z;
        float length = std::sqrt(dx*dx + dy*dy + dz*dz);
        segmentLengths.push_back(length);
        totalLength += length;
    }

     
    if (totalLength < 1e-6f) {
        qDebug() << "ERROR: Total curve length is too small:" << totalLength;
        return curve;   
    }

    float stepLength = totalLength / (targetPointCount - 1);

    result.push_back(curve[0]);

    for (int i = 1; i < targetPointCount - 1; ++i) {
        float targetLength = i * stepLength;

        float accumulatedLength = 0.0f;
        size_t currentSegment = 0;

        for (size_t j = 0; j < segmentLengths.size(); ++j) {
            if (accumulatedLength + segmentLengths[j] >= targetLength) {
                currentSegment = j;
                break;
            }
            accumulatedLength += segmentLengths[j];
        }

         
        if (segmentLengths[currentSegment] < 1e-6f) {
            qDebug() << "WARNING: Segment" << currentSegment << "has near-zero length";
            result.push_back(curve[currentSegment]);   
            continue;
        }

        float segmentProgress = (targetLength - accumulatedLength) / segmentLengths[currentSegment];

         
        segmentProgress = std::max(0.0f, std::min(1.0f, segmentProgress));

        const Point3D& p1 = curve[currentSegment];
        const Point3D& p2 = curve[currentSegment + 1];

        Point3D interpolatedPoint;
        interpolatedPoint.x = p1.x + segmentProgress * (p2.x - p1.x);
        interpolatedPoint.y = p1.y + segmentProgress * (p2.y - p1.y);
        interpolatedPoint.z = p1.z + segmentProgress * (p2.z - p1.z);

         
        if (std::isnan(interpolatedPoint.x) || std::isnan(interpolatedPoint.y) || std::isnan(interpolatedPoint.z)) {
            qDebug() << "ERROR: Interpolation produced NaN at point" << i;
            return curve;   
        }

        result.push_back(interpolatedPoint);
    }

    result.push_back(curve.back());

    qDebug() << "Interpolated" << curve.size() << "points to" << result.size() << "points";

    return result;
}

bool DeformationEngine::applyDeformationToTube(Tube& tube, int& currentVersionId)
{
    qDebug() << "DeformationEngine::applyDeformationToTube - Starting tube deformation";

    if (!isValid() || tube.getSectionCount() < 2) {
        qDebug() << "Invalid state or insufficient sections";
        return false;
    }

    if (!hasActiveDeformations()) {
        qDebug() << "No active deformations";
        return true;
    }

    try {
         
        std::vector<Point3D> originalCentersCurve;
        for (size_t i = 0; i < tube.getSectionCount(); ++i) {
            const Section& section = tube.getSection(static_cast<int>(i + 1));
            Point3D center = section.getCenter();
            originalCentersCurve.push_back(center);
        }

        qDebug() << "Original centers curve extracted, size:" << originalCentersCurve.size();

         
         
         
        std::vector<Point3D> deformedCentersCurve = applyDeformationToCurve(originalCentersCurve);

        if (deformedCentersCurve.empty()) {
            qDebug() << "Failed to deform centers curve";
            return false;
        }

        qDebug() << "Centers curve deformed successfully, size:" << deformedCentersCurve.size();

         
         
        std::vector<Point3D> newCentersForSections;
        std::vector<Point3D> tangentsForSections;
        newCentersForSections.reserve(tube.getSectionCount());
        tangentsForSections.reserve(tube.getSectionCount());

         
        if (deformedCentersCurve.size() != originalCentersCurve.size()) {
            qDebug() << "Curve was interpolated, extracting section centers by Z-coordinate";
            for (size_t i = 0; i < tube.getSectionCount(); ++i) {
                float targetZ = originalCentersCurve[i].z;
                Point3D newCenter = interpolatePointOnCurve(deformedCentersCurve, targetZ);
                newCentersForSections.push_back(newCenter);

                 
                Point3D tangent = getTangentAtZ(deformedCentersCurve, targetZ);
                tangentsForSections.push_back(tangent);
            }
        } else {
             
            qDebug() << "Using direct correspondence for centers";
            for (size_t i = 0; i < deformedCentersCurve.size(); ++i) {
                newCentersForSections.push_back(deformedCentersCurve[i]);
                Point3D tangent = calculateTangentAtPoint(deformedCentersCurve, i);
                tangentsForSections.push_back(tangent);
            }
        }

         
        for (size_t i = 0; i < tangentsForSections.size(); ++i) {
            qDebug() << "  Section" << (i + 1) << "tangent: ("
                     << tangentsForSections[i].x << ","
                     << tangentsForSections[i].y << ","
                     << tangentsForSections[i].z << ")";
        }

        qDebug() << "Tangents calculated for all sections";

         
        currentVersionId++;
        qDebug() << "Version incremented to:" << currentVersionId;

         
        for (size_t i = 0; i < tube.getSectionCount(); ++i) {
            Section& section = tube.getSection(static_cast<int>(i + 1));

            Point3D oldCenter = originalCentersCurve[i];
            Point3D newCenter = newCentersForSections[i];
            Point3D tangent = tangentsForSections[i];

            qDebug() << "Deforming section" << (i + 1)
                     << "from (" << oldCenter.x << "," << oldCenter.y << "," << oldCenter.z << ")"
                     << "to (" << newCenter.x << "," << newCenter.y << "," << newCenter.z << ")";

             
            updateSectionPoints(section, oldCenter, newCenter, tangent);
        }

        qDebug() << "All sections deformed successfully";

         
        qDebug() << "Updating segment geometry...";
        tube.updateSegmentGeometry();
        qDebug() << "Segment geometry updated successfully";

        return true;
    }
    catch (const std::exception& e) {
        qDebug() << "Error in DeformationEngine::applyDeformationToTube:" << e.what();
        return false;
    }
}

Point3D DeformationEngine::findNewCenterPosition(const Point3D& oldCenter,
                                                 const std::vector<Point3D>& deformedCurve) const
{
    float targetZ = oldCenter.z;

    return interpolatePointOnCurve(deformedCurve, targetZ);
}

void DeformationEngine::updateSectionPoints(Section& section,
                                            const Point3D& oldCenter,
                                            const Point3D& newCenter,
                                            const Point3D& tangent)
{
    qDebug() << "  updateSectionPoints: section with" << section.getPointCount() << "points";
    qDebug() << "  Old center: (" << oldCenter.x << "," << oldCenter.y << "," << oldCenter.z << ")";
    qDebug() << "  New center: (" << newCenter.x << "," << newCenter.y << "," << newCenter.z << ")";
    qDebug() << "  Tangent: (" << tangent.x << "," << tangent.y << "," << tangent.z << ")";

     
    Point3D displacement(
        newCenter.x - oldCenter.x,
        newCenter.y - oldCenter.y,
        newCenter.z - oldCenter.z
        );

    qDebug() << "  Displacement: (" << displacement.x << "," << displacement.y << "," << displacement.z << ")";

     
     
    Point3D normalizedTangent = tangent;
    normalizeVector(normalizedTangent);

     
    Point3D normal, binormal;
    calculateOrthonormalBasis(normalizedTangent, normal, binormal);

    qDebug() << "  Orthonormal basis:";
    qDebug() << "    Tangent: (" << normalizedTangent.x << "," << normalizedTangent.y << "," << normalizedTangent.z << ")";
    qDebug() << "    Normal: (" << normal.x << "," << normal.y << "," << normal.z << ")";
    qDebug() << "    Binormal: (" << binormal.x << "," << binormal.y << "," << binormal.z << ")";

     
     
    Point3D oldTangent(0.0f, 0.0f, 1.0f);
    Point3D oldNormal(1.0f, 0.0f, 0.0f);    
    Point3D oldBinormal(0.0f, 1.0f, 0.0f);  

     
    for (size_t i = 0; i < section.getPointCount(); ++i) {
        Point3D& point = section.getPoint(static_cast<int>(i + 1));

         
        float localX = point.x - oldCenter.x;
        float localY = point.y - oldCenter.y;
        float localZ = point.z - oldCenter.z;

         
         


         
         
        point.x = newCenter.x + localX * normal.x + localY * binormal.x + localZ * normalizedTangent.x;
        point.y = newCenter.y + localX * normal.y + localY * binormal.y + localZ * normalizedTangent.y;
        point.z = newCenter.z + localX * normal.z + localY * binormal.z + localZ * normalizedTangent.z;
    }

    qDebug() << "  Updated" << section.getPointCount() << "points with rotation";
}

Point3D DeformationEngine::calculateTangentAtPoint(
    const std::vector<Point3D>& curve,
    size_t pointIndex) const
{
    if (curve.empty() || pointIndex >= curve.size()) {
        return Point3D(0, 0, 1);   
    }

    Point3D tangent;

    if (pointIndex == 0) {
         
        tangent = curve[1] - curve[0];
    }
    else if (pointIndex == curve.size() - 1) {
         
        tangent = curve[pointIndex] - curve[pointIndex - 1];
    }
    else {
         
        tangent = curve[pointIndex + 1] - curve[pointIndex - 1];
    }

     
    float length = std::sqrt(tangent.x*tangent.x + tangent.y*tangent.y + tangent.z*tangent.z);
    if (length > 1e-6f) {
        tangent = tangent / length;
    } else {
        tangent = Point3D(0, 0, 1);   
    }

    return tangent;
}

void DeformationEngine::calculateOrthonormalBasis(const Point3D& tangent,
                                                  Point3D& normal,
                                                  Point3D& binormal)
{
    qDebug() << "calculateOrthonormalBasis: Input tangent = ("
             << tangent.x << "," << tangent.y << "," << tangent.z << ")";

     
    float tangentLength = vectorLength(tangent);
    if (std::abs(tangentLength - 1.0f) > 0.1f) {
        qDebug() << "WARNING: Tangent is not normalized, length =" << tangentLength;
    }

     
     
    Point3D helper;

     
    float tangentZComponent = std::abs(tangent.z);

    if (tangentZComponent < 0.9f) {
         
        helper = Point3D(0.0f, 0.0f, 1.0f);
        qDebug() << "calculateOrthonormalBasis: Using Z-axis as helper vector";
    }
    else {
         
        helper = Point3D(1.0f, 0.0f, 0.0f);
        qDebug() << "calculateOrthonormalBasis: Tangent parallel to Z, using X-axis as helper";
    }

     
     
    normal = crossProduct(tangent, helper);

     
    float normalLength = vectorLength(normal);
    if (normalLength < 1e-6f) {
        qDebug() << "ERROR: Normal vector is too small, using fallback";
         
        helper = Point3D(0.0f, 1.0f, 0.0f);
        normal = crossProduct(tangent, helper);
        normalLength = vectorLength(normal);

        if (normalLength < 1e-6f) {
             
            qDebug() << "ERROR: Failed to compute normal, using default orthogonal basis";
            normal = Point3D(1.0f, 0.0f, 0.0f);
            binormal = Point3D(0.0f, 1.0f, 0.0f);
            return;
        }
    }

     
    normalizeVector(normal);

    qDebug() << "calculateOrthonormalBasis: Normal = ("
             << normal.x << "," << normal.y << "," << normal.z << ")";

     
     
     
    binormal = crossProduct(tangent, normal);

     
    float binormalLength = vectorLength(binormal);
    if (binormalLength < 1e-6f) {
        qDebug() << "ERROR: Binormal vector is too small";
         
        binormal = Point3D(0.0f, 1.0f, 0.0f);
        return;
    }

     
    normalizeVector(binormal);

    qDebug() << "calculateOrthonormalBasis: Binormal = ("
             << binormal.x << "," << binormal.y << "," << binormal.z << ")";

     
    float dot_tn = dotProduct(tangent, normal);
    float dot_tb = dotProduct(tangent, binormal);
    float dot_nb = dotProduct(normal, binormal);

    qDebug() << "calculateOrthonormalBasis: Orthogonality check:";
    qDebug() << "  tangent·normal =" << dot_tn << "(should be ~0)";
    qDebug() << "  tangent·binormal =" << dot_tb << "(should be ~0)";
    qDebug() << "  normal·binormal =" << dot_nb << "(should be ~0)";

     
    if (std::abs(dot_tn) > 0.01f || std::abs(dot_tb) > 0.01f || std::abs(dot_nb) > 0.01f) {
        qDebug() << "WARNING: Basis is not perfectly orthogonal!";
    }
}

Point3D DeformationEngine::getTangentAtZ(const std::vector<Point3D>& deformedCurve, float zCoord) const
{
    if (deformedCurve.empty()) {
        qDebug() << "getTangentAtZ: empty curve, returning default tangent (0,0,1)";
        return Point3D(0.0f, 0.0f, 1.0f);
    }

    if (deformedCurve.size() == 1) {
        qDebug() << "getTangentAtZ: single point curve, returning default tangent (0,0,1)";
        return Point3D(0.0f, 0.0f, 1.0f);
    }

     
    if (zCoord <= deformedCurve.front().z) {
        qDebug() << "getTangentAtZ: zCoord <= first point, using tangent at index 0";
        return calculateTangentAtPoint(deformedCurve, 0);
    }

     
    if (zCoord >= deformedCurve.back().z) {
        qDebug() << "getTangentAtZ: zCoord >= last point, using tangent at index" << (deformedCurve.size() - 1);
        return calculateTangentAtPoint(deformedCurve, deformedCurve.size() - 1);
    }

     
    for (size_t i = 0; i < deformedCurve.size() - 1; ++i) {
        const Point3D& p1 = deformedCurve[i];
        const Point3D& p2 = deformedCurve[i + 1];

        if (zCoord >= p1.z && zCoord <= p2.z) {
             

             
            float t = (zCoord - p1.z) / (p2.z - p1.z);

            qDebug() << "getTangentAtZ: found segment between indices" << i << "and" << (i+1)
                     << ", interpolation parameter t =" << t;

             
            Point3D tangent1 = calculateTangentAtPoint(deformedCurve, i);
            Point3D tangent2 = calculateTangentAtPoint(deformedCurve, i + 1);

             
            Point3D interpolatedTangent;
            interpolatedTangent.x = tangent1.x + t * (tangent2.x - tangent1.x);
            interpolatedTangent.y = tangent1.y + t * (tangent2.y - tangent1.y);
            interpolatedTangent.z = tangent1.z + t * (tangent2.z - tangent1.z);

             
            normalizeVector(interpolatedTangent);

            qDebug() << "getTangentAtZ: interpolated tangent = ("
                     << interpolatedTangent.x << ","
                     << interpolatedTangent.y << ","
                     << interpolatedTangent.z << ")";

            return interpolatedTangent;
        }
    }

     
    qDebug() << "getTangentAtZ: WARNING - segment not found, returning default tangent (0,0,1)";
    return Point3D(0.0f, 0.0f, 1.0f);
}
