#include "mainwindow.h"

#include "projectserializer.h"
#include "scenariodialog.h"
#include "signaldialog.h"
#include "trainconfigdialog.h"

#include <QAction>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_data.trainConfig = TrainConfig{};
    m_data.scenario = Scenario{};

    setupUi();
    refreshSignalTable();
}

void MainWindow::addSignal()
{
    SignalDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_data.signalEntries.append(dialog.signal());
    refreshSignalTable();
}

void MainWindow::removeSignal()
{
    const int row = m_table->currentRow();
    if (row < 0 || row >= m_data.signalEntries.size()) {
        return;
    }

    m_data.signalEntries.removeAt(row);
    refreshSignalTable();
}

void MainWindow::togglePlayback()
{
    if (m_engine.isRunning()) {
        m_engine.stop();
        m_startStopAction->setText(QStringLiteral("Старт"));
        m_statusLabel->setText(QStringLiteral("Остановлено"));
        return;
    }

    m_engine.setProjectData(collectData());
    QString error;
    if (!m_engine.start(&error)) {
        QMessageBox::critical(this, QStringLiteral("Ошибка"), error);
        return;
    }

    m_startStopAction->setText(QStringLiteral("Стоп"));
    m_statusLabel->setText(QStringLiteral("Воспроизведение"));
}

void MainWindow::openProject()
{
    const QString path = QFileDialog::getOpenFileName(this,
        QStringLiteral("Открыть проект"),
        QString(),
        QStringLiteral("Проект VibroGen (*.vbg.json);;JSON (*.json)"));
    if (path.isEmpty()) {
        return;
    }

    ProjectData loaded;
    QString error;
    if (!ProjectSerializer::load(path, &loaded, &error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка чтения"), error);
        return;
    }

    applyData(loaded);
    statusBar()->showMessage(QStringLiteral("Проект загружен: %1").arg(path), 3000);
}

void MainWindow::saveProject()
{
    const QString path = QFileDialog::getSaveFileName(this,
        QStringLiteral("Сохранить проект"),
        QStringLiteral("project.vbg.json"),
        QStringLiteral("Проект VibroGen (*.vbg.json);;JSON (*.json)"));
    if (path.isEmpty()) {
        return;
    }

    QString error;
    if (!ProjectSerializer::save(path, collectData(), &error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка записи"), error);
        return;
    }

    statusBar()->showMessage(QStringLiteral("Проект сохранён: %1").arg(path), 3000);
}

void MainWindow::editTrainConfig()
{
    TrainConfigDialog dialog(m_data.trainConfig, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_data.trainConfig = dialog.config();
}

void MainWindow::editScenario()
{
    ScenarioDialog dialog(m_data.scenario, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_data.scenario = dialog.scenario();
    m_statusLabel->setText(QStringLiteral("Сценарий: %1").arg(scenarioTypeToString(m_data.scenario.type)));
}

void MainWindow::setupUi()
{
    setWindowTitle(QStringLiteral("VibroGen — генератор вибросигналов"));
    resize(1280, 760);

    setupMenus();
    setupCentral();

    auto *toolBar = addToolBar(QStringLiteral("Воспроизведение"));
    m_startStopAction = toolBar->addAction(QStringLiteral("Старт"), this, &MainWindow::togglePlayback);

    m_statusLabel = new QLabel(QStringLiteral("Остановлено"), this);
    statusBar()->addPermanentWidget(m_statusLabel);
}

void MainWindow::setupMenus()
{
    auto *fileMenu = menuBar()->addMenu(QStringLiteral("Файл"));
    fileMenu->addAction(QStringLiteral("Открыть проект..."), this, &MainWindow::openProject);
    fileMenu->addAction(QStringLiteral("Сохранить проект..."), this, &MainWindow::saveProject);
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("Выход"), this, &QWidget::close);

    auto *settingsMenu = menuBar()->addMenu(QStringLiteral("Параметры"));
    settingsMenu->addAction(QStringLiteral("Сценарий скорости..."), this, &MainWindow::editScenario);
    settingsMenu->addAction(QStringLiteral("Ходовая часть..."), this, &MainWindow::editTrainConfig);

    auto *signalsMenu = menuBar()->addMenu(QStringLiteral("Сигналы"));
    signalsMenu->addAction(QStringLiteral("Добавить..."), this, &MainWindow::addSignal);
    signalsMenu->addAction(QStringLiteral("Удалить выбранный"), this, &MainWindow::removeSignal);
}

void MainWindow::setupCentral()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("Тип"),
        QStringLiteral("Имя"),
        QStringLiteral("Амплитуда"),
        QStringLiteral("Частота, Гц"),
        QStringLiteral("Шум/мощность"),
        QStringLiteral("Дефект"),
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto *buttonsWidget = new QWidget(this);
    auto *buttonsLayout = new QHBoxLayout(buttonsWidget);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    auto *addButton = new QPushButton(QStringLiteral("Добавить сигнал"), this);
    auto *removeButton = new QPushButton(QStringLiteral("Удалить сигнал"), this);
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addStretch();

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSignal);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeSignal);

    m_tabs = new QTabWidget(this);
    auto *oscillo = new QTextEdit(this);
    oscillo->setReadOnly(true);
    oscillo->setText(QStringLiteral("Осциллограмма будет отображаться здесь."));

    auto *spectrum = new QTextEdit(this);
    spectrum->setReadOnly(true);
    spectrum->setText(QStringLiteral("Спектр будет отображаться здесь."));

    auto *waterfall = new QTextEdit(this);
    waterfall->setReadOnly(true);
    waterfall->setText(QStringLiteral("Waterfall будет отображаться здесь."));

    m_tabs->addTab(oscillo, QStringLiteral("Осциллограмма"));
    m_tabs->addTab(spectrum, QStringLiteral("Спектр"));
    m_tabs->addTab(waterfall, QStringLiteral("Waterfall"));

    layout->addWidget(m_table, 3);
    layout->addWidget(buttonsWidget);
    layout->addWidget(m_tabs, 2);

    setCentralWidget(central);
}

void MainWindow::refreshSignalTable()
{
    m_table->setRowCount(m_data.signalEntries.size());

    for (int row = 0; row < m_data.signalEntries.size(); ++row) {
        const auto &signal = m_data.signalEntries.at(row);
        m_table->setItem(row, 0, new QTableWidgetItem(signalTypeToString(signal.type)));
        m_table->setItem(row, 1, new QTableWidgetItem(signal.name));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::number(signal.amplitude, 'f', 2)));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(signal.frequencyHz, 'f', 2)));

        QString misc = signal.type == SignalType::WhiteNoise
            ? QStringLiteral("Полоса: %1 / P=%2").arg(signal.bandwidthHz).arg(signal.spectralPower)
            : QStringLiteral("-");
        m_table->setItem(row, 4, new QTableWidgetItem(misc));
        m_table->setItem(row, 5, new QTableWidgetItem(signal.defectKind));
    }
}

ProjectData MainWindow::collectData() const
{
    return m_data;
}

void MainWindow::applyData(const ProjectData &data)
{
    m_data = data;
    refreshSignalTable();
}
