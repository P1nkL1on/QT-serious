#ifndef TESTAUTORIG_H
#define TESTAUTORIG_H

#include "StructFBX/rig.h"
#include "QVector"
#include "qpainter.h"

#include "Eigen/Core"
#include "Derivable/dermatops.h"
#include "Eigen/Dense"
#include "QTime"
#include "mainwindow.h"
#include "gaussnewton.h"
#include "mainwindow.h"
using Eigen::Matrix;
using namespace  DerOperations;



class TestAutoRig : public GraphicsObjectStruct::GraphicsObject
{
private:
    QVector<Rig*> targetMeshes;
    Rig* bendingRig;
    int targMeshInd;
    QVector<GraphicMotion> gt;

public:
    QVector3D* camCenter;
    QMainWindow* window;
    void ChangeTargetMeshInd (int count);
    QString ApplyDrawToCanvas(QPainter* painter,const QMatrix4x4 view, const QMatrix4x4 perspective,
                           const int width, const int hei) override;
    TestAutoRig();
    TestAutoRig(Rig* rig, QVector<Rig*> mesh);

    bool SetCustomLowModel ();
    bool GausNewtone ();
    bool MiscBugHunt ();
    float TestSkinBending ();
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};




#endif // TESTAUTORIG_H
