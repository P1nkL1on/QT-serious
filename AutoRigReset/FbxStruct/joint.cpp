#include "joint.h"

using namespace FbxStruct;
using namespace Df;

Joint::Joint(const Vector3<float> &localTranslation,
             const Vector3<float> &localRotation,
             const Vector3<float> &localScaling,
             const bool isMeshDepended,
             const QString name):
    localTranslation(localTranslation),
    localRotation(localRotation),
    localScaling(localScaling),
    meshDependent(isMeshDepended),
    jointName(name)
{

}

void Joint::setPaterIndex(const int paterPtrInd)
{
    Q_ASSERT(paterInd == -1);
    paterInd = paterPtrInd;
}

void Joint::addKidIndex(const int kidPtrInd)
{
    if (kidPtrInd != -1)
        kidsInd << kidPtrInd;
}

void Joint::addClusterIndex(const int clusterPtrInd)
{
    clusterInds << clusterPtrInd;
}

void Joint::setBindTransform(const Df::Matrix4<double> &value)
{
    bindTransform = value;
}

bool Joint::isMeshDependent() const
{
    return meshDependent;
}

int Joint::getPaterInd() const
{
    return paterInd;
}

QVector<int> Joint::getKidsInd() const
{
    return kidsInd;
}

Df::Matrix4<double> Joint::getBindTransform() const
{
    return bindTransform;
}

Df::Matrix4<double> Joint::getLocalTransform() const
{
    return localTransform;
}

Df::Matrix4<double> Joint::setGlobalTransform(const Df::Matrix4<double> &value)
{
    globalTransform = value;
    //traceMatrix(globalTransform);
    return globalTransform;
}

QString Joint::getJointName() const
{
    return jointName;
}

Df::Vector3<float> Joint::getLocalTranslation() const
{
    return localTranslation;
}

int d = 0;
Df::Vector3<float> Joint::getLocalRotation() const
{
    //int scaling = (getJointName().indexOf("Arm") >= 0) ? ++d : 0;
    //return {0, scaling, 0};
    //return localRotation;
    return makeVector3fromDoubles<float>(0.0, 0.0, 0.0);
}

Df::Vector3<float> Joint::getLocalScaling() const
{
    //    for (int i = 0; i < 3; ++i)
    //        if (localScaling(i,0) < .0001 && localScaling(i,0) > .0001)
    //            return Df::makeUnitVector3<float>();

    return localScaling;
}

Df::Matrix4<double> Joint::setLocalTransform(const Df::Matrix4<double> &value)
{
    localTransform = value;
    return localTransform;
}

Df::Matrix4<double> Joint::getGlobalTransform() const
{
    return globalTransform;
}


//Matrix4d computeLocalMatrix(/*some stuff like translation, rotation, etc*/)
//void computeLocalMatrices(/*Some information about all translations, rotations, etc*/, QVector<Matrix4d> &outLocalMatrices)
//QVector<Matrix4d> computeLocalMatrices(/*Some information about all translations, rotations, etc*/)
