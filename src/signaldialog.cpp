#include "signaldialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QVBoxLayout>

SignalDialog::SignalDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Добавление сигнала"));

    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem(signalTypeToString(SignalType::Harmonic), static_cast<int>(SignalType::Harmonic));
    m_typeCombo->addItem(signalTypeToString(SignalType::WhiteNoise), static_cast<int>(SignalType::WhiteNoise));
    m_typeCombo->addItem(signalTypeToString(SignalType::Defect), static_cast<int>(SignalType::Defect));

    m_nameEdit = new QLineEdit(this);
    m_amplitude = new QDoubleSpinBox(this);
    m_amplitude->setRange(0.0, 10.0);
    m_amplitude->setValue(1.0);

    m_frequency = new QDoubleSpinBox(this);
    m_frequency->setRange(0.1, 20000.0);
    m_frequency->setValue(100.0);

    m_bandwidth = new QDoubleSpinBox(this);
    m_bandwidth->setRange(1.0, 20000.0);
    m_bandwidth->setValue(1000.0);

    m_power = new QDoubleSpinBox(this);
    m_power->setRange(0.0, 10.0);
    m_power->setValue(1.0);

    m_defectKind = new QLineEdit(this);
    m_defectKind->setText(QStringLiteral("Дефект наружного кольца подшипника"));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Тип:"), m_typeCombo);
    form->addRow(QStringLiteral("Наименование:"), m_nameEdit);
    form->addRow(QStringLiteral("Амплитуда:"), m_amplitude);
    form->addRow(QStringLiteral("Частота, Гц:"), m_frequency);
    form->addRow(QStringLiteral("Полоса шума, Гц:"), m_bandwidth);
    form->addRow(QStringLiteral("Спектральная мощность:"), m_power);
    form->addRow(QStringLiteral("Тип дефекта:"), m_defectKind);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_typeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &SignalDialog::updateUiForType);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    updateUiForType();
}

SignalEntry SignalDialog::signal() const
{
    SignalEntry signal;
    signal.type = static_cast<SignalType>(m_typeCombo->currentData().toInt());
    signal.name = m_nameEdit->text();
    signal.amplitude = m_amplitude->value();
    signal.frequencyHz = m_frequency->value();
    signal.bandwidthHz = m_bandwidth->value();
    signal.spectralPower = m_power->value();
    signal.defectKind = m_defectKind->text();

    if (signal.name.isEmpty()) {
        signal.name = signalTypeToString(signal.type);
    }

    return signal;
}

void SignalDialog::updateUiForType()
{
    const auto type = static_cast<SignalType>(m_typeCombo->currentData().toInt());
    const bool isNoise = type == SignalType::WhiteNoise;
    const bool isDefect = type == SignalType::Defect;

    m_frequency->setEnabled(!isNoise);
    m_bandwidth->setEnabled(isNoise);
    m_power->setEnabled(isNoise);
    m_defectKind->setEnabled(isDefect);
}
