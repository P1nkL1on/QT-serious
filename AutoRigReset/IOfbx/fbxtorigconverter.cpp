#include "fbxtorigconverter.h"
#include <QDebug>

using namespace FbxStruct;
using namespace Df;

void IOfbx::FbxConverter::convertContainerToRig(const IOfbx::FbxParsedContainer *container)
{
    QVector<Joint> fbxJoints;

    QVector<QString> parsedMeshIds; // meant geometry
    QVector<QString> parsedJointIds;

    for (const auto parsedMesh : container->getMeshes())
        parsedMeshIds << parsedMesh.getId();

    for (const auto parsedJoint : container->getJoints()){
        fbxJoints << convertJoint(parsedJoint);
        parsedJointIds << parsedJoint.getId();
    }

    for (const auto pose : container->getPosenodes()){
        int jointIndex = parsedJointIds.indexOf(pose.getId());
        if (jointIndex >= 0){
            fbxJoints[jointIndex].setBindTransform(initialiseMatrix(pose.getTransformMatrixArray()));
            traceMessage( QString("v   PoseNode connected to %3: #%1 (%2)")
                          .arg(jointIndex)
                          .arg(parsedJointIds[jointIndex])
                          .arg((fbxJoints[jointIndex].isMeshDependent()) ? "mesh" : "bone"));
        }
    }

    for (const auto connect : container->getConnections()){
        int leftIndex = -1;
        int rightIndex = -1;
        switch (connect.getType()){
        case ConnectionType::BoneToBone:
            leftIndex= parsedJointIds.indexOf(connect.getIdLeft());
            rightIndex = parsedJointIds.indexOf(connect.getIdRight());
            if (leftIndex >= 0 && rightIndex >= 0)
                traceMessage( QString("v   Bones connected: #%1 (%3)-> #%2 (%4)")
                              .arg(leftIndex).arg(rightIndex)
                              .arg(parsedJointIds[leftIndex])
                              .arg(parsedJointIds[rightIndex]));
            break;
        default:
            break;
        }
    }


#warning убрать потом из общео числа джойнтов все те, которые отвечают за меши
#warning убрать из числа деформеров те, которые отвечают не за кластеры

    return;
}

Joint IOfbx::FbxConverter::convertJoint(const IOfbx::FbxModelJoint &parsedJoint)
{
    if (parsedJoint.isMeshDependent())
        traceMessage(QString("o   Bone with ID %1 is mesh denedent;").arg(parsedJoint.getId()));
    return Joint(
                makeVector3fromQVector<float>(parsedJoint.getLocalTranslation()),
                makeVector3fromQVector<float>(parsedJoint.getLocalRotation()),
                makeVector3fromQVector<float>(parsedJoint.getLocalScaling()),
                parsedJoint.isMeshDependent());
}
