//===========================================
//  photo compare source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#ifndef IMAGECOMPAREWIDGET_H
#define IMAGECOMPAREWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QSize>
#include <QPoint>
#include <QTimer>
#include <QPropertyAnimation>

class ImageCompareWidget : public QWidget
{
    Q_OBJECT

public:
    enum CompareDirection {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };

    enum CompareMode {
        WipeMode,
        DissolveMode
    };

    explicit ImageCompareWidget(QWidget *parent = nullptr);
    
    void setImages(const QString &firstImagePath, const QString &secondImagePath);
    void setDirection(CompareDirection direction);
    void setCompareMode(CompareMode mode);
    void setDissolveSettings(double holdTime, double transitionTime);
    void startDissolve();
    void stopDissolve();
    
    QSize sizeHint() const override;

public slots:
    void setOpacity(double opacity);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onDissolveTimer();

private:
    void updateRevealPosition(const QPoint &mousePos);
    void resetZoom();
    void zoomIn();
    void zoomOut();
    void updateImageTransforms();
    void startDissolveTransition();
    QPoint mapToImageCoordinates(const QPoint &widgetPos) const;
    QPixmap scalePixmapToFit(const QPixmap &pixmap, const QSize &targetSize) const;
    QPixmap scalePixmapToFill(const QPixmap &pixmap, const QSize &targetSize) const;

    QPixmap firstImage;
    QPixmap secondImage;
    QPixmap scaledFirstImage;
    QPixmap scaledSecondImage;
    
    CompareDirection direction;
    CompareMode compareMode;
    double revealPosition; // 0.0 to 1.0, represents how much of second image to show
    bool hasImages;
    
    // Zoom and pan functionality
    double zoomFactor;
    QPoint panOffset;
    QPoint lastPanPoint;
    bool isPanning;
    
    // Dissolve functionality
    QTimer *dissolveTimer;
    QPropertyAnimation *opacityAnimation;
    double currentOpacity; // 0.0 = first image only, 1.0 = second image only
    double holdTime; // seconds to hold each image
    double transitionTime; // seconds for transition
    bool isDissolving;
    bool showingSecondImage; // true when transitioning to/showing second image
    
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;
    static constexpr double MIN_ZOOM = 0.1;
    static constexpr double MAX_ZOOM = 10.0;
    static constexpr double ZOOM_STEP = 1.2;
};

#endif // IMAGECOMPAREWIDGET_H
