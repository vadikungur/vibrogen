#pragma once

#include "models.h"

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QTextEdit;

class TrainConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit TrainConfigDialog(const TrainConfig &config, QWidget *parent = nullptr);

    TrainConfig config() const;

private slots:
    void applyPreset();

private:
    QComboBox *m_preset = nullptr;
    QDoubleSpinBox *m_wheelDiameter = nullptr;
    QLineEdit *m_axleboxType = nullptr;
    QLineEdit *m_gearboxType = nullptr;
    QTextEdit *m_notes = nullptr;
};
