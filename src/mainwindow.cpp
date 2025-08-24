//===========================================
//  photo compare source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , controlsLayout(nullptr)
    , imageControlsLayout(nullptr)
    , firstImageButton(nullptr)
    , secondImageButton(nullptr)
    , firstImageLabel(nullptr)
    , secondImageLabel(nullptr)
    , modeControlsLayout(nullptr)
    , wipeLayout(nullptr)
    , wipeModeRadio(nullptr)
    , directionComboBox(nullptr)
    , dissolveLayout(nullptr)
    , dissolveModeRadio(nullptr)
    , holdTimeLabel(nullptr)
    , holdTimeSpinBox(nullptr)
    , transitionTimeLabel(nullptr)
    , transitionTimeSpinBox(nullptr)
    , dissolveToggleButton(nullptr)
    , modeGroup(nullptr)
    , compareWidget(nullptr)
    , isDissolving(false)
{
    setupUI();
}

MainWindow::~MainWindow()
{
    // Qt handles cleanup automatically for child widgets
}

void MainWindow::setupUI()
{
    setWindowTitle("Photo Compare");
    setMinimumSize(900, 700);
    
    // Create central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Create single row controls layout split in middle
    controlsLayout = new QHBoxLayout();
    
    // Left side - Image controls (stacked)
    imageControlsLayout = new QVBoxLayout();
    
    // First image row
    QHBoxLayout *firstImageLayout = new QHBoxLayout();
    firstImageButton = new QPushButton("Select First Image", this);
    firstImageLabel = new QLabel("No image selected", this);
    firstImageLabel->setStyleSheet("color: gray; font-style: italic;");
    firstImageButton->setMaximumWidth(150);
    
    firstImageLayout->addWidget(firstImageButton);
    firstImageLayout->addWidget(firstImageLabel);
    firstImageLayout->addStretch();
    
    // Second image row
    QHBoxLayout *secondImageLayout = new QHBoxLayout();
    secondImageButton = new QPushButton("Select Second Image", this);
    secondImageLabel = new QLabel("No image selected", this);
    secondImageLabel->setStyleSheet("color: gray; font-style: italic;");
    secondImageButton->setMaximumWidth(150);
    
    secondImageLayout->addWidget(secondImageButton);
    secondImageLayout->addWidget(secondImageLabel);
    secondImageLayout->addStretch();
    
    imageControlsLayout->addLayout(firstImageLayout);
    imageControlsLayout->addLayout(secondImageLayout);
    
    // Right side - Mode controls (stacked)
    modeControlsLayout = new QVBoxLayout();
    
    // Wipe mode row
    wipeLayout = new QHBoxLayout();
    wipeModeRadio = new QRadioButton("Wipe", this);
    directionComboBox = new QComboBox(this);
    directionComboBox->addItem("Left to Right");
    directionComboBox->addItem("Right to Left");
    directionComboBox->addItem("Top to Bottom");
    directionComboBox->addItem("Bottom to Top");
    directionComboBox->setCurrentIndex(0); // Default to Left to Right
    
    wipeLayout->addWidget(wipeModeRadio);
    wipeLayout->addWidget(directionComboBox);
    wipeLayout->addStretch();
    
    // Dissolve mode row
    dissolveLayout = new QHBoxLayout();
    dissolveModeRadio = new QRadioButton("Dissolve", this);
    
    holdTimeLabel = new QLabel("Hold:", this);
    holdTimeSpinBox = new QDoubleSpinBox(this);
    holdTimeSpinBox->setRange(0.1, 10.0);
    holdTimeSpinBox->setSingleStep(0.1);
    holdTimeSpinBox->setValue(2.0);
    holdTimeSpinBox->setDecimals(1);
    holdTimeSpinBox->setSuffix("s");
    holdTimeSpinBox->setMaximumWidth(60);
    
    transitionTimeLabel = new QLabel("Fade:", this);
    transitionTimeSpinBox = new QDoubleSpinBox(this);
    transitionTimeSpinBox->setRange(0.1, 5.0);
    transitionTimeSpinBox->setSingleStep(0.1);
    transitionTimeSpinBox->setValue(1.0);
    transitionTimeSpinBox->setDecimals(1);
    transitionTimeSpinBox->setSuffix("s");
    transitionTimeSpinBox->setMaximumWidth(60);
    
    dissolveToggleButton = new QPushButton("Start", this);
    dissolveToggleButton->setEnabled(false);
    dissolveToggleButton->setMaximumWidth(60);
    
    dissolveLayout->addWidget(dissolveModeRadio);
    dissolveLayout->addWidget(holdTimeLabel);
    dissolveLayout->addWidget(holdTimeSpinBox);
    dissolveLayout->addWidget(transitionTimeLabel);
    dissolveLayout->addWidget(transitionTimeSpinBox);
    dissolveLayout->addWidget(dissolveToggleButton);
    dissolveLayout->addStretch();
    
    modeControlsLayout->addLayout(wipeLayout);
    modeControlsLayout->addLayout(dissolveLayout);
    
    // Set up radio button group
    modeGroup = new QButtonGroup(this);
    modeGroup->addButton(wipeModeRadio);
    modeGroup->addButton(dissolveModeRadio);
    wipeModeRadio->setChecked(true); // Default selection
    
    // Initially enable wipe controls and disable dissolve controls
    directionComboBox->setEnabled(true);
    holdTimeSpinBox->setEnabled(false);
    transitionTimeSpinBox->setEnabled(false);
    dissolveToggleButton->setEnabled(false);
    
    // Add left and right sides to main controls layout
    controlsLayout->addLayout(imageControlsLayout, 1);
    controlsLayout->addLayout(modeControlsLayout, 1);
    
    // Create image compare widget
    compareWidget = new ImageCompareWidget(this);
    
    // Add everything to main layout
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(compareWidget, 1); // Give it stretch factor of 1
    
    // Connect signals
    connect(firstImageButton, &QPushButton::clicked, this, &MainWindow::selectFirstImage);
    connect(secondImageButton, &QPushButton::clicked, this, &MainWindow::selectSecondImage);
    connect(directionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDirectionChanged);
    connect(wipeModeRadio, &QRadioButton::toggled, this, &MainWindow::onCompareModeChanged);
    connect(dissolveModeRadio, &QRadioButton::toggled, this, &MainWindow::onCompareModeChanged);
    connect(holdTimeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onDissolveSettingsChanged);
    connect(transitionTimeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onDissolveSettingsChanged);
    connect(dissolveToggleButton, &QPushButton::clicked, this, &MainWindow::onDissolveToggle);
}

void MainWindow::selectFirstImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select First Image",
        "",
        "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)");
    
    if (!fileName.isEmpty()) {
        firstImagePath = fileName;
        QFileInfo fileInfo(fileName);
        firstImageLabel->setText(fileInfo.fileName());
        firstImageLabel->setStyleSheet("color: black; font-style: normal;");
        updateCompareWidget();
    }
}

void MainWindow::selectSecondImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select Second Image",
        "",
        "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)");
    
    if (!fileName.isEmpty()) {
        secondImagePath = fileName;
        QFileInfo fileInfo(fileName);
        secondImageLabel->setText(fileInfo.fileName());
        secondImageLabel->setStyleSheet("color: black; font-style: normal;");
        updateCompareWidget();
    }
}

void MainWindow::onDirectionChanged()
{
    updateCompareWidget();
}

void MainWindow::onCompareModeChanged()
{
    if (wipeModeRadio->isChecked()) {
        compareWidget->setCompareMode(ImageCompareWidget::WipeMode);
        
        // Enable wipe controls, disable dissolve controls
        directionComboBox->setEnabled(true);
        holdTimeSpinBox->setEnabled(false);
        transitionTimeSpinBox->setEnabled(false);
        dissolveToggleButton->setEnabled(false);
        
        // Stop dissolve if it's running
        if (isDissolving) {
            compareWidget->stopDissolve();
            isDissolving = false;
            dissolveToggleButton->setText("Start");
        }
    } else if (dissolveModeRadio->isChecked()) {
        compareWidget->setCompareMode(ImageCompareWidget::DissolveMode);
        
        // Disable wipe controls, enable dissolve controls
        directionComboBox->setEnabled(false);
        holdTimeSpinBox->setEnabled(true);
        transitionTimeSpinBox->setEnabled(true);
        
        // Enable dissolve button only if images are loaded
        bool hasImages = !firstImagePath.isEmpty() && !secondImagePath.isEmpty();
        dissolveToggleButton->setEnabled(hasImages);
    }
}

void MainWindow::onDissolveSettingsChanged()
{
    double holdTime = holdTimeSpinBox->value();
    double transitionTime = transitionTimeSpinBox->value();
    compareWidget->setDissolveSettings(holdTime, transitionTime);
}

void MainWindow::onDissolveToggle()
{
    if (!isDissolving) {
        // Start dissolve
        compareWidget->startDissolve();
        isDissolving = true;
        dissolveToggleButton->setText("Stop");
        
        // Disable settings while dissolving
        holdTimeSpinBox->setEnabled(false);
        transitionTimeSpinBox->setEnabled(false);
    } else {
        // Stop dissolve
        compareWidget->stopDissolve();
        isDissolving = false;
        dissolveToggleButton->setText("Start");
        
        // Re-enable settings
        holdTimeSpinBox->setEnabled(true);
        transitionTimeSpinBox->setEnabled(true);
    }
}

void MainWindow::loadFirstImage(const QString &imagePath)
{
    if (!imagePath.isEmpty()) {
        firstImagePath = imagePath;
        QFileInfo fileInfo(imagePath);
        firstImageLabel->setText(fileInfo.fileName());
        firstImageLabel->setStyleSheet("color: black; font-style: normal;");
        updateCompareWidget();
    }
}

void MainWindow::loadSecondImage(const QString &imagePath)
{
    if (!imagePath.isEmpty()) {
        secondImagePath = imagePath;
        QFileInfo fileInfo(imagePath);
        secondImageLabel->setText(fileInfo.fileName());
        secondImageLabel->setStyleSheet("color: black; font-style: normal;");
        updateCompareWidget();
    }
}

void MainWindow::updateCompareWidget()
{
    if (!firstImagePath.isEmpty() && !secondImagePath.isEmpty()) {
        compareWidget->setImages(firstImagePath, secondImagePath);
        
        // Set direction based on combo box selection
        int directionIndex = directionComboBox->currentIndex();
        switch (directionIndex) {
            case 0:
                compareWidget->setDirection(ImageCompareWidget::LeftToRight);
                break;
            case 1:
                compareWidget->setDirection(ImageCompareWidget::RightToLeft);
                break;
            case 2:
                compareWidget->setDirection(ImageCompareWidget::TopToBottom);
                break;
            case 3:
                compareWidget->setDirection(ImageCompareWidget::BottomToTop);
                break;
            default:
                compareWidget->setDirection(ImageCompareWidget::LeftToRight);
                break;
        }
        
        // Set compare mode
        if (wipeModeRadio->isChecked()) {
            compareWidget->setCompareMode(ImageCompareWidget::WipeMode);
        } else if (dissolveModeRadio->isChecked()) {
            compareWidget->setCompareMode(ImageCompareWidget::DissolveMode);
        }
        
        // Update dissolve settings
        compareWidget->setDissolveSettings(holdTimeSpinBox->value(), transitionTimeSpinBox->value());
        
        // Enable dissolve button if in dissolve mode
        if (dissolveModeRadio->isChecked()) {
            dissolveToggleButton->setEnabled(true);
        }
    } else {
        // Disable dissolve button if no images
        dissolveToggleButton->setEnabled(false);
    }
}
