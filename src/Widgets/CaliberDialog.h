#pragma once

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;

/// @brief Modal dialog for bullet diameter selection
///
/// Provides a user interface for selecting bullet diameter (caliber) for accurate
/// shot marking. The dialog offers both preset caliber selections and custom
/// diameter input. When a preset is selected, the diameter spin box is
/// automatically populated and disabled. Selecting "Custom..." enables manual
/// entry of any diameter value.
///
/// The last selected caliber is persisted to QSettings and restored on next open.
/// On first use, defaults to .223 / 5.56mm (0.224 inches).
/// Diameter values are specified in inches and can range from 0.1 to 1.0 inches
/// with 3 decimal places of precision.
class CaliberDialog : public QDialog
{
    Q_OBJECT

public:
    /// @brief Constructs a new CaliberDialog
    ///
    /// Initializes the dialog with preset caliber options and sets up the UI.
    /// Restores the last saved caliber from QSettings, defaulting to .223 on
    /// first use.
    ///
    /// \param pParent Parent widget (typically the main window)
    explicit CaliberDialog(QWidget* pParent = nullptr);
    
    ~CaliberDialog();

    /// @brief Returns the currently selected bullet diameter
    ///
    /// \return Bullet diameter in inches (0.1 to 1.0 range)
    double bulletDiameter() const;
    
    /// @brief Sets the bullet diameter and updates the UI accordingly
    ///
    /// If the provided diameter matches a preset value, that preset is
    /// automatically selected and the diameter spin box is disabled.
    /// Otherwise, "Custom..." is selected and the spin box is enabled
    /// for manual editing.
    ///
    /// \param diameter Bullet diameter in inches (0.1 to 1.0 range)
    void setBulletDiameter(double diameter);

    /// @brief Saves the current caliber selection to QSettings and accepts the dialog
    void accept() override;

private:
    /// @brief Sets up the dialog UI layout and widgets
    ///
    /// Creates the vertical layout with instruction label, preset combo box,
    /// diameter spin box, and standard OK/Cancel buttons.
    void setupUi();
    
    /// @brief Populates the preset combo box with caliber options
    ///
    /// Loads all preset calibers and restores the last saved selection from
    /// QSettings, defaulting to .223 / 5.56mm on first use.
    void populatePresets();

private:
    QComboBox* m_pPresetCombo = nullptr;      ///< Combo box for preset caliber selection
    QDoubleSpinBox* m_pDiameterSpin = nullptr; ///< Spin box for diameter input (inches)
};
