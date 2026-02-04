#include "CaliberDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

namespace {
    const QList<QPair<QString, double>> kPresets = {
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
}

CaliberDialog::CaliberDialog(QWidget* pParent)
    : QDialog(pParent)
{
    setWindowTitle(tr("Set Bullet Diameter"));
    setupUi();
    populatePresets();
}

CaliberDialog::~CaliberDialog() = default;

void CaliberDialog::setupUi()
{
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    
    // Instruction
    QLabel* pLabel = new QLabel(tr("Select the bullet diameter for accurate shot marking:"));
    pLabel->setWordWrap(true);
    pLayout->addWidget(pLabel);
    
    pLayout->addSpacing(10);
    
    // Preset combo
    QHBoxLayout* pPresetLayout = new QHBoxLayout();
    pPresetLayout->addWidget(new QLabel(tr("Preset:")));
    m_pPresetCombo = new QComboBox();
    connect(m_pPresetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                if (index < 0 || index >= kPresets.size()) return;
                
                double diameter = kPresets[index].second;
                if (diameter > 0.0)
                {
                    m_pDiameterSpin->setValue(diameter);
                    m_pDiameterSpin->setEnabled(false);
                }
                else
                {
                    // Custom - enable editing
                    m_pDiameterSpin->setEnabled(true);
                }
            });
    pPresetLayout->addWidget(m_pPresetCombo, 1);
    pLayout->addLayout(pPresetLayout);
    
    // Custom diameter
    QHBoxLayout* pDiameterLayout = new QHBoxLayout();
    pDiameterLayout->addWidget(new QLabel(tr("Diameter:")));
    m_pDiameterSpin = new QDoubleSpinBox();
    m_pDiameterSpin->setRange(0.1, 1.0);
    m_pDiameterSpin->setDecimals(3);
    m_pDiameterSpin->setSingleStep(0.001);
    m_pDiameterSpin->setSuffix(" inches");
    pDiameterLayout->addWidget(m_pDiameterSpin, 1);
    pLayout->addLayout(pDiameterLayout);
    
    pLayout->addSpacing(10);
    
    // Buttons
    QDialogButtonBox* pButtonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    pLayout->addWidget(pButtonBox);
    
    setMinimumWidth(350);
}

void CaliberDialog::populatePresets()
{
    for (const auto& preset : kPresets)
    {
        m_pPresetCombo->addItem(preset.first, preset.second);
    }
    
    // Default to .308
    for (int i = 0; i < kPresets.size(); ++i)
    {
        if (qFuzzyCompare(kPresets[i].second, 0.308))
        {
            m_pPresetCombo->setCurrentIndex(i);
            break;
        }
    }
}

double CaliberDialog::bulletDiameter() const
{
    return m_pDiameterSpin->value();
}

void CaliberDialog::setBulletDiameter(double diameter)
{
    m_pDiameterSpin->setValue(diameter);
    
    // Try to find matching preset
    for (int i = 0; i < kPresets.size(); ++i)
    {
        if (qFuzzyCompare(kPresets[i].second, diameter))
        {
            m_pPresetCombo->setCurrentIndex(i);
            return;
        }
    }
    
    // Custom value - select "Custom..."
    m_pPresetCombo->setCurrentIndex(kPresets.size() - 1);
}

