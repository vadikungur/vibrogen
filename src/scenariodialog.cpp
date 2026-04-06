#include "scenariodialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>

ScenarioDialog::ScenarioDialog(const Scenario &scenario, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Параметры сценария скорости"));

    m_type = new QComboBox(this);
    m_type->addItem(scenarioTypeToString(ScenarioType::ConstantSpeed), static_cast<int>(ScenarioType::ConstantSpeed));
    m_type->addItem(scenarioTypeToString(ScenarioType::Accelerate), static_cast<int>(ScenarioType::Accelerate));
    m_type->addItem(scenarioTypeToString(ScenarioType::Brake), static_cast<int>(ScenarioType::Brake));
    m_type->addItem(scenarioTypeToString(ScenarioType::Cycle), static_cast<int>(ScenarioType::Cycle));

    m_startSpeed = new QDoubleSpinBox(this);
    m_startSpeed->setRange(0.0, 150.0);
    m_startSpeed->setValue(scenario.startSpeedKmh);

    m_targetSpeed = new QDoubleSpinBox(this);
    m_targetSpeed->setRange(0.0, 150.0);
    m_targetSpeed->setValue(scenario.targetSpeedKmh);

    m_accel = new QDoubleSpinBox(this);
    m_accel->setRange(0.1, 20.0);
    m_accel->setValue(scenario.accelKmhPerSec);

    m_cycleStop = new QDoubleSpinBox(this);
    m_cycleStop->setRange(1.0, 300.0);
    m_cycleStop->setValue(scenario.cycleStopSec);

    m_cycleCruise = new QDoubleSpinBox(this);
    m_cycleCruise->setRange(1.0, 600.0);
    m_cycleCruise->setValue(scenario.cycleCruiseSec);

    m_type->setCurrentIndex(m_type->findData(static_cast<int>(scenario.type)));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Тип сценария:"), m_type);
    form->addRow(QStringLiteral("Начальная скорость, км/ч:"), m_startSpeed);
    form->addRow(QStringLiteral("Целевая скорость, км/ч:"), m_targetSpeed);
    form->addRow(QStringLiteral("Темп изменения, км/ч/с:"), m_accel);
    form->addRow(QStringLiteral("Остановка в цикле, с:"), m_cycleStop);
    form->addRow(QStringLiteral("Равномерный ход в цикле, с:"), m_cycleCruise);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

Scenario ScenarioDialog::scenario() const
{
    Scenario s;
    s.type = static_cast<ScenarioType>(m_type->currentData().toInt());
    s.startSpeedKmh = m_startSpeed->value();
    s.targetSpeedKmh = m_targetSpeed->value();
    s.accelKmhPerSec = m_accel->value();
    s.cycleStopSec = m_cycleStop->value();
    s.cycleCruiseSec = m_cycleCruise->value();
    return s;
}
