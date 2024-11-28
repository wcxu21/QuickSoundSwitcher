#include "SoundOverlay.h"
#include "ui_SoundOverlay.h"
#include "Utils.h"
#include <QTimer>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>


SoundOverlay::SoundOverlay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SoundOverlay)
    , shown(false)
    , animationTimerOut(nullptr)
    , isAnimatingOut(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);

    expireTimer = new QTimer(this);
    connect(expireTimer, &QTimer::timeout, this, &SoundOverlay::animateOut);
}

SoundOverlay::~SoundOverlay()
{
    delete ui;
}

void SoundOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setBrush(this->palette().color(QPalette::Window));
    painter.setPen(Qt::NoPen);

    QPainterPath path;
    path.addRoundedRect(this->rect().adjusted(1, 1, -1, -1), 8, 8);
    painter.drawPath(path);

    QPen borderPen(QColor(0, 0, 0, 37));
    borderPen.setWidth(1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath borderPath;
    borderPath.addRoundedRect(this->rect().adjusted(0, 0, -1, -1), 8, 8);
    painter.drawPath(borderPath);
}

void SoundOverlay::animateIn()
{
    if (isAnimatingOut) return;

    if (shown) {
        expireTimer->start(3000);
        return;
    }

    shown = true;

    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    QRect availableScreenGeometry = QApplication::primaryScreen()->availableGeometry();
    int taskbarHeight = screenGeometry.height() - availableScreenGeometry.height();
    int screenCenterX = screenGeometry.center().x();
    int margin = 12;
    int soundOverlayX = screenCenterX - this->width() / 2;
    int startY = screenGeometry.bottom() + taskbarHeight;
    int targetY = screenGeometry.bottom() - this->height() - taskbarHeight - margin;

    this->move(soundOverlayX, startY);
    this->show();

    const int durationMs = 300;
    const int refreshRate = 1;
    const double totalSteps = durationMs / refreshRate;

    int currentStep = 0;
    QTimer *animationTimer = new QTimer(this);

    animationTimer->start(refreshRate);
    connect(animationTimer, &QTimer::timeout, this, [=, this]() mutable {
        double t = static_cast<double>(currentStep) / totalSteps;
        double easedT = 1 - pow(1 - t, 3);
        int currentY = startY + easedT * (targetY - startY);

        if (currentY == targetY) {
            animationTimer->stop();
            animationTimer->deleteLater();
            Utils::setAlwaysOnTopState(this, true);
            return;
        }

        this->move(soundOverlayX, currentY);
        ++currentStep;
    });

    expireTimer->start(3000);
}

void SoundOverlay::animateOut()
{
    shown = false;
    isAnimatingOut = true;
    Utils::setAlwaysOnTopState(this, false);

    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    QRect availableScreenGeometry = QApplication::primaryScreen()->availableGeometry();
    int taskbarHeight = screenGeometry.height() - availableScreenGeometry.height();
    int screenCenterX = screenGeometry.center().x();
    int margin = 12;
    int soundOverlayX = screenCenterX - this->width() / 2;
    int startY = screenGeometry.bottom() - this->height() - taskbarHeight - margin;
    int targetY = screenGeometry.bottom() + taskbarHeight;

    const int durationMs = 300;
    const int refreshRate = 1;
    const double totalSteps = durationMs / refreshRate;

    int currentStep = 0;

    animationTimerOut = new QTimer(this);
    animationTimerOut->start(refreshRate);
    connect(animationTimerOut, &QTimer::timeout, this, [=]() mutable {
        double t = static_cast<double>(currentStep) / totalSteps;
        double easedT = 1 - pow(1 - t, 3);
        int currentY = startY + easedT * (targetY - startY);

        if (currentY == targetY) {
            animationTimerOut->stop();
            animationTimerOut->deleteLater();
            animationTimerOut = nullptr;
            isAnimatingOut = false;
            this->hide();
            emit overlayClosed();
            return;
        }

        this->move(soundOverlayX, currentY);
        ++currentStep;
    });
}

void SoundOverlay::updateVolumeIconAndLabel(QIcon icon, int volume)
{
    ui->iconLabel->setPixmap(icon.pixmap(16, 16));
    ui->valueLabel->setText(QString::number(volume));
    ui->progressBar->setValue(volume);
}

void SoundOverlay::updateMuteIcon(QIcon icon)
{
    ui->iconLabel->setPixmap(icon.pixmap(16, 16));
}

