#pragma once

#include "models.h"

#include <QString>

class ProjectSerializer {
public:
    static bool save(const QString &filePath, const ProjectData &data, QString *error);
    static bool load(const QString &filePath, ProjectData *data, QString *error);
};
