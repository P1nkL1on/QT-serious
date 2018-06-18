#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFileDialog>
#include <QDebug>
#include "IOfbx/iofbxtests.h"
#include "IOfbx/fbxtorigconverter.h"

using namespace IOfbx;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->runTests();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
FbxParsedContainer* parsed = nullptr;

void MainWindow::on_actionLoad_Rig_triggered()
{
    loadRigByAdress(QFileDialog::getOpenFileName(this, "Load Rig", "../Models/FBX/", "FBX files (*.FBX)"));
}

void MainWindow::on_actionLoad_Guad_triggered()
{
    loadRigGuardYelling();
}

void MainWindow::on_actionLoad_Ivan_Sergeich_triggered()
{
    loadRigIvanSergeich();
}

FbxStruct::Rig *r = nullptr;
void MainWindow::loadRigByAdress(QString path)
{
    QString err = "";
    parsed = loadFromPath(path, err);
    if (!err.isEmpty())
        qDebug() << err;
    else
    {
        parsed->traceInfo();
        if (r != nullptr)
            delete r;
        r = FbxConverter::convertContainerToRig(parsed);
        delete parsed;
        this->repaint();
    }

}

void MainWindow::loadRigGuardYelling()
{
    loadRigByAdress("../Models/FBX/!guard yelling exported.FBX");
}

void MainWindow::loadRigIvanSergeich()
{
    loadRigByAdress("../Models/FBX/!IvanSergeich0.FBX");
}

void MainWindow::on_actionLoad_Hand_triggered()
{
    loadRigByAdress("../Models/FBX/hand.FBX");
}

void MainWindow::runTests()
{
    IOfbx::IOfbxTests fbxTests;
    QTest::qExec(&fbxTests);
}
void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter qp(this);
    if (r != nullptr)
        r->repaint(&qp);
    qp.end();

}

void MainWindow::on_actionTransformMesh_triggered()
{
//    if (r != nullptr)
//        r->TestRotate();
    this->repaint();
}
