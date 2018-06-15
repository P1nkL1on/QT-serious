#include "fbxtorigconverter.h"
#include <QDebug>

using namespace FbxStruct;
using namespace Df;

void IOfbx::FbxConverter::convertContainerToRig(const IOfbx::FbxParsedContainer *container)
{
    QVector<Joint> fbxJoints;
    QVector<Cluster> fbxClusters;

    QVector<QString> parsedMeshIds; // meant geometry
    QVector<QString> parsedJointIds;
    QVector<QString> parsedClustersIds;
    QVector<QString> parsedClustersAttendedToDeformsIds;
    QVector<QString> parsedDeformerIds;
    QVector<QString> parsedDeformerAttendedToGeometryIds;

    for (const auto parsedMesh : container->getMeshes()){
        parsedMeshIds << parsedMesh.getId();
        parsedDeformerAttendedToGeometryIds << QString(); // id of attended deformer
    }

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

    for (const auto parsedCluster : container->getClusters()){
        if (!parsedCluster.isParentDeformder()){
            fbxClusters << convertCluster(parsedCluster);
            parsedClustersIds << parsedCluster.getId();
            parsedClustersAttendedToDeformsIds << QString();
        } else
            parsedDeformerIds << parsedCluster.getId();
    }

    for (const auto connect : container->getConnections()){
        int leftIndex = -1;
        int rightIndex = -1;
        switch (connect.getType()){
        case ConnectionType::BoneToBone:
            leftIndex= parsedJointIds.indexOf(connect.getIdLeft());
            rightIndex = parsedJointIds.indexOf(connect.getIdRight());
            if (leftIndex < 0 || rightIndex < 0)
                break;
            traceMessage( QString("v   Bones connected: #%1 (%3)-> #%2 (%4)")
                          .arg(leftIndex).arg(rightIndex)
                          .arg(parsedJointIds[leftIndex])
                          .arg(parsedJointIds[rightIndex]));
            fbxJoints[leftIndex].setPaterIndex(rightIndex);
            fbxJoints[rightIndex].addKidIndex(leftIndex);
            break;
        case ConnectionType::ModelToSubDeformer:
            // bone to cluster
            leftIndex= parsedJointIds.indexOf(connect.getIdLeft());
            rightIndex = parsedClustersIds.indexOf(connect.getIdRight());
            if (leftIndex < 0 || rightIndex < 0)
                break;
            traceMessage( QString("v   Bone #%1 connected to Cluster #%2")
                          .arg(leftIndex).arg(rightIndex));
            fbxJoints[leftIndex].addClusterIndex(rightIndex);
            break;
        case ConnectionType::SubDeformerToDeformer:
            leftIndex = parsedClustersIds.indexOf(connect.getIdLeft());
            rightIndex = parsedDeformerIds.indexOf(connect.getIdRight()); // index of attended deformer
            if (leftIndex < 0 || rightIndex < 0)
                break;
            traceMessage( QString("v   SubDeformer (cluster) #%1 connected to Deformer #%2")
                          .arg(leftIndex).arg(rightIndex));
            parsedClustersAttendedToDeformsIds[leftIndex]
                    = parsedDeformerIds[rightIndex];
            break;
        case ConnectionType::DeformderToGeometry:
            leftIndex = parsedDeformerIds.indexOf(connect.getIdLeft()); // index of attended deformer
            rightIndex = parsedMeshIds.indexOf(connect.getIdRight());   // index of geoemtry-mesh
            if (leftIndex < 0 || rightIndex < 0)
                break;
            traceMessage( QString("v   Deformer #%1 connected to Geometry #%2")
                          .arg(leftIndex).arg(rightIndex));
            parsedDeformerAttendedToGeometryIds[rightIndex]
                    = parsedDeformerIds[leftIndex];
            break;
        default:
            break;
        }
    }

    // upto connect clusters with geoemtry:
    for (int clustIndex = 0; clustIndex < fbxClusters.length(); ++clustIndex){
        int meshInd = parsedDeformerAttendedToGeometryIds.indexOf(parsedClustersAttendedToDeformsIds[clustIndex]);
        if (meshInd >= 0){
            fbxClusters[clustIndex].setMeshIndex(meshInd);
            traceMessage( QString("v   Cluster #%1 connected to mesh #%2;")
                          .arg(clustIndex).arg(fbxClusters[clustIndex].getMeshIndex()));
        }else
            traceMessage( QString("v   Cluster #%1, ID %2 was not connected to any mesh, it is useless;")
                          .arg(clustIndex).arg(parsedClustersIds[clustIndex]));

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

Cluster IOfbx::FbxConverter::convertCluster(const IOfbx::FbxSubDeformerCluster &parsedCluster)
{
    if (parsedCluster.isEmpty())
        traceMessage(QString("o   Csluter with Id %1 is empty. Is it a Deformer? elsewhere it is strange;").arg(parsedCluster.getId()));
    return Cluster(parsedCluster.getIndexes(),
                   parsedCluster.getWeights(),
                   initialiseMatrix<double>(parsedCluster.getTransformMatrix()),
                   initialiseMatrix<double>(parsedCluster.getTransformLinkMatrix()));
}
