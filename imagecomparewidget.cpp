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
    , compareMode(WipeMode)
    , revealPosition(0.0)
    , hasImages(false)
    , zoomFactor(1.0)
    , panOffset(0, 0)
    , lastPanPoint(0, 0)
    , isPanning(false)
    , dissolveTimer(nullptr)
    , opacityAnimation(nullptr)
    , currentOpacity(0.0)
    , holdTime(2.0)
    , transitionTime(1.0)
    , isDissolving(false)
    , showingSecondImage(false)
{
    setMouseTracking(true);
    setMinimumSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    setFocusPolicy(Qt::StrongFocus); // Enable keyboard events
    
    // Initialize dissolve timer
    dissolveTimer = new QTimer(this);
    connect(dissolveTimer, &QTimer::timeout, this, &ImageCompareWidget::onDissolveTimer);
    
    // Initialize opacity animation
    opacityAnimation = new QPropertyAnimation(this, "currentOpacity", this);
    connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [this](const QVariant &value) {
        setOpacity(value.toDouble());
    });
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
        QString helpText = "Select two images to compare\nUse mouse wheel to zoom, drag to pan";
        if (compareMode == DissolveMode) {
            helpText += "\nDissolve mode: images will fade between each other";
        }
        painter.drawText(rect(), Qt::AlignCenter, helpText);
        return;
    }
    
    // Scale images to fit the widget while maintaining aspect ratio, then apply zoom
    QRect widgetRect = rect();
    scaledFirstImage = scalePixmapToFit(firstImage, widgetRect.size());
    scaledSecondImage = scalePixmapToFit(secondImage, widgetRect.size());
    
    // Apply zoom factor to the scaled images
    QSize zoomedSize = QSize(
        static_cast<int>(scaledFirstImage.width() * zoomFactor),
        static_cast<int>(scaledFirstImage.height() * zoomFactor)
    );
    
    QPixmap zoomedFirstImage = scaledFirstImage.scaled(zoomedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap zoomedSecondImage = scaledSecondImage.scaled(zoomedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Calculate the position to center images with pan offset
    QPoint firstImagePos = QPoint(
        (widgetRect.width() - zoomedFirstImage.width()) / 2 + panOffset.x(),
        (widgetRect.height() - zoomedFirstImage.height()) / 2 + panOffset.y()
    );
    QPoint secondImagePos = QPoint(
        (widgetRect.width() - zoomedSecondImage.width()) / 2 + panOffset.x(),
        (widgetRect.height() - zoomedSecondImage.height()) / 2 + panOffset.y()
    );
    
    if (compareMode == DissolveMode) {
        // Draw first image
        painter.drawPixmap(firstImagePos, zoomedFirstImage);
        
        // Draw second image with opacity
        if (currentOpacity > 0.0) {
            painter.setOpacity(currentOpacity);
            painter.drawPixmap(secondImagePos, zoomedSecondImage);
            painter.setOpacity(1.0);
        }
    } else {
        // Wipe mode - original functionality
        // Draw the first image (base image)
        painter.drawPixmap(firstImagePos, zoomedFirstImage);
        
        // Create clipping region for the second image based on reveal position and direction
        if (revealPosition > 0.0) {
            QRect clipRect;
            
            if (direction == LeftToRight) {
                int revealWidth = static_cast<int>(zoomedSecondImage.width() * revealPosition);
                clipRect = QRect(secondImagePos.x(), secondImagePos.y(), revealWidth, zoomedSecondImage.height());
            } else if (direction == RightToLeft) {
                int revealWidth = static_cast<int>(zoomedSecondImage.width() * revealPosition);
                int startX = secondImagePos.x() + zoomedSecondImage.width() - revealWidth;
                clipRect = QRect(startX, secondImagePos.y(), revealWidth, zoomedSecondImage.height());
            } else if (direction == TopToBottom) {
                int revealHeight = static_cast<int>(zoomedSecondImage.height() * revealPosition);
                clipRect = QRect(secondImagePos.x(), secondImagePos.y(), zoomedSecondImage.width(), revealHeight);
            } else { // BottomToTop
                int revealHeight = static_cast<int>(zoomedSecondImage.height() * revealPosition);
                int startY = secondImagePos.y() + zoomedSecondImage.height() - revealHeight;
                clipRect = QRect(secondImagePos.x(), startY, zoomedSecondImage.width(), revealHeight);
            }
            
            // Set clipping region and draw second image
            painter.setClipRect(clipRect);
            painter.drawPixmap(secondImagePos, zoomedSecondImage);
            painter.setClipping(false);
            
            // Draw a subtle line to show the reveal boundary
            painter.setPen(QPen(QColor(255, 255, 255, 180), 2));
            if (direction == LeftToRight) {
                int lineX = secondImagePos.x() + static_cast<int>(zoomedSecondImage.width() * revealPosition);
                painter.drawLine(lineX, secondImagePos.y(), lineX, secondImagePos.y() + zoomedSecondImage.height());
            } else if (direction == RightToLeft) {
                int lineX = secondImagePos.x() + zoomedSecondImage.width() - static_cast<int>(zoomedSecondImage.width() * revealPosition);
                painter.drawLine(lineX, secondImagePos.y(), lineX, secondImagePos.y() + zoomedSecondImage.height());
            } else if (direction == TopToBottom) {
                int lineY = secondImagePos.y() + static_cast<int>(zoomedSecondImage.height() * revealPosition);
                painter.drawLine(secondImagePos.x(), lineY, secondImagePos.x() + zoomedSecondImage.width(), lineY);
            } else { // BottomToTop
                int lineY = secondImagePos.y() + zoomedSecondImage.height() - static_cast<int>(zoomedSecondImage.height() * revealPosition);
                painter.drawLine(secondImagePos.x(), lineY, secondImagePos.x() + zoomedSecondImage.width(), lineY);
            }
        }
    }
    
    // Draw zoom level indicator
    if (zoomFactor != 1.0) {
        painter.setPen(QPen(QColor(255, 255, 255, 200), 1));
        painter.setBrush(QBrush(QColor(0, 0, 0, 100)));
        QRect zoomRect(10, 10, 80, 25);
        painter.drawRoundedRect(zoomRect, 5, 5);
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(zoomRect, Qt::AlignCenter, QString("Zoom: %1%").arg(static_cast<int>(zoomFactor * 100)));
    }
    
    // Draw dissolve mode indicator
    if (compareMode == DissolveMode && isDissolving) {
        painter.setPen(QPen(QColor(255, 255, 255, 200), 1));
        painter.setBrush(QBrush(QColor(0, 0, 0, 100)));
        QRect dissolveRect(10, 40, 100, 25);
        painter.drawRoundedRect(dissolveRect, 5, 5);
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(dissolveRect, Qt::AlignCenter, "Dissolving...");
    }
}

void ImageCompareWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (hasImages) {
        if (isPanning && (event->buttons() & Qt::LeftButton)) {
            // Handle panning
            QPoint delta = event->pos() - lastPanPoint;
            panOffset += delta;
            lastPanPoint = event->pos();
            update();
        } else {
            // Handle image comparison reveal
            updateRevealPosition(event->pos());
        }
    }
    QWidget::mouseMoveEvent(event);
}

void ImageCompareWidget::mousePressEvent(QMouseEvent *event)
{
    if (hasImages && event->button() == Qt::LeftButton) {
        isPanning = true;
        lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QWidget::mousePressEvent(event);
}

void ImageCompareWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

void ImageCompareWidget::wheelEvent(QWheelEvent *event)
{
    if (hasImages) {
        // Get the mouse position for zoom center
        QPoint mousePos = event->position().toPoint();
        
        // Calculate zoom change
        double zoomChange = (event->angleDelta().y() > 0) ? ZOOM_STEP : (1.0 / ZOOM_STEP);
        double newZoomFactor = zoomFactor * zoomChange;
        
        // Clamp zoom factor
        newZoomFactor = qBound(MIN_ZOOM, newZoomFactor, MAX_ZOOM);
        
        if (newZoomFactor != zoomFactor) {
            // Calculate the point in image coordinates before zoom
            QRect widgetRect = rect();
            QPoint imageCenter = QPoint(widgetRect.width() / 2, widgetRect.height() / 2);
            QPoint mouseRelativeToCenter = mousePos - imageCenter;
            QPoint adjustedMousePos = mouseRelativeToCenter - panOffset;
            
            // Apply zoom
            double zoomRatio = newZoomFactor / zoomFactor;
            zoomFactor = newZoomFactor;
            
            // Adjust pan offset to keep the mouse position fixed
            QPoint newMousePos = QPoint(
                static_cast<int>(adjustedMousePos.x() * zoomRatio),
                static_cast<int>(adjustedMousePos.y() * zoomRatio)
            );
            panOffset += adjustedMousePos - newMousePos;
            
            update();
        }
    }
    QWidget::wheelEvent(event);
}

void ImageCompareWidget::keyPressEvent(QKeyEvent *event)
{
    if (hasImages) {
        switch (event->key()) {
            case Qt::Key_Plus:
            case Qt::Key_Equal:
                zoomIn();
                break;
            case Qt::Key_Minus:
                zoomOut();
                break;
            case Qt::Key_0:
                resetZoom();
                break;
            default:
                QWidget::keyPressEvent(event);
                return;
        }
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void ImageCompareWidget::leaveEvent(QEvent *event)
{
    // Reset reveal position when mouse leaves the widget
    revealPosition = 0.0;
    isPanning = false;
    setCursor(Qt::ArrowCursor);
    update();
    QWidget::leaveEvent(event);
}

void ImageCompareWidget::updateRevealPosition(const QPoint &mousePos)
{
    if (!hasImages) return;
    
    // Calculate the zoomed image bounds with pan offset
    QRect widgetRect = rect();
    QSize zoomedSize = QSize(
        static_cast<int>(scaledFirstImage.width() * zoomFactor),
        static_cast<int>(scaledFirstImage.height() * zoomFactor)
    );
    
    QPoint imagePos = QPoint(
        (widgetRect.width() - zoomedSize.width()) / 2 + panOffset.x(),
        (widgetRect.height() - zoomedSize.height()) / 2 + panOffset.y()
    );
    QRect imageRect(imagePos, zoomedSize);
    
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

void ImageCompareWidget::resetZoom()
{
    zoomFactor = 1.0;
    panOffset = QPoint(0, 0);
    update();
}

void ImageCompareWidget::zoomIn()
{
    double newZoomFactor = zoomFactor * ZOOM_STEP;
    if (newZoomFactor <= MAX_ZOOM) {
        zoomFactor = newZoomFactor;
        update();
    }
}

void ImageCompareWidget::zoomOut()
{
    double newZoomFactor = zoomFactor / ZOOM_STEP;
    if (newZoomFactor >= MIN_ZOOM) {
        zoomFactor = newZoomFactor;
        update();
    }
}

void ImageCompareWidget::updateImageTransforms()
{
    // This method can be used for future enhancements
    update();
}

QPoint ImageCompareWidget::mapToImageCoordinates(const QPoint &widgetPos) const
{
    if (!hasImages) return QPoint();
    
    QRect widgetRect = rect();
    QSize zoomedSize = QSize(
        static_cast<int>(scaledFirstImage.width() * zoomFactor),
        static_cast<int>(scaledFirstImage.height() * zoomFactor)
    );
    
    QPoint imagePos = QPoint(
        (widgetRect.width() - zoomedSize.width()) / 2 + panOffset.x(),
        (widgetRect.height() - zoomedSize.height()) / 2 + panOffset.y()
    );
    
    return widgetPos - imagePos;
}

void ImageCompareWidget::setCompareMode(CompareMode mode)
{
    if (compareMode != mode) {
        compareMode = mode;
        
        // Stop dissolve if switching away from dissolve mode
        if (mode != DissolveMode) {
            stopDissolve();
        }
        
        // Reset states
        revealPosition = 0.0;
        currentOpacity = 0.0;
        
        update();
    }
}

void ImageCompareWidget::setDissolveSettings(double newHoldTime, double newTransitionTime)
{
    holdTime = qMax(0.1, newHoldTime); // Minimum 0.1 seconds
    transitionTime = qMax(0.1, newTransitionTime); // Minimum 0.1 seconds
}

void ImageCompareWidget::startDissolve()
{
    if (!hasImages || compareMode != DissolveMode) return;
    
    isDissolving = true;
    showingSecondImage = false;
    currentOpacity = 0.0;
    
    // Start with first image, schedule transition to second image
    dissolveTimer->start(static_cast<int>(holdTime * 1000)); // Convert to milliseconds
    update();
}

void ImageCompareWidget::stopDissolve()
{
    isDissolving = false;
    dissolveTimer->stop();
    opacityAnimation->stop();
    currentOpacity = 0.0;
    showingSecondImage = false;
    update();
}

void ImageCompareWidget::setOpacity(double opacity)
{
    currentOpacity = qBound(0.0, opacity, 1.0);
    update();
}

void ImageCompareWidget::onDissolveTimer()
{
    if (!isDissolving || !hasImages) return;
    
    dissolveTimer->stop();
    startDissolveTransition();
}

void ImageCompareWidget::startDissolveTransition()
{
    if (!isDissolving || !hasImages) return;
    
    opacityAnimation->stop();
    
    // Disconnect any previous connections
    disconnect(opacityAnimation, &QPropertyAnimation::finished, this, nullptr);
    
    if (!showingSecondImage) {
        // Transition from first image to second image (fade in second image)
        opacityAnimation->setStartValue(0.0);
        opacityAnimation->setEndValue(1.0);
        showingSecondImage = true;
    } else {
        // Transition from second image back to first image (fade out second image)
        opacityAnimation->setStartValue(1.0);
        opacityAnimation->setEndValue(0.0);
        showingSecondImage = false;
    }
    
    opacityAnimation->setDuration(static_cast<int>(transitionTime * 1000)); // Convert to milliseconds
    opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    
    // When animation finishes, start hold timer for next phase
    connect(opacityAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (isDissolving) {
            dissolveTimer->start(static_cast<int>(holdTime * 1000));
        }
    });
    
    opacityAnimation->start();
}
