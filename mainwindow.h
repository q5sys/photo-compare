#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include "imagecomparewidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Public methods for loading images from command line
    void loadFirstImage(const QString &imagePath);
    void loadSecondImage(const QString &imagePath);

private slots:
    void selectFirstImage();
    void selectSecondImage();
    void onDirectionChanged();
    void onCompareModeChanged();
    void onDissolveSettingsChanged();
    void onDissolveToggle();

private:
    void setupUI();
    void updateCompareWidget();

    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *controlsLayout;
    
    // Left side - Image controls
    QVBoxLayout *imageControlsLayout;
    QPushButton *firstImageButton;
    QPushButton *secondImageButton;
    QLabel *firstImageLabel;
    QLabel *secondImageLabel;
    
    // Right side - Mode controls
    QVBoxLayout *modeControlsLayout;
    
    // Wipe mode controls
    QHBoxLayout *wipeLayout;
    QRadioButton *wipeModeRadio;
    QComboBox *directionComboBox;
    
    // Dissolve mode controls
    QHBoxLayout *dissolveLayout;
    QRadioButton *dissolveModeRadio;
    QLabel *holdTimeLabel;
    QDoubleSpinBox *holdTimeSpinBox;
    QLabel *transitionTimeLabel;
    QDoubleSpinBox *transitionTimeSpinBox;
    QPushButton *dissolveToggleButton;
    
    QButtonGroup *modeGroup;
    
    ImageCompareWidget *compareWidget;
    
    // Data
    QString firstImagePath;
    QString secondImagePath;
    bool isDissolving;
};

#endif // MAINWINDOW_H
