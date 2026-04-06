#pragma once

#include "models.h"

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;

class SignalDialog : public QDialog {
    Q_OBJECT
public:
    explicit SignalDialog(QWidget *parent = nullptr);

    SignalEntry signal() const;

private slots:
    void updateUiForType();

private:
    QComboBox *m_typeCombo = nullptr;
    QLineEdit *m_nameEdit = nullptr;
    QDoubleSpinBox *m_amplitude = nullptr;
    QDoubleSpinBox *m_frequency = nullptr;
    QDoubleSpinBox *m_bandwidth = nullptr;
    QDoubleSpinBox *m_power = nullptr;
    QLineEdit *m_defectKind = nullptr;
};
