#include "trainconfigdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

TrainConfigDialog::TrainConfigDialog(const TrainConfig &config, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Параметры узлов ходовой части"));

    m_preset = new QComboBox(this);
    m_preset->addItem(QStringLiteral("ЭС2Г Ласточка (база)"));
    m_preset->addItem(QStringLiteral("Пользовательская конфигурация"));

    m_wheelDiameter = new QDoubleSpinBox(this);
    m_wheelDiameter->setRange(500.0, 1500.0);
    m_wheelDiameter->setValue(config.wheelDiameterMm);
    m_wheelDiameter->setSuffix(QStringLiteral(" мм"));

    m_axleboxType = new QLineEdit(config.axleboxType, this);
    m_gearboxType = new QLineEdit(config.gearboxType, this);

    m_notes = new QTextEdit(this);
    m_notes->setPlainText(config.notes);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Конфигурация:"), m_preset);
    form->addRow(QStringLiteral("Диаметр колеса:"), m_wheelDiameter);
    form->addRow(QStringLiteral("Тип буксового узла:"), m_axleboxType);
    form->addRow(QStringLiteral("Тип редуктора:"), m_gearboxType);
    form->addRow(QStringLiteral("Примечания:"), m_notes);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_preset, qOverload<int>(&QComboBox::currentIndexChanged), this, &TrainConfigDialog::applyPreset);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

TrainConfig TrainConfigDialog::config() const
{
    TrainConfig cfg;
    cfg.presetName = m_preset->currentText();
    cfg.wheelDiameterMm = m_wheelDiameter->value();
    cfg.axleboxType = m_axleboxType->text();
    cfg.gearboxType = m_gearboxType->text();
    cfg.notes = m_notes->toPlainText();
    return cfg;
}

void TrainConfigDialog::applyPreset()
{
    if (m_preset->currentIndex() != 0) {
        return;
    }

    m_wheelDiameter->setValue(957.0);
    m_axleboxType->setText(QStringLiteral("Кассетный подшипниковый узел"));
    m_gearboxType->setText(QStringLiteral("Одноступенчатый тяговый редуктор"));
}
