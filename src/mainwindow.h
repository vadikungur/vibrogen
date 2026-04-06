#pragma once

#include "models.h"
#include "signalengine.h"

#include <QMainWindow>

class QLabel;
class QTableWidget;
class QTabWidget;
class QAction;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addSignal();
    void removeSignal();
    void togglePlayback();
    void openProject();
    void saveProject();
    void editTrainConfig();
    void editScenario();

private:
    void setupUi();
    void setupMenus();
    void setupCentral();
    void refreshSignalTable();
    ProjectData collectData() const;
    void applyData(const ProjectData &data);

    QAction *m_startStopAction = nullptr;
    QTableWidget *m_table = nullptr;
    QTabWidget *m_tabs = nullptr;
    QLabel *m_statusLabel = nullptr;

    ProjectData m_data;
    SignalEngine m_engine;
};
