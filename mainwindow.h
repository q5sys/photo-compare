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
#include "imagecomparewidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectFirstImage();
    void selectSecondImage();
    void onDirectionChanged();

private:
    void setupUI();
    void updateCompareWidget();

    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QVBoxLayout *controlsLayout;
    QHBoxLayout *imageButtonsLayout;
    QHBoxLayout *directionLayout;
    
    QPushButton *firstImageButton;
    QPushButton *secondImageButton;
    QLabel *firstImageLabel;
    QLabel *secondImageLabel;
    
    QRadioButton *leftToRightRadio;
    QRadioButton *rightToLeftRadio;
    QRadioButton *topToBottomRadio;
    QRadioButton *bottomToTopRadio;
    QButtonGroup *directionGroup;
    
    ImageCompareWidget *compareWidget;
    
    // Data
    QString firstImagePath;
    QString secondImagePath;
};

#endif // MAINWINDOW_H
