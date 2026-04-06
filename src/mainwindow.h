#pragma once

#include "models.h"
#include "signalengine.h"

#include <QMainWindow>

class QLabel;
class QTableWidget;
class QTabWidget;
class QAction;
class QTimer;
class OscillogramWidget;
class SpectrumWidget;
class WaterfallWidget;

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
    void updateVisualizations();

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
    QTimer *m_visualTimer = nullptr;
    OscillogramWidget *m_oscillogram = nullptr;
    SpectrumWidget *m_spectrum = nullptr;
    WaterfallWidget *m_waterfall = nullptr;

    ProjectData m_data;
    SignalEngine m_engine;
};
