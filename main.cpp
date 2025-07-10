//===========================================
//  photo compare source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFileInfo>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Photo Compare");
    app.setApplicationVersion("1.0");
    app.setApplicationDisplayName("Photo Compare");
    
    // Set up command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("A tool for comparing two images with wipe and dissolve modes");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add positional arguments for the two image files
    parser.addPositionalArgument("image1", "Path to the first image file");
    parser.addPositionalArgument("image2", "Path to the second image file");
    
    // Process command line arguments
    parser.process(app);
    
    // Get positional arguments
    const QStringList args = parser.positionalArguments();
    
    QString firstImagePath;
    QString secondImagePath;
    
    // Check if images were provided as arguments
    if (args.size() >= 1) {
        firstImagePath = args.at(0);
        // Validate first image file
        QFileInfo firstFile(firstImagePath);
        if (!firstFile.exists() || !firstFile.isFile()) {
            QMessageBox::critical(nullptr, "Error", 
                QString("First image file does not exist: %1").arg(firstImagePath));
            return 1;
        }
    }
    
    if (args.size() >= 2) {
        secondImagePath = args.at(1);
        // Validate second image file
        QFileInfo secondFile(secondImagePath);
        if (!secondFile.exists() || !secondFile.isFile()) {
            QMessageBox::critical(nullptr, "Error", 
                QString("Second image file does not exist: %1").arg(secondImagePath));
            return 1;
        }
    }
    
    // Create and show main window
    MainWindow window;
    
    // Load images if provided
    if (!firstImagePath.isEmpty()) {
        window.loadFirstImage(firstImagePath);
    }
    if (!secondImagePath.isEmpty()) {
        window.loadSecondImage(secondImagePath);
    }
    
    window.show();
    
    return app.exec();
}
