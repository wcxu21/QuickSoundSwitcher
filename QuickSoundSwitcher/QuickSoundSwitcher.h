#ifndef QUICKSOUNDSWITCHER_H
#define QUICKSOUNDSWITCHER_H

#include "Panel.h"
#include "OverlaySettings.h"
#include "OverlayWidget.h"
#include "SoundOverlay.h"
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSettings>

class QuickSoundSwitcher : public QMainWindow
{
    Q_OBJECT

public:
    QuickSoundSwitcher(QWidget *parent = nullptr);
    ~QuickSoundSwitcher();
    static QuickSoundSwitcher* instance;
    void adjustOutputVolume(bool up);
    void toggleMuteWithKey();

public slots:
    void onVolumeChanged();
    void onOutputMuteChanged();

private slots:
    void onInputMuteChanged();
    void onPanelClosed();
    void onRunAtStartupStateChanged();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    QSystemTrayIcon *trayIcon;
    Panel* panel;
    SoundOverlay* soundOverlay;
    void createTrayIcon();
    void showPanel();
    void hidePanel();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    bool hiding;
    void createDeviceSubMenu(QMenu *parentMenu, const QList<AudioDevice> &devices, const QString &title);

    static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    static HHOOK mouseHook;
    static HHOOK keyboardHook;
    void installGlobalMouseHook();
    void uninstallGlobalMouseHook();
    void installKeyboardHook();
    void uninstallKeyboardHook();

    static const int HOTKEY_ID = 1;
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    void toggleMicMute();
    void toggleMutedOverlay(bool enabled);
    void sendNotification(bool enabled);
    void loadSettings();
    void onSettingsChanged();
    void onSettingsClosed();
    void showSettings();
    bool disableNotification;
    bool disableOverlay;
    bool potatoMode;
    bool hotkeyRegistered;
    bool disableMuteHotkey;
    bool mergeSimilarApps;
    int volumeIncrement;
    OverlayWidget *overlayWidget;
    OverlaySettings *overlaySettings;
    QSettings settings;
    QString position;

signals:
    void muteStateChanged();
    void outputMuteStateChanged(bool state);
    void volumeChangedWithTray();
};

#endif // QUICKSOUNDSWITCHER_H
