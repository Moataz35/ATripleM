#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt headers first: Image.h (via ImageEditor.h) defines helper macros for non-Qt builds.
#include <QComboBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFutureWatcher>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QSlider>
#include <QTimer>
#include <functional>
#include <memory>

#include "CanvasView.h"
#include "History.h"
#include "ImageEditor.h"
#include "ImageOps.h"
#include "Widgets.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void openFile(const QString &path);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    struct OpResult {
        std::shared_ptr<Image> image;
        QString error;
    };
    struct LoadResult {
        std::shared_ptr<ImageEditor> editor;
        QString error;
    };
    using ImageOp = std::function<void(Image &)>;

    // construction
    void buildActions();
    void buildToolbars();
    void buildDocks();
    void buildStatusBar();
    QWidget *buildAdjustPanel();
    QWidget *buildHistoryPanel();
    QWidget *buildPalettePanel();
    QAction *makeAction(QMenu *menu, const QString &text, const QKeySequence &key,
                       std::function<void()> fn, bool needsImage = true);

    void addSwatch(const QColor &c);

    // document
    bool maybeSave();
    void saveFile();
    void saveFileAs();
    void exportFile();
    void closeImage();
    bool writeTo(const QString &path);
    void onLoaded();
    void onCurrentImageChanged();
    void onHistoryChanged();

    // operations
    void runOp(const QString &name, ImageOp op, bool selectionAware = true);
    void onOpFinished();
    void setBusy(bool busy, const QString &message = QString());
    void updateUi();
    void updateInfo();
    void updateTitle();

    // preview
    ImageOps::AdjustParams currentParams() const;
    void schedulePreview();
    void startPreview();
    void resetSliders();

    // dialogs
    void resizeDialog();
    void frameDialog();
    void mergeDialog();
    void skewDialog();
    void cropSelection(const QRect &r);
    void setZoomFromText(const QString &text);
    void onZoomChanged(double z);

    Ui::MainWindow *ui;
    CanvasView *m_canvas = nullptr;
    History m_history;
    std::shared_ptr<ImageEditor> m_editor;
    QString m_path;
    QString m_pendingPath;
    int m_swatchCount = 0;

    bool m_busy = false;
    bool m_skipSync = false;
    bool m_skipCanvas = false;
    bool m_resetView = false;
    QString m_opName;
    QFutureWatcher<OpResult> m_opWatcher;
    QFutureWatcher<LoadResult> m_loadWatcher;

    // preview state
    QFutureWatcher<QImage> m_previewWatcher;
    QTimer m_previewTimer;
    bool m_previewRunning = false;
    bool m_previewPending = false;
    int m_previewGen = 0;
    int m_runningGen = 0;
    const Image *m_proxyFor = nullptr;
    std::shared_ptr<const Image> m_proxy;
    std::shared_ptr<const Image> m_proxyHold;

    // widgets
    QList<QAction *> m_imageActions;
    QAction *m_undoAct = nullptr;
    QAction *m_redoAct = nullptr;
    QList<QAction *> m_toolActs;
    QWidget *m_adjustPanel = nullptr;
    SliderRow *m_brightness = nullptr, *m_contrast = nullptr, *m_saturation = nullptr;
    SliderRow *m_hue = nullptr, *m_blur = nullptr, *m_sharpen = nullptr, *m_edge = nullptr;
    QListWidget *m_historyList = nullptr;
    ColorSwatchWidget *m_swatch = nullptr;
    QLineEdit *m_hexEdit = nullptr;
    QWidget *m_swatchGrid = nullptr;
    QLabel *m_posLabel = nullptr, *m_infoLabel = nullptr, *m_busyLabel = nullptr;
    SpinnerWidget *m_spinner = nullptr;
    QComboBox *m_zoomCombo = nullptr;
    QSlider *m_zoomSlider = nullptr;
};

#endif // MAINWINDOW_H
