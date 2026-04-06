#pragma once

#include "models.h"

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;

class ScenarioDialog : public QDialog {
    Q_OBJECT
public:
    explicit ScenarioDialog(const Scenario &scenario, QWidget *parent = nullptr);

    Scenario scenario() const;

private:
    QComboBox *m_type = nullptr;
    QDoubleSpinBox *m_startSpeed = nullptr;
    QDoubleSpinBox *m_targetSpeed = nullptr;
    QDoubleSpinBox *m_accel = nullptr;
    QDoubleSpinBox *m_cycleStop = nullptr;
    QDoubleSpinBox *m_cycleCruise = nullptr;
};
