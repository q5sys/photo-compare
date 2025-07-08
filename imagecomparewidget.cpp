//===========================================
//  photo compare source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include "imagecomparewidget.h"
#include <QPaintEvent>
#include <QFileInfo>

ImageCompareWidget::ImageCompareWidget(QWidget *parent)
    : QWidget(parent)
    , direction(LeftToRight)
    , revealPosition(0.0)
    , hasImages(false)
{
    setMouseTracking(true);
    setMinimumSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

void ImageCompareWidget::setImages(const QString &firstImagePath, const QString &secondImagePath)
{
    firstImage = QPixmap(firstImagePath);
    secondImage = QPixmap(secondImagePath);
    
    if (!firstImage.isNull() && !secondImage.isNull()) {
        hasImages = true;
        // Scale images will be done in paintEvent to always fit current widget size
        revealPosition = 0.0; // Reset reveal position
        update(); // Trigger repaint
    } else {
        hasImages = false;
        update();
    }
}

void ImageCompareWidget::setDirection(CompareDirection newDirection)
{
    direction = newDirection;
    revealPosition = 0.0; // Reset reveal position when direction changes
    update();
}

QSize ImageCompareWidget::sizeHint() const
{
    return QSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

void ImageCompareWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (!hasImages) {
        // Draw placeholder text with system theme colors
        painter.fillRect(rect(), palette().color(QPalette::Window));
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawText(rect(), Qt::AlignCenter, "Select two images to compare");
        return;
    }
    
    // Scale images to fit the widget while maintaining aspect ratio
    QRect widgetRect = rect();
    scaledFirstImage = scalePixmapToFit(firstImage, widgetRect.size());
    scaledSecondImage = scalePixmapToFit(secondImage, widgetRect.size());
    
    // Calculate the position to center images
    QPoint firstImagePos = QPoint(
        (widgetRect.width() - scaledFirstImage.width()) / 2,
        (widgetRect.height() - scaledFirstImage.height()) / 2
    );
    QPoint secondImagePos = QPoint(
        (widgetRect.width() - scaledSecondImage.width()) / 2,
        (widgetRect.height() - scaledSecondImage.height()) / 2
    );
    
    // Draw the first image (base image)
    painter.drawPixmap(firstImagePos, scaledFirstImage);
    
    // Create clipping region for the second image based on reveal position and direction
    if (revealPosition > 0.0) {
        QRect clipRect;
        
        if (direction == LeftToRight) {
            int revealWidth = static_cast<int>(scaledSecondImage.width() * revealPosition);
            clipRect = QRect(secondImagePos.x(), secondImagePos.y(), revealWidth, scaledSecondImage.height());
        } else if (direction == RightToLeft) {
            int revealWidth = static_cast<int>(scaledSecondImage.width() * revealPosition);
            int startX = secondImagePos.x() + scaledSecondImage.width() - revealWidth;
            clipRect = QRect(startX, secondImagePos.y(), revealWidth, scaledSecondImage.height());
        } else if (direction == TopToBottom) {
            int revealHeight = static_cast<int>(scaledSecondImage.height() * revealPosition);
            clipRect = QRect(secondImagePos.x(), secondImagePos.y(), scaledSecondImage.width(), revealHeight);
        } else { // BottomToTop
            int revealHeight = static_cast<int>(scaledSecondImage.height() * revealPosition);
            int startY = secondImagePos.y() + scaledSecondImage.height() - revealHeight;
            clipRect = QRect(secondImagePos.x(), startY, scaledSecondImage.width(), revealHeight);
        }
        
        // Set clipping region and draw second image
        painter.setClipRect(clipRect);
        painter.drawPixmap(secondImagePos, scaledSecondImage);
        painter.setClipping(false);
        
        // Draw a subtle line to show the reveal boundary
        painter.setPen(QPen(QColor(255, 255, 255, 180), 2));
        if (direction == LeftToRight) {
            int lineX = secondImagePos.x() + static_cast<int>(scaledSecondImage.width() * revealPosition);
            painter.drawLine(lineX, secondImagePos.y(), lineX, secondImagePos.y() + scaledSecondImage.height());
        } else if (direction == RightToLeft) {
            int lineX = secondImagePos.x() + scaledSecondImage.width() - static_cast<int>(scaledSecondImage.width() * revealPosition);
            painter.drawLine(lineX, secondImagePos.y(), lineX, secondImagePos.y() + scaledSecondImage.height());
        } else if (direction == TopToBottom) {
            int lineY = secondImagePos.y() + static_cast<int>(scaledSecondImage.height() * revealPosition);
            painter.drawLine(secondImagePos.x(), lineY, secondImagePos.x() + scaledSecondImage.width(), lineY);
        } else { // BottomToTop
            int lineY = secondImagePos.y() + scaledSecondImage.height() - static_cast<int>(scaledSecondImage.height() * revealPosition);
            painter.drawLine(secondImagePos.x(), lineY, secondImagePos.x() + scaledSecondImage.width(), lineY);
        }
    }
}

void ImageCompareWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (hasImages) {
        updateRevealPosition(event->pos());
    }
    QWidget::mouseMoveEvent(event);
}

void ImageCompareWidget::leaveEvent(QEvent *event)
{
    // Reset reveal position when mouse leaves the widget
    revealPosition = 0.0;
    update();
    QWidget::leaveEvent(event);
}

void ImageCompareWidget::updateRevealPosition(const QPoint &mousePos)
{
    if (!hasImages) return;
    
    // Calculate the image bounds
    QRect widgetRect = rect();
    QPoint imagePos = QPoint(
        (widgetRect.width() - scaledFirstImage.width()) / 2,
        (widgetRect.height() - scaledFirstImage.height()) / 2
    );
    QRect imageRect(imagePos, scaledFirstImage.size());
    
    // Check if mouse is within image bounds
    if (!imageRect.contains(mousePos)) {
        revealPosition = 0.0;
        update();
        return;
    }
    
    // Calculate reveal position based on direction
    if (direction == LeftToRight) {
        double relativeX = static_cast<double>(mousePos.x() - imageRect.left()) / imageRect.width();
        revealPosition = qBound(0.0, relativeX, 1.0);
    } else if (direction == RightToLeft) {
        double relativeX = static_cast<double>(imageRect.right() - mousePos.x()) / imageRect.width();
        revealPosition = qBound(0.0, relativeX, 1.0);
    } else if (direction == TopToBottom) {
        double relativeY = static_cast<double>(mousePos.y() - imageRect.top()) / imageRect.height();
        revealPosition = qBound(0.0, relativeY, 1.0);
    } else { // BottomToTop
        double relativeY = static_cast<double>(imageRect.bottom() - mousePos.y()) / imageRect.height();
        revealPosition = qBound(0.0, relativeY, 1.0);
    }
    
    update(); // Trigger repaint
}

QPixmap ImageCompareWidget::scalePixmapToFit(const QPixmap &pixmap, const QSize &targetSize) const
{
    if (pixmap.isNull()) return QPixmap();
    
    // Scale pixmap to fit within target size while maintaining aspect ratio
    return pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QPixmap ImageCompareWidget::scalePixmapToFill(const QPixmap &pixmap, const QSize &targetSize) const
{
    if (pixmap.isNull()) return QPixmap();
    
    // Scale pixmap to fill target size while maintaining aspect ratio (may crop)
    return pixmap.scaled(targetSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}
