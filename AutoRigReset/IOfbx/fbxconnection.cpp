#include "fbxconnection.h"

QString IOfbx::FbxConnection::parse(const QStringList &buffer)
{
    if (buffer.length() != 2)
        return "Incomplete connection type!";
    if (buffer[0].indexOf(',') < 0 || buffer[1].indexOf(',') < 0)
        return "Not pairable connection! (Required: a,b)";
    QStringList names = buffer[0].split(',');
    QStringList ids = buffer[1].split(',');
    if (names.length() != 2 || ids.length() < 3){
        for (int i = 0; i < buffer.length(); ++i)
            traceMessage ( buffer[i]);
        return "Unusuall connection format!";
    }
    idLeft = ids[1];
    idRight = ids[2];
    type = ConnectionType::Other;
    if (names[0].indexOf("Model::") >= 0){
        if (names[1].indexOf("Model::") >= 0)
            type = ConnectionType::BoneToBone;
        if (names[1].indexOf("SubDeformer::") >= 0)
            type = ConnectionType::ModelToSubDeformer;
        return QString();
    }

    if (names[0].indexOf("Model::") >= 0 && names[1].indexOf("Model::") >= 0)
        type = ConnectionType::BoneToBone;
    if (names[0].indexOf("Model::") >= 0 && names[1].indexOf("SubDeformer::") >= 0)
        type = ConnectionType::ModelToSubDeformer;
    if (names[0].indexOf("SubDeformer::") >= 0 && names[1].indexOf("Deformer::") >= 0)
        type = ConnectionType::SubDeformerToDeformer;
    if (names[0].indexOf("Deformer::") >= 0 && names[1].indexOf("Geometry::") >= 0)
        type = ConnectionType::DeformderToGeometry;
    if (names[0].indexOf("Geometry::") >= 0 && names[1].indexOf("Model::") >= 0)
        type = ConnectionType::GeometryToMesh;

    return QString();
}

QString IOfbx::FbxConnection::getIdLeft() const
{
    return idLeft;
}

QString IOfbx::FbxConnection::getIdRight() const
{
    return idRight;
}

IOfbx::ConnectionType IOfbx::FbxConnection::getType() const
{
    return type;
}


bool IOfbx::FbxConnection::setIds(const QString &left, const QString &right)
{
    idLeft = left;
    idRight = right;
    return true;
}

void IOfbx::FbxConnection::setType(const IOfbx::ConnectionType &value)
{
    type = value;
}

