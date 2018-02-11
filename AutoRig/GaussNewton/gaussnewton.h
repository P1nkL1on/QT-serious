#ifndef GAUSSNEWTON_H
#define GAUSSNEWTON_H

#include "Eigen/Core"
#include "Eigen/Dense"
#include "QTime"
#include "QDebug"
#include "QPainter"
#include "QColor"

#include "jacobiancalculator.h"

using Eigen::Matrix;

class GraphicMotion
{
private:
    QColor clr;
    int hei, widperstep, top, left, steps;
    float MAX;
public:
    QVector<float> values;
    GraphicMotion();
    GraphicMotion(QColor clr0);
    GraphicMotion(QColor clr0, int hei0, int width, int top0, int left0, int MAX0);
    void DrawOn (QPainter* painter);
};

namespace OptimiseMethods {
    template <typename Function>//, typename CallBack>
    QVector<float> GaussNewtonMethod (Function& func, /*CallBack& callback,*/ const QVector<float> params,
                                     const float epsilon, const int maxIterationCount, QVector<GraphicMotion>&mts, bool isNumerical){

        QTime t , ttotal;
        t.start(); ttotal.start();
        QVector<float> res = params;
        Matrix<float,-1,-1>
                jacobMatrix, F;
        JacobianCalculator::CalculateForFunction(res, jacobMatrix, F, func );
        Matrix<float,-1,-1>
                jacobTrans = Matrix<float, -1, -1>(jacobMatrix.rows(), jacobMatrix.cols()),
                step = Matrix<float,-1,-1>( jacobMatrix.cols(), 1),
                h = Matrix<float,-1,-1>(jacobMatrix.cols(),1);
        for (int i = 0; i < jacobMatrix.cols(); i++)
            h(i,0) = (i < (jacobMatrix.cols() - 3) / 4 * 3 + 3)? 1 : 1;

        int iterationNumber = 0;
        float currentDistance = func(res), firstDist = currentDistance; // set proto distance here

        mts.clear(); int wid = 150, hei = 10;
        mts << GraphicMotion(Qt::black, hei,  (wid + 10) * 4, 25, 10, 0);
        for (int i = 0; i < params.length(); i++){
            if (i < 3){
                // root
                mts << GraphicMotion(Qt::red, hei,  wid, 55, (wid + 10) * (i % 3), 0);
            }else{
                if (i < (jacobMatrix.cols() - 3) / 4 * 3 + 3){
                    // angles
                     mts << GraphicMotion(QColor(255, 200,0), hei,  wid, 60 + (i / 3) * (hei + 4), (wid + 10) * (i % 3), 180);
                }else{
                    // scales
                    mts << GraphicMotion(Qt::green, hei,  wid, 60 + (i - (jacobMatrix.cols() - 3) / 4 * 3 - 2) * (hei + 4), (wid + 10) * 3, 2);
                    //qDebug() << 20 + (i - (jacobMatrix.cols() - 3) / 4 * 3) * (hei + 4);
                }
            }
        }

        do{
            if (isNumerical)
                JacobianCalculator::CalculateNumerical(res, jacobMatrix, F, func );
            else
                JacobianCalculator::CalculateForFunction(res, jacobMatrix, F, func );
            jacobTrans = jacobMatrix.transpose();
            Eigen::MatrixXf jTj = jacobTrans * jacobMatrix;
            Eigen::MatrixXf jF = jacobTrans * F;
            // trace a jF
            for (int i = 0; i < jF.rows(); i++){

                qDebug() << ((i < (jacobMatrix.cols() - 3) / 4 * 3 + 3)?((i < 3)? "ASS" : "JOINT") : "SCALE")
                         << jF(i,0) << "   val:" << res[i];
            }
            //
            step = jTj.colPivHouseholderQr().solve(jF);
//            Eigen::SelfAdjointEigenSolver <Eigen::MatrixXf> es(jTj, false);
//            Eigen::VectorXf evalues = es.eigenvalues();
//            qDebug() << "smalles eigen value" << evalues.array().minCoeff();


            for (int i = 0; i < jacobMatrix.cols(); i++)
            //if (i < (jacobMatrix.cols() - 3) / 4 * 3 + 3)   // do not apply scale
            {
                    res[i] = res[i] - step(i, 0) * h(i, 0);
                    mts[i + 1].values << res[i];
            }

            currentDistance = func(res); mts[0].values << currentDistance;

            iterationNumber ++;
            //qDebug() << ">> Callback call !"; callback (res);
            //if (t.elapsed() > 100)
            qDebug() << "Iteration " << iterationNumber << "Current distance is now " << currentDistance << "      Iteration time is: " << t.elapsed() << " ms"; t.restart();
            if (iterationNumber > maxIterationCount){
                qDebug() << ((currentDistance < firstDist * .5)? "@ Finish by too much iteration count!" : "@ !!!!!!!AAAAAAAAAAAA!!!!!!!!!");
                break;
            }
            //if (stepLength < 1){ qDebug() << "@ Finish cause steps become too liitle!"; break; }

        } while (currentDistance > epsilon);
        qDebug() << "@ Done in " << ttotal.elapsed() << " ms;";
        return res;
    }
}

//                if (i >= (jacobMatrix.cols() - 3) / 4 * 3 + 6 || i < (jacobMatrix.cols() - 3) / 4 * 3 + 3)
//                    //qDebug() << "scale " << res[i] << " - " << step(i,0);
//                    res[i] = res[i] + step(i, 0) * .0;
//                else


#endif // GAUSSNEWTON_H
