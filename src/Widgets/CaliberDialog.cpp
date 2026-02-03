#include "CaliberDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

const QList<QPair<QString, double>> CaliberDialog::PRESETS = {
    {".17 HMR", 0.172},
    {".22 LR / .223 / 5.56mm", 0.224},
    {"6mm / .243", 0.243},
    {"6.5mm / .264", 0.264},
    {".270", 0.277},
    {"7mm / .284", 0.284},
    {".30 / .308 / 7.62mm", 0.308},
    {".338", 0.338},
    {"9mm / .355", 0.355},
    {".375", 0.375},
    {".40 / 10mm", 0.400},
    {".45 ACP", 0.452},
    {".50 BMG", 0.510},
    {"Custom...", 0.0}
};

CaliberDialog::CaliberDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Set Bullet Diameter"));
    setupUi();
    populatePresets();
}

CaliberDialog::~CaliberDialog() = default;

void CaliberDialog::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Instruction
    QLabel *label = new QLabel(tr("Select the bullet diameter for accurate shot marking:"));
    label->setWordWrap(true);
    layout->addWidget(label);
    
    layout->addSpacing(10);
    
    // Preset combo
    QHBoxLayout *presetLayout = new QHBoxLayout();
    presetLayout->addWidget(new QLabel(tr("Preset:")));
    m_presetCombo = new QComboBox();
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CaliberDialog::onPresetChanged);
    presetLayout->addWidget(m_presetCombo, 1);
    layout->addLayout(presetLayout);
    
    // Custom diameter
    QHBoxLayout *diameterLayout = new QHBoxLayout();
    diameterLayout->addWidget(new QLabel(tr("Diameter:")));
    m_diameterSpin = new QDoubleSpinBox();
    m_diameterSpin->setRange(0.1, 1.0);
    m_diameterSpin->setDecimals(3);
    m_diameterSpin->setSingleStep(0.001);
    m_diameterSpin->setSuffix(" inches");
    diameterLayout->addWidget(m_diameterSpin, 1);
    layout->addLayout(diameterLayout);
    
    layout->addSpacing(10);
    
    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    setMinimumWidth(350);
}

void CaliberDialog::populatePresets()
{
    for (const auto &preset : PRESETS)
    {
        m_presetCombo->addItem(preset.first, preset.second);
    }
    
    // Default to .308
    for (int i = 0; i < PRESETS.size(); ++i)
    {
        if (qFuzzyCompare(PRESETS[i].second, 0.308))
        {
            m_presetCombo->setCurrentIndex(i);
            break;
        }
    }
}

double CaliberDialog::bulletDiameter() const
{
    return m_diameterSpin->value();
}

void CaliberDialog::setBulletDiameter(double diameter)
{
    m_diameterSpin->setValue(diameter);
    
    // Try to find matching preset
    for (int i = 0; i < PRESETS.size(); ++i)
    {
        if (qFuzzyCompare(PRESETS[i].second, diameter))
        {
            m_presetCombo->setCurrentIndex(i);
            return;
        }
    }
    
    // Custom value - select "Custom..."
    m_presetCombo->setCurrentIndex(PRESETS.size() - 1);
}

void CaliberDialog::onPresetChanged(int index)
{
    if (index < 0 || index >= PRESETS.size()) return;
    
    double diameter = PRESETS[index].second;
    if (diameter > 0.0)
    {
        m_diameterSpin->setValue(diameter);
        m_diameterSpin->setEnabled(false);
    }
    else
    {
        // Custom - enable editing
        m_diameterSpin->setEnabled(true);
    }
}
