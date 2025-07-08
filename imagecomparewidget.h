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
#include <QSize>

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

    explicit ImageCompareWidget(QWidget *parent = nullptr);
    
    void setImages(const QString &firstImagePath, const QString &secondImagePath);
    void setDirection(CompareDirection direction);
    
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateRevealPosition(const QPoint &mousePos);
    QPixmap scalePixmapToFit(const QPixmap &pixmap, const QSize &targetSize) const;
    QPixmap scalePixmapToFill(const QPixmap &pixmap, const QSize &targetSize) const;

    QPixmap firstImage;
    QPixmap secondImage;
    QPixmap scaledFirstImage;
    QPixmap scaledSecondImage;
    
    CompareDirection direction;
    double revealPosition; // 0.0 to 1.0, represents how much of second image to show
    bool hasImages;
    
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;
};

#endif // IMAGECOMPAREWIDGET_H
