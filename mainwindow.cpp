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
    , imageButtonsLayout(nullptr)
    , directionLayout(nullptr)
    , firstImageButton(nullptr)
    , secondImageButton(nullptr)
    , firstImageLabel(nullptr)
    , secondImageLabel(nullptr)
    , leftToRightRadio(nullptr)
    , topToBottomRadio(nullptr)
    , directionGroup(nullptr)
    , compareWidget(nullptr)
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
    
    // Create controls layout
    controlsLayout = new QVBoxLayout();
    
    // Image selection buttons
    imageButtonsLayout = new QHBoxLayout();
    firstImageButton = new QPushButton("Select First Image", this);
    secondImageButton = new QPushButton("Select Second Image", this);
    firstImageLabel = new QLabel("No image selected", this);
    secondImageLabel = new QLabel("No image selected", this);
    
    firstImageLabel->setStyleSheet("color: gray; font-style: italic;");
    secondImageLabel->setStyleSheet("color: gray; font-style: italic;");
    
    imageButtonsLayout->addWidget(firstImageButton);
    imageButtonsLayout->addWidget(firstImageLabel);
    imageButtonsLayout->addStretch();
    imageButtonsLayout->addWidget(secondImageButton);
    imageButtonsLayout->addWidget(secondImageLabel);
    
    // Direction selection
    directionLayout = new QHBoxLayout();
    leftToRightRadio = new QRadioButton("Left to Right", this);
    rightToLeftRadio = new QRadioButton("Right to Left", this);
    topToBottomRadio = new QRadioButton("Top to Bottom", this);
    bottomToTopRadio = new QRadioButton("Bottom to Top", this);
    directionGroup = new QButtonGroup(this);
    
    directionGroup->addButton(leftToRightRadio);
    directionGroup->addButton(rightToLeftRadio);
    directionGroup->addButton(topToBottomRadio);
    directionGroup->addButton(bottomToTopRadio);
    leftToRightRadio->setChecked(true); // Default selection
    
    directionLayout->addWidget(new QLabel("Compare Direction:", this));
    directionLayout->addWidget(leftToRightRadio);
    directionLayout->addWidget(rightToLeftRadio);
    directionLayout->addWidget(topToBottomRadio);
    directionLayout->addWidget(bottomToTopRadio);
    directionLayout->addStretch();
    
    // Add controls to controls layout
    controlsLayout->addLayout(imageButtonsLayout);
    controlsLayout->addLayout(directionLayout);
    
    // Create image compare widget
    compareWidget = new ImageCompareWidget(this);
    
    // Add everything to main layout
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(compareWidget, 1); // Give it stretch factor of 1
    
    // Connect signals
    connect(firstImageButton, &QPushButton::clicked, this, &MainWindow::selectFirstImage);
    connect(secondImageButton, &QPushButton::clicked, this, &MainWindow::selectSecondImage);
    connect(leftToRightRadio, &QRadioButton::toggled, this, &MainWindow::onDirectionChanged);
    connect(rightToLeftRadio, &QRadioButton::toggled, this, &MainWindow::onDirectionChanged);
    connect(topToBottomRadio, &QRadioButton::toggled, this, &MainWindow::onDirectionChanged);
    connect(bottomToTopRadio, &QRadioButton::toggled, this, &MainWindow::onDirectionChanged);
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

void MainWindow::updateCompareWidget()
{
    if (!firstImagePath.isEmpty() && !secondImagePath.isEmpty()) {
        compareWidget->setImages(firstImagePath, secondImagePath);
        
        if (leftToRightRadio->isChecked()) {
            compareWidget->setDirection(ImageCompareWidget::LeftToRight);
        } else if (rightToLeftRadio->isChecked()) {
            compareWidget->setDirection(ImageCompareWidget::RightToLeft);
        } else if (topToBottomRadio->isChecked()) {
            compareWidget->setDirection(ImageCompareWidget::TopToBottom);
        } else if (bottomToTopRadio->isChecked()) {
            compareWidget->setDirection(ImageCompareWidget::BottomToTop);
        }
    }
}
