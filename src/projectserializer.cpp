#include "projectserializer.h"

#include <QFile>
#include <QJsonDocument>

bool ProjectSerializer::save(const QString &filePath, const ProjectData &data, QString *error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
            *error = file.errorString();
        }
        return false;
    }

    const QJsonDocument doc(data.toJson());
    if (file.write(doc.toJson(QJsonDocument::Indented)) < 0) {
        if (error) {
            *error = file.errorString();
        }
        return false;
    }

    return true;
}

bool ProjectSerializer::load(const QString &filePath, ProjectData *data, QString *error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) {
            *error = file.errorString();
        }
        return false;
    }

    QJsonParseError parseError;
    const auto doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        if (error) {
            *error = parseError.errorString();
        }
        return false;
    }

    *data = ProjectData::fromJson(doc.object());
    return true;
}
