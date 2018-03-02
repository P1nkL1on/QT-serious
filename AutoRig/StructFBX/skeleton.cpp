#include "skeleton.h"
#include "qdebug.h"

using namespace DerOperations;
using Eigen::Matrix;

#include <Eigen/LU>
#include "dermatops.h"
using namespace DerivableVectorMatrixes;

Skeleton::Skeleton()
{
    localRotations = {};
    joints = {};
    localScales = QVector<Matrix<Derivable,1,3>>(0);
}

Skeleton::Skeleton(QVector<Joint *> j)
{
    joints = j;
    localRotations = {};
    for (int lclId = 0; lclId < j.length(); lclId++){
        localRotations << Matrix<Derivable,1,3>();
        if (joints[lclId]->pater == NULL)
            rootTransate = joints[lclId]->localTranslation;
    }
    localScales = QVector<Matrix<Derivable,1,3>>(joints.length());
}

void Skeleton::SetNullRotations()
{
    localRotations = {};
    for (int lclId = 0; lclId < joints.length(); lclId++)
        localRotations << Matrix<Derivable,1,3>();
}


void TraceJoint (const Joint* who, const int depth){
    QString res = ""; for (int i = 0; i < depth; i++)res += "|  ";
    qDebug() << res + who->name;
    for (int kidInd = 0; kidInd < who->kids.length(); kidInd++)
        TraceJoint (who->kids[kidInd], depth + 1);
    return;
}

void Skeleton::DebugTree() const
{
    if (joints.length() <= 0)
    { qDebug () << "No bones;"; return; }
    TraceJoint(joints[0], 0);

    for (int curJoint = 0; curJoint < joints.length(); curJoint++)
        qDebug () << QString::number(curJoint) + ". " + joints[curJoint]->name;
}

void Skeleton::RecursiveApplyLocalRotations(Joint *joint, Matrix<Derivable,1,3> currentRotation)
{
    transformesApplied ++ ;
    Matrix<Derivable,1,3> wasRotation = (joint->pater == NULL)? joint->currentRotation : joint->pater->currentRotation,
              newRotation = wasRotation + currentRotation,
              cTranslation = (joint->pater == NULL)? joint->currentTranslation : joint->pater->currentTranslation;

    //qDebug() << joint->name << wasRotation << currentRotation << " ----  " << newRotation<< " ----  " << joint->localTranslation;
    joint->currentTranslation =
            CommonFuncs::AddDirectWtParent(cTranslation, joint->localTranslation, wasRotation, currentRotation);
    joint->currentRotation = newRotation;

    for (int childId = 0; childId < joint->kids.length(); childId++)
        RecursiveApplyLocalRotations(joint->kids[childId], newRotation);
}

bool EqualSkeletonRotations (QVector<Matrix<Derivable,1,3>> a, QVector<Matrix<Derivable,1,3>> b){
    Q_ASSERT(a.length() == b.length());
    for (int rotInd = 0; rotInd < a.length(); rotInd++)
        if (a != b)//((a[rotInd](0,0) != b[rotInd](0,0)) || (a[rotInd](0,1) != b[rotInd](0,1)) || (a[rotInd](0,2) != b[rotInd](0,2)))
            return false;
    return true;
}

void RecursiveGlobalCalculateCall (Joint* joint){
    joint->CalculateGlobalTransformMatrix();

    joint->currentTranslation = CommonFuncs::AddDirectMatrx(Matrix<Derivable,1,3>(0,0,0),
                                                            joint->globalTransformMatrix);
    for (int childId = 0; childId < joint->kids.length(); childId++)
        RecursiveGlobalCalculateCall(joint->kids[childId]);
}

// MAIN METHOD  OF GETTING SKELETON SEEMS <CHOOL'>

bool Skeleton::CalculateGlobalCoordForEachJointMatrix()
{
    transformesApplied = 0;
//    if (EqualSkeletonRotations(getJointsLocalRotationsForDrawing(), localRotations))
//        return false;

    QVector<int> rootInds = {};

    for (int curJoint = 0; curJoint < joints.length(); curJoint++){
        joints[curJoint]->currentTranslation = Matrix<Derivable,1,3>();              // reset XYZ to calculate them further
        joints[curJoint]->currentRotation = localRotations[curJoint];               // now each joint have info about it need angles
        if (joints[curJoint]->pater == NULL){
            rootInds << curJoint;
            joints[curJoint]->localTranslation = rootTransate;
        }
    }
    for (int jointInd = 0; jointInd < joints.length(); jointInd++){
        joints[jointInd]->RecaulculateLocalTransformMatrix();
    }
    Q_ASSERT(rootInds.length() > 0);
    for (int curRootInd = 0; curRootInd < rootInds.length(); curRootInd ++){
        Joint* root = (joints[rootInds[curRootInd]]);
        RecursiveGlobalCalculateCall(root);
    }
    return true;
}

QVector<Matrix<Derivable, 4, 4> > Skeleton::SetBonesScaleAsBoneLength()
{
    QVector<Matrix<Derivable, 4, 4> > res = getJointsGlobalTranslationsForSkin();
    for (int i = 0; i < joints.length(); i++)
    {
        qDebug() << "";
        qDebug() << " Joint " << i;
        TraceVector(joints[i]->localTranslation);
        TraceVector(joints[i]->localScale);
        joints[i]->localTranslation = CommonFuncs::AddDirectMatrx(joints[i]->localTranslation, /*MakeDeriveRotationMatrix(localRotations[i])* */MakeDeriveScaleMatrix( joints[i]->localScale));

        joints[i]->localScale = Matrix<Derivable,1,3>(1,1,1);
        localScales[i] = Matrix<Derivable,1,3>(1,1,1);//joints[i]->localScale;
        qDebug() << "=";
        TraceVector(joints[i]->localTranslation);
        TraceVector(joints[i]->localScale);
    }
    qDebug() << "All bones' scale translated into local transforms;";
    return res;
}


QVector<Matrix<Derivable,1,3>> Skeleton::getJointsLocalRotationsForDrawing() const
{
    QVector<Matrix<Derivable,1,3>> res;
    for (int curJoint = 0; curJoint < joints.length(); curJoint++)
        res << joints[curJoint]->currentRotation;
    return res;
}



Matrix<Derivable,1,3> Skeleton::getJointCoordByIndex(int index, Matrix<Derivable,1,3> &paterCoord)
{
    if (joints[index]->pater == NULL)
        paterCoord = joints[index]->currentTranslation;
    else
        paterCoord = joints[index]->pater->currentTranslation;
    return joints[index]->currentTranslation;
}

void Skeleton::SetRotation(const Matrix<Derivable,1,3> newRotation, int jointInd)
{
    localRotations[jointInd] = newRotation; //! change -
}

void Skeleton::SetRotations(const QVector<Matrix<Derivable, 1, 3> > newRotations){
    Q_ASSERT(newRotations.length() == localRotations.length());
    for (int curJ = 0; curJ < localRotations.length(); curJ++)
        SetRotation(newRotations[curJ], curJ);
}

void Skeleton::SetRootTranslation(const Eigen::Matrix<Derivable, 1, 3> assTranslate)
{
    rootTransate = assTranslate;
}

void Skeleton::SetScales(const QVector<Matrix<Derivable,1,3>> newScales)
{
    Q_ASSERT(newScales.length() == localScales.length());
    //for (int curJ = 0; curJ < localScales.length(); curJ++)
        //localScales[curJ] = newScales[curJ];
    for (int cJ = 0; cJ < joints.length(); cJ++)
        joints[cJ]->localScale = newScales[cJ];
}

bool Skeleton::getJointTranslationAndRotation(const int jointIndex, Matrix<Derivable,1,3> &translation, Matrix<Derivable,1,3> &rotation) const
{
    translation = joints[jointIndex]->currentTranslation;
    rotation = Matrix<Derivable,1,3>(); Joint* jn = joints[jointIndex];
    do{
        rotation = rotation + jn->currentRotation;
        jn = jn->pater;
    } while ( jn != NULL );
    return true;
}

QVector<Matrix<Derivable,4,4>> Skeleton::getJointsGlobalTranslationsForSkin() const
{
    QVector<Matrix<Derivable,4,4>> res;
    for (int curJoint = 0; curJoint < joints.length(); curJoint++)
            res <<
                   MakeDeriveScaleMatrix(joints[curJoint]->localScale).inverse()
                   * MakeDeriveRotationMatrix(joints[curJoint]->currentRotation)
                   * joints[curJoint]->globalTransformMatrix;
    return res;
}
