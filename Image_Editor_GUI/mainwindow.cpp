#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QActionGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <cmath>

#include "ImageConvert.h"

namespace {

struct FilterDef {
    const char *name;
    std::function<void(Image &)> op;
    bool selectionAware;
};

const std::vector<FilterDef> &quickFilters()
{
    static const std::vector<FilterDef> list = {
        {"Grayscale", [](Image &i) { ImageFilter::grayscale(i); }, true},
        {"Black && White", [](Image &i) { ImageFilter::BlackandWhite(i); }, true},
        {"Invert", [](Image &i) { ImageFilter::Invert(i); }, true},
        {"Darken", [](Image &i) { ImageFilter::DarkenandLighten(i, -50); }, true},
        {"Lighten", [](Image &i) { ImageFilter::DarkenandLighten(i, 50); }, true},
        {"Sunlight", [](Image &i) { ImageFilter::sunLightEffect(i); }, true},
        {"Infrared", [](Image &i) { ImageFilter::Infrared(i); }, true},
        {"Purple Effect", [](Image &i) { ImageFilter::PurpleEffect(i); }, true},
        {"Old TV", [](Image &i) { ImageFilter::OldTV(i); }, true},
        {"Oil Painting", [](Image &i) { ImageOps::oilPainting(i, 3, 20); }, true},
    };
    return list;
}

const std::vector<FilterDef> &transforms()
{
    static const std::vector<FilterDef> list = {
        {"Rotate 90°", [](Image &i) { ImageFilter::Rotate90(i); }, false},
        {"Rotate 180°", [](Image &i) { ImageFilter::Rotate180(i); }, false},
        {"Rotate 270°", [](Image &i) { ImageFilter::Rotate270(i); }, false},
        {"Flip Horizontal", [](Image &i) { ImageFilter::FlipHorizontally(i); }, true},
        {"Flip Vertical", [](Image &i) { ImageFilter::FlipVertically(i); }, true},
    };
    return list;
}

Color toColor(const QColor &c)
{
    return Color{static_cast<unsigned char>(c.red()), static_cast<unsigned char>(c.green()),
                 static_cast<unsigned char>(c.blue())};
}

const char *kOpenFilter = "Images (*.png *.jpg *.jpeg *.bmp *.tga);;All files (*)";
const char *kSaveFilter = "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;TGA (*.tga)";

} // namespace

// ============================================================ construction

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1440, 900);
    setAcceptDrops(true);
    setDockOptions(AllowTabbedDocks | AnimatedDocks | AllowNestedDocks);

    m_canvas = new CanvasView(this);
    setCentralWidget(m_canvas);

    buildActions();
    buildToolbars();
    buildDocks();
    buildStatusBar();

    connect(&m_history, &History::changed, this, &MainWindow::onHistoryChanged);
    connect(&m_history, &History::currentImageChanged, this, &MainWindow::onCurrentImageChanged);
    connect(&m_opWatcher, &QFutureWatcher<OpResult>::finished, this, &MainWindow::onOpFinished);
    connect(&m_loadWatcher, &QFutureWatcher<LoadResult>::finished, this, &MainWindow::onLoaded);
    connect(&m_previewWatcher, &QFutureWatcher<QImage>::finished, this, [this]() {
        m_previewRunning = false;
        const QImage r = m_previewWatcher.result();
        if (m_runningGen == m_previewGen && !r.isNull() && !currentParams().isIdentity())
            m_canvas->setPreviewImage(r);
        if (m_previewPending) {
            m_previewPending = false;
            startPreview();
        }
    });
    m_previewTimer.setSingleShot(true);
    m_previewTimer.setInterval(40);
    connect(&m_previewTimer, &QTimer::timeout, this, &MainWindow::startPreview);

    connect(m_canvas, &CanvasView::filesDropped, this, [this](const QStringList &p) { openFile(p.first()); });
    connect(m_canvas, &CanvasView::zoomChanged, this, &MainWindow::onZoomChanged);
    connect(m_canvas, &CanvasView::cropRequested, this, &MainWindow::cropSelection);
    connect(m_canvas, &CanvasView::selectionChanged, this, [this]() {
        updateInfo();
        schedulePreview();
    });
    connect(m_canvas, &CanvasView::cursorPosChanged, this, [this](const QPoint &p, bool inside) {
        m_posLabel->setText(inside ? tr("X: %1   Y: %2").arg(p.x()).arg(p.y()) : tr("X: –   Y: –"));
    });
    connect(m_canvas, &CanvasView::colorPicked, this, [this](const QColor &c, bool bg) {
        if (bg) m_swatch->setBackground(c); else m_swatch->setForeground(c);
    });
    connect(m_canvas, &CanvasView::strokeFinished, this, [this](const QImage &img, const QString &name) {
        if (!m_history.current() || m_busy)
            return;
        auto converted = ImageConvert::fromQImage(img);
        m_skipCanvas = true; // the canvas already shows the painted result
        m_history.push(name, converted);
        m_skipCanvas = false;
    });

    updateUi();
    updateTitle();
    onZoomChanged(1.0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QAction *MainWindow::makeAction(QMenu *menu, const QString &text, const QKeySequence &key,
                                std::function<void()> fn, bool needsImage)
{
    QAction *a = menu->addAction(text);
    if (!key.isEmpty())
        a->setShortcut(key);
    connect(a, &QAction::triggered, this, [fn]() { fn(); });
    if (needsImage)
        m_imageActions << a;
    return a;
}

void MainWindow::buildActions()
{
    QMenu *file = menuBar()->addMenu(tr("&File"));
    makeAction(file, tr("&Open…"), QKeySequence::Open, [this]() {
        const QString p = QFileDialog::getOpenFileName(this, tr("Open Image"), QFileInfo(m_path).absolutePath(),
                                                       tr(kOpenFilter));
        if (!p.isEmpty()) openFile(p);
    }, false);
    file->addSeparator();
    makeAction(file, tr("&Save"), QKeySequence::Save, [this]() { saveFile(); });
    makeAction(file, tr("Save &As…"), QKeySequence::SaveAs, [this]() { saveFileAs(); });
    makeAction(file, tr("&Export…"), QKeySequence("Ctrl+Shift+E"), [this]() { exportFile(); });
    file->addSeparator();
    makeAction(file, tr("&Close Image"), QKeySequence("Ctrl+W"), [this]() { closeImage(); });
    makeAction(file, tr("E&xit"), QKeySequence("Ctrl+Q"), [this]() { close(); }, false);

    QMenu *edit = menuBar()->addMenu(tr("&Edit"));
    m_undoAct = makeAction(edit, tr("&Undo"), QKeySequence::Undo, [this]() { m_history.undo(); }, false);
    m_redoAct = makeAction(edit, tr("&Redo"), QKeySequence("Ctrl+Y"), [this]() { m_history.redo(); }, false);
    m_redoAct->setShortcuts({QKeySequence("Ctrl+Y"), QKeySequence("Ctrl+Shift+Z")});
    makeAction(edit, tr("Revert to &Original"), QKeySequence("Ctrl+Shift+R"), [this]() {
        if (!m_editor) return;
        try {
            m_editor->reset();
        } catch (const std::exception &e) {
            QMessageBox::warning(this, tr("Revert"), QString::fromLocal8Bit(e.what()));
            return;
        }
        m_history.push(tr("Revert to Original"), std::make_shared<Image>(m_editor->getImage()));
    });
    edit->addSeparator();
    makeAction(edit, tr("Select &All"), QKeySequence::SelectAll, [this]() { m_canvas->selectAll(); });
    makeAction(edit, tr("&Deselect"), QKeySequence("Ctrl+D"), [this]() { m_canvas->clearSelection(); });
    makeAction(edit, tr("Crop to &Selection"), QKeySequence("Ctrl+Shift+X"), [this]() {
        if (m_canvas->selection().isEmpty())
            statusBar()->showMessage(tr("Make a selection first (Select or Crop tool)."), 3000);
        else
            cropSelection(m_canvas->selection());
    });
    edit->addSeparator();
    makeAction(edit, tr("Swap Colors"), QKeySequence("X"), [this]() { m_swatch->swap(); }, false);

    QMenu *image = menuBar()->addMenu(tr("&Image"));
    for (const FilterDef &t : transforms()) {
        const QString name = QString::fromUtf8(t.name);
        makeAction(image, name, QKeySequence(), [this, name, op = t.op, aware = t.selectionAware]() {
            runOp(name, op, aware);
        });
    }
    image->addSeparator();
    makeAction(image, tr("&Resize…"), QKeySequence("Ctrl+Alt+I"), [this]() { resizeDialog(); });
    makeAction(image, tr("Add &Frame…"), QKeySequence(), [this]() { frameDialog(); });
    makeAction(image, tr("&Merge with Image…"), QKeySequence(), [this]() { mergeDialog(); });
    makeAction(image, tr("S&kew…"), QKeySequence(), [this]() { skewDialog(); });

    QMenu *filters = menuBar()->addMenu(tr("&Filters"));
    for (const FilterDef &f : quickFilters()) {
        QString name = QString::fromUtf8(f.name);
        name.replace("&&", "&");
        makeAction(filters, name, QKeySequence(), [this, name, op = f.op]() { runOp(name, op); });
    }
    makeAction(filters, tr("Detect &Edges"), QKeySequence(), [this]() {
        const int t = m_edge->value();
        runOp(tr("Edge Detection"), [t](Image &i) { ImageFilter::EdgeDetection(i, static_cast<unsigned char>(t)); });
    });
    filters->addSeparator();
    makeAction(filters, tr("&Blur (Level 1)"), QKeySequence(), [this]() {
        runOp(tr("Blur"), [](Image &i) { ImageFilter::Blur(i, 1); });
    });
    makeAction(filters, tr("&Sharpen"), QKeySequence(), [this]() {
        runOp(tr("Sharpen"), [](Image &i) { ImageOps::sharpen(i, 50); });
    });

    QMenu *view = menuBar()->addMenu(tr("&View"));
    view->setObjectName("viewMenu");
    makeAction(view, tr("Zoom &In"), QKeySequence("Ctrl+="), [this]() { m_canvas->zoomIn(); });
    makeAction(view, tr("Zoom &Out"), QKeySequence("Ctrl+-"), [this]() { m_canvas->zoomOut(); });
    makeAction(view, tr("&Fit to Screen"), QKeySequence("Ctrl+0"), [this]() { m_canvas->fitToScreen(); });
    makeAction(view, tr("&Actual Size (100%)"), QKeySequence("Ctrl+1"), [this]() { m_canvas->actualSize(); });
    view->addSeparator();

    QMenu *help = menuBar()->addMenu(tr("&Help"));
    makeAction(help, tr("&About"), QKeySequence(), [this]() {
        QMessageBox::about(this, tr("About QtPhoto"),
                           tr("<b>QtPhoto</b><br>A Photoshop-style editor built on Qt 6 and a custom "
                              "C++ image engine (stb_image)."));
    }, false);
}

void MainWindow::buildToolbars()
{
    auto *tools = addToolBar(tr("Tools"));
    tools->setObjectName("toolsBar");
    tools->setMovable(false);
    tools->setOrientation(Qt::Vertical);
    tools->setIconSize(QSize(26, 26));
    addToolBar(Qt::LeftToolBarArea, tools);

    struct ToolDef { Tool tool; const char *name; const char *key; };
    const ToolDef defs[] = {
        {Tool::Hand, "Pan / Hand", "H"},       {Tool::Zoom, "Zoom", "Z"},
        {Tool::Crop, "Crop", "C"},             {Tool::Select, "Rectangular Select", "M"},
        {Tool::Picker, "Color Picker", "I"},   {Tool::Brush, "Brush / Pen", "B"},
        {Tool::Eraser, "Eraser", "E"},
    };
    auto *group = new QActionGroup(this);
    group->setExclusive(true);
    for (const ToolDef &d : defs) {
        QAction *a = tools->addAction(Theme::toolIcon(d.tool), QString::fromUtf8(d.name));
        a->setCheckable(true);
        a->setShortcut(QKeySequence(QString::fromUtf8(d.key)));
        a->setToolTip(QStringLiteral("%1 (%2)").arg(QString::fromUtf8(d.name), QString::fromUtf8(d.key)));
        group->addAction(a);
        m_toolActs << a;
        connect(a, &QAction::triggered, this, [this, t = d.tool]() { m_canvas->setTool(t); });
        if (d.tool == Tool::Select || d.tool == Tool::Picker)
            tools->addSeparator();
    }
    m_toolActs.first()->setChecked(true);

    auto *options = new QToolBar(tr("Tool Options"), this);
    options->setObjectName("optionsBar");
    options->setMovable(false);
    addToolBar(Qt::TopToolBarArea, options);
    options->addWidget(new QLabel(tr("Brush size")));
    auto *size = new QSpinBox;
    size->setRange(1, 500);
    size->setValue(12);
    size->setSuffix(" px");
    options->addWidget(size);
    options->addSeparator();
    options->addWidget(new QLabel(tr("Opacity")));
    auto *opacity = new QSpinBox;
    opacity->setRange(1, 100);
    opacity->setValue(100);
    opacity->setSuffix(" %");
    options->addWidget(opacity);
    connect(size, &QSpinBox::valueChanged, m_canvas, &CanvasView::setBrushSize);
    connect(opacity, &QSpinBox::valueChanged, m_canvas, &CanvasView::setBrushOpacity);
    m_canvas->setBrushSize(size->value());
}

void MainWindow::buildDocks()
{
    auto make = [this](const QString &title, const QString &name, QWidget *content) {
        auto *d = new QDockWidget(title, this);
        d->setObjectName(name);
        d->setWidget(content);
        d->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |
                       QDockWidget::DockWidgetClosable);
        return d;
    };
    auto *adj = make(tr("Filters && Adjustments").replace("&&", "&"), "adjustDock", buildAdjustPanel());
    auto *hist = make(tr("History"), "historyDock", buildHistoryPanel());
    auto *pal = make(tr("Color Palette"), "paletteDock", buildPalettePanel());
    adj->setMinimumWidth(300);

    addDockWidget(Qt::RightDockWidgetArea, adj);
    addDockWidget(Qt::RightDockWidgetArea, hist);
    addDockWidget(Qt::RightDockWidgetArea, pal);
    splitDockWidget(adj, hist, Qt::Vertical);
    tabifyDockWidget(hist, pal);
    hist->raise();
    resizeDocks({adj, hist}, {600, 260}, Qt::Vertical);
    resizeDocks({adj}, {330}, Qt::Horizontal);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);

    if (auto *view = findChild<QMenu *>("viewMenu")) {
        for (QDockWidget *d : {adj, hist, pal})
            view->addAction(d->toggleViewAction());
    }
}

QWidget *MainWindow::buildAdjustPanel()
{
    auto group = [](const QString &title, QWidget *parent) {
        auto *g = new QGroupBox(title, parent);
        g->setLayout(new QVBoxLayout);
        return g;
    };
    m_adjustPanel = new QWidget;
    auto *root = new QVBoxLayout(m_adjustPanel);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(10);

    // --- real-time adjustments
    auto *adj = group(tr("Adjustments"), m_adjustPanel);
    m_brightness = new SliderRow(tr("Brightness"), -100, 100);
    m_contrast = new SliderRow(tr("Contrast"), -100, 100);
    m_saturation = new SliderRow(tr("Saturation"), -100, 100);
    m_hue = new SliderRow(tr("Hue"), -180, 180, 0, "°");
    m_blur = new SliderRow(tr("Blur"), 0, 10);
    m_sharpen = new SliderRow(tr("Sharpen"), 0, 100);
    for (SliderRow *r : {m_brightness, m_contrast, m_saturation, m_hue, m_blur, m_sharpen}) {
        adj->layout()->addWidget(r);
        connect(r, &SliderRow::valueChanged, this, &MainWindow::schedulePreview);
    }
    auto *row = new QHBoxLayout;
    auto *apply = new QPushButton(tr("Apply"));
    apply->setObjectName("primaryButton");
    auto *reset = new QPushButton(tr("Reset"));
    row->addWidget(reset);
    row->addWidget(apply);
    static_cast<QVBoxLayout *>(adj->layout())->addLayout(row);
    connect(reset, &QPushButton::clicked, this, [this]() {
        resetSliders();
        m_canvas->clearPreview();
        ++m_previewGen;
    });
    connect(apply, &QPushButton::clicked, this, [this]() {
        const auto p = currentParams();
        if (p.isIdentity()) return;
        runOp(tr("Adjustments"), [p](Image &i) { ImageOps::applyAdjustments(i, p); });
    });
    root->addWidget(adj);

    // --- one-click filters
    auto *quick = group(tr("Quick Filters"), m_adjustPanel);
    auto *grid = new QGridLayout;
    grid->setSpacing(6);
    int idx = 0;
    for (const FilterDef &f : quickFilters()) {
        auto *b = new QPushButton(QString::fromUtf8(f.name).replace("&&", "&&"));
        b->setObjectName("filterButton");
        QString plain = QString::fromUtf8(f.name);
        plain.replace("&&", "&");
        connect(b, &QPushButton::clicked, this, [this, plain, op = f.op]() { runOp(plain, op); });
        grid->addWidget(b, idx / 2, idx % 2);
        ++idx;
    }
    static_cast<QVBoxLayout *>(quick->layout())->addLayout(grid);
    root->addWidget(quick);

    // --- edge detection
    auto *edge = group(tr("Edge Detection"), m_adjustPanel);
    m_edge = new SliderRow(tr("Threshold"), 1, 255, 100);
    auto *edgeBtn = new QPushButton(tr("Detect Edges"));
    edgeBtn->setObjectName("filterButton");
    edge->layout()->addWidget(m_edge);
    edge->layout()->addWidget(edgeBtn);
    connect(edgeBtn, &QPushButton::clicked, this, [this]() {
        const int t = m_edge->value();
        runOp(tr("Edge Detection"), [t](Image &i) { ImageFilter::EdgeDetection(i, static_cast<unsigned char>(t)); });
    });
    root->addWidget(edge);

    // --- transforms
    auto *tr_ = group(tr("Transform"), m_adjustPanel);
    auto *tgrid = new QGridLayout;
    tgrid->setSpacing(6);
    idx = 0;
    for (const FilterDef &t : transforms()) {
        const QString name = QString::fromUtf8(t.name);
        auto *b = new QPushButton(name);
        b->setObjectName("filterButton");
        connect(b, &QPushButton::clicked, this, [this, name, op = t.op, aware = t.selectionAware]() {
            runOp(name, op, aware);
        });
        tgrid->addWidget(b, idx / 2, idx % 2);
        ++idx;
    }
    static_cast<QVBoxLayout *>(tr_->layout())->addLayout(tgrid);
    root->addWidget(tr_);

    auto *hint = new QLabel(tr("Filters apply to the selection when one exists.\nDouble-click a value badge to reset it."));
    hint->setObjectName("hintLabel");
    hint->setWordWrap(true);
    root->addWidget(hint);
    root->addStretch(1);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(m_adjustPanel);
    return scroll;
}

QWidget *MainWindow::buildHistoryPanel()
{
    m_historyList = new QListWidget;
    m_historyList->setUniformItemSizes(true);
    connect(m_historyList, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0) return;
        if (m_busy) {
            onHistoryChanged();
            return;
        }
        m_history.jumpTo(row);
    });
    return m_historyList;
}

QWidget *MainWindow::buildPalettePanel()
{
    auto *w = new QWidget;
    auto *lay = new QVBoxLayout(w);
    lay->setContentsMargins(10, 10, 10, 10);
    lay->setSpacing(10);

    m_swatch = new ColorSwatchWidget;
    m_hexEdit = new QLineEdit("#000000");
    m_hexEdit->setMaxLength(7);
    auto *top = new QHBoxLayout;
    top->addWidget(m_swatch);
    auto *side = new QVBoxLayout;
    side->addWidget(new QLabel(tr("Foreground (hex)")));
    side->addWidget(m_hexEdit);
    side->addStretch(1);
    top->addLayout(side, 1);
    lay->addLayout(top);

    auto *buttons = new QGridLayout;
    auto *custom = new QPushButton(tr("Custom Color…"));
    auto *add = new QPushButton(tr("Add to Swatches"));
    auto *swap = new QPushButton(tr("Swap (X)"));
    auto *def = new QPushButton(tr("Black / White"));
    buttons->addWidget(custom, 0, 0);
    buttons->addWidget(add, 0, 1);
    buttons->addWidget(swap, 1, 0);
    buttons->addWidget(def, 1, 1);
    lay->addLayout(buttons);

    auto *label = new QLabel(tr("Swatches — click: foreground, right-click / Shift: background"));
    label->setObjectName("hintLabel");
    label->setWordWrap(true);
    lay->addWidget(label);

    m_swatchGrid = new QWidget;
    auto *grid = new QGridLayout(m_swatchGrid);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(4);
    lay->addWidget(m_swatchGrid);
    lay->addStretch(1);

    const char *initial[] = {"#000000", "#404040", "#808080", "#c0c0c0", "#ffffff", "#7f0000", "#ff0000",
                             "#ff7f00", "#ffd400", "#ffff00", "#7fff00", "#00a000", "#00ff7f", "#00ffff",
                             "#007acc", "#0000ff", "#4b0082", "#8b00ff", "#ff00ff", "#ff69b4", "#8b4513",
                             "#d2b48c", "#2f4f4f", "#f5f5dc"};
    for (const char *c : initial)
        addSwatch(QColor(c));

    connect(m_swatch, &ColorSwatchWidget::foregroundChanged, this, [this](const QColor &c) {
        m_canvas->setForegroundColor(c);
        QSignalBlocker b(m_hexEdit);
        m_hexEdit->setText(c.name().toUpper());
    });
    connect(m_swatch, &ColorSwatchWidget::backgroundChanged, m_canvas, &CanvasView::setBackgroundColor);
    connect(m_hexEdit, &QLineEdit::textEdited, this, [this](QString text) {
        if (!text.startsWith('#')) text.prepend('#');
        const QColor c(text);
        if (text.size() == 7 && c.isValid()) m_swatch->setForeground(c);
    });
    connect(custom, &QPushButton::clicked, this, [this]() {
        const QColor c = QColorDialog::getColor(m_swatch->foreground(), this, tr("Choose Color"));
        if (c.isValid()) m_swatch->setForeground(c);
    });
    connect(add, &QPushButton::clicked, this, [this]() { addSwatch(m_swatch->foreground()); });
    connect(swap, &QPushButton::clicked, m_swatch, &ColorSwatchWidget::swap);
    connect(def, &QPushButton::clicked, m_swatch, &ColorSwatchWidget::resetDefaults);
    return w;
}

void MainWindow::addSwatch(const QColor &c)
{
    auto *grid = static_cast<QGridLayout *>(m_swatchGrid->layout());
    const int columns = 8;
    auto *b = new QToolButton;
    b->setFixedSize(26, 26);
    b->setCursor(Qt::PointingHandCursor);
    b->setToolTip(c.name().toUpper());
    const QString css = QStringLiteral(
        "QToolButton{background-color:%1;border:1px solid #444;border-radius:4px;}"
        "QToolButton:hover{background-color:%1;border:2px solid #ffffff;}").arg(c.name());
    b->setStyleSheet(css);
    b->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(b, &QToolButton::clicked, this, [this, c]() {
        if (QApplication::keyboardModifiers() & Qt::ShiftModifier) m_swatch->setBackground(c);
        else m_swatch->setForeground(c);
    });
    connect(b, &QToolButton::customContextMenuRequested, this, [this, c]() { m_swatch->setBackground(c); });
    grid->addWidget(b, m_swatchCount / columns, m_swatchCount % columns);
    ++m_swatchCount;
}

void MainWindow::buildStatusBar()
{
    QStatusBar *sb = statusBar();
    sb->setSizeGripEnabled(false);

    m_spinner = new SpinnerWidget;
    m_busyLabel = new QLabel;
    sb->addWidget(m_spinner);
    sb->addWidget(m_busyLabel);

    m_posLabel = new QLabel(tr("X: –   Y: –"));
    m_posLabel->setMinimumWidth(120);
    m_infoLabel = new QLabel(tr("No image"));
    m_infoLabel->setObjectName("statusInfo");
    sb->addPermanentWidget(m_posLabel);
    sb->addPermanentWidget(m_infoLabel);

    auto *fit = new QToolButton;
    fit->setText(tr("Fit"));
    fit->setToolTip(tr("Fit to Screen (Ctrl+0)"));
    auto *actual = new QToolButton;
    actual->setText(tr("1:1"));
    actual->setToolTip(tr("Actual Size (Ctrl+1)"));
    connect(fit, &QToolButton::clicked, m_canvas, &CanvasView::fitToScreen);
    connect(actual, &QToolButton::clicked, m_canvas, &CanvasView::actualSize);

    m_zoomCombo = new QComboBox;
    m_zoomCombo->setEditable(true);
    m_zoomCombo->setInsertPolicy(QComboBox::NoInsert);
    m_zoomCombo->addItems({"25%", "50%", "100%", "200%", "400%", "Fit Screen"});
    m_zoomCombo->setFixedWidth(96);
    m_zoomCombo->setCurrentText("100%");
    connect(m_zoomCombo, &QComboBox::textActivated, this, &MainWindow::setZoomFromText);

    m_zoomSlider = new QSlider(Qt::Horizontal);
    m_zoomSlider->setRange(5, 800);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setFixedWidth(150);
    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int v) { m_canvas->setZoom(v / 100.0); });

    sb->addPermanentWidget(fit);
    sb->addPermanentWidget(actual);
    sb->addPermanentWidget(m_zoomCombo);
    sb->addPermanentWidget(m_zoomSlider);
}

// ============================================================ zoom UI

void MainWindow::setZoomFromText(const QString &text)
{
    if (text.startsWith("Fit", Qt::CaseInsensitive)) {
        m_canvas->fitToScreen();
        return;
    }
    QString t = text;
    t.remove('%');
    bool ok = false;
    const double v = t.trimmed().toDouble(&ok);
    if (ok && v > 0)
        m_canvas->setZoom(v / 100.0);
    m_canvas->setFocus();
}

void MainWindow::onZoomChanged(double z)
{
    const int pct = static_cast<int>(std::lround(z * 100.0));
    {
        QSignalBlocker b(m_zoomCombo);
        m_zoomCombo->setEditText(QString::number(pct) + "%");
    }
    {
        QSignalBlocker b(m_zoomSlider);
        m_zoomSlider->setValue(std::clamp(pct, 5, 800));
    }
}

// ============================================================ document

void MainWindow::openFile(const QString &path)
{
    if (m_busy) {
        statusBar()->showMessage(tr("Please wait until the current operation finishes."), 3000);
        return;
    }
    if (!maybeSave())
        return;
    m_pendingPath = path;
    setBusy(true, tr("Opening %1…").arg(QFileInfo(path).fileName()));
    m_loadWatcher.setFuture(QtConcurrent::run([path]() -> LoadResult {
        LoadResult r;
        r.editor = ImageOps::loadEditor(path, &r.error);
        return r;
    }));
}

void MainWindow::onLoaded()
{
    setBusy(false);
    const LoadResult r = m_loadWatcher.result();
    if (!r.editor) {
        QMessageBox::warning(this, tr("Open Image"), tr("Could not open the image:\n%1").arg(r.error));
        return;
    }
    m_editor = r.editor;
    m_path = m_pendingPath;
    m_skipSync = true;
    m_resetView = true;
    m_history.reset(tr("Open"), std::make_shared<Image>(m_editor->getImage()));
    m_skipSync = false;
    m_resetView = false;
    statusBar()->showMessage(tr("Opened %1").arg(QFileInfo(m_path).fileName()), 3000);
}

bool MainWindow::maybeSave()
{
    if (!m_editor || !m_editor->isModified())
        return true;
    const auto r = QMessageBox::question(this, tr("Unsaved changes"),
                                         tr("Save changes to %1?").arg(QFileInfo(m_path).fileName()),
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (r == QMessageBox::Cancel)
        return false;
    if (r == QMessageBox::Save) {
        saveFile();
        return !m_editor->isModified();
    }
    return true;
}

bool MainWindow::writeTo(const QString &path)
{
    QString err;
    if (!ImageOps::saveEditor(*m_editor, path, &err)) {
        QMessageBox::warning(this, tr("Save"), tr("Could not save the image:\n%1").arg(err));
        return false;
    }
    m_path = path;
    updateTitle();
    statusBar()->showMessage(tr("Saved %1").arg(QFileInfo(path).fileName()), 3000);
    return true;
}

void MainWindow::saveFile()
{
    if (!m_editor || m_busy) return;
    static const QStringList ok{"png", "jpg", "jpeg", "bmp", "tga"};
    if (m_path.isEmpty() || !ok.contains(QFileInfo(m_path).suffix().toLower()))
        saveFileAs();
    else
        writeTo(m_path);
}

static QString chooseSavePath(QWidget *parent, const QString &title, const QString &startPath, QString *selected)
{
    QString path = QFileDialog::getSaveFileName(parent, title, startPath, QObject::tr(kSaveFilter), selected);
    if (path.isEmpty())
        return path;
    static const QStringList ok{"png", "jpg", "jpeg", "bmp", "tga"};
    if (!ok.contains(QFileInfo(path).suffix().toLower())) {
        const QString f = selected ? *selected : QString();
        path += f.startsWith("JPEG") ? ".jpg" : f.startsWith("BMP") ? ".bmp" : f.startsWith("TGA") ? ".tga" : ".png";
    }
    return path;
}

void MainWindow::saveFileAs()
{
    if (!m_editor || m_busy) return;
    QString filter;
    const QString start = m_path.isEmpty() ? QStringLiteral("untitled.png") : m_path;
    const QString path = chooseSavePath(this, tr("Save As"), start, &filter);
    if (!path.isEmpty())
        writeTo(path);
}

void MainWindow::exportFile()
{
    auto cur = m_history.current();
    if (!cur || m_busy) return;
    QString filter;
    const QString path = chooseSavePath(this, tr("Export"), QFileInfo(m_path).completeBaseName() + ".png", &filter);
    if (path.isEmpty()) return;
    int quality = 90;
    const QString ext = QFileInfo(path).suffix().toLower();
    if (ext == "jpg" || ext == "jpeg") {
        bool ok = false;
        quality = QInputDialog::getInt(this, tr("JPEG Quality"), tr("Quality (1-100):"), 90, 1, 100, 1, &ok);
        if (!ok) return;
    }
    QString err;
    if (!ImageOps::writeImage(*cur, path, quality, &err))
        QMessageBox::warning(this, tr("Export"), tr("Could not export the image:\n%1").arg(err));
    else
        statusBar()->showMessage(tr("Exported %1").arg(QFileInfo(path).fileName()), 3000);
}

void MainWindow::closeImage()
{
    if (m_busy || !maybeSave()) return;
    if (m_editor) m_editor->unloadImage();
    m_editor.reset();
    m_path.clear();
    m_history.clear();
    m_canvas->clearImage();
    updateUi();
    updateInfo();
    updateTitle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) event->accept(); else event->ignore();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (!CanvasView::imagePathsFromMime(event->mimeData()).isEmpty())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QStringList paths = CanvasView::imagePathsFromMime(event->mimeData());
    if (!paths.isEmpty()) {
        event->acceptProposedAction();
        openFile(paths.first());
    }
}

// ============================================================ history & state

void MainWindow::onCurrentImageChanged()
{
    ++m_previewGen;
    const auto cur = m_history.current();
    if (!cur) {
        updateUi();
        return;
    }
    if (!m_skipSync && m_editor)
        m_editor->setImage(*cur);
    if (!m_skipCanvas)
        m_canvas->setImage(ImageConvert::toQImage(*cur), m_resetView);
    m_canvas->clearPreview();
    resetSliders();
    updateInfo();
    updateTitle();
    updateUi();
}

void MainWindow::onHistoryChanged()
{
    QSignalBlocker block(m_historyList);
    m_historyList->clear();
    for (int i = 0; i < m_history.count(); ++i) {
        auto *item = new QListWidgetItem(QStringLiteral("%1.  %2").arg(i + 1).arg(m_history.name(i)));
        if (i > m_history.index())
            item->setForeground(QColor("#666666"));
        m_historyList->addItem(item);
    }
    m_historyList->setCurrentRow(m_history.index());
    updateUi();
}

void MainWindow::updateUi()
{
    const bool has = m_history.current() != nullptr;
    for (QAction *a : std::as_const(m_imageActions))
        a->setEnabled(has && !m_busy);
    m_undoAct->setEnabled(!m_busy && m_history.canUndo());
    m_redoAct->setEnabled(!m_busy && m_history.canRedo());
    m_adjustPanel->setEnabled(has && !m_busy);
    m_historyList->setEnabled(has);
    m_zoomCombo->setEnabled(has);
    m_zoomSlider->setEnabled(has);
}

void MainWindow::updateInfo()
{
    const auto cur = m_history.current();
    if (!cur) {
        m_infoLabel->setText(tr("No image"));
        return;
    }
    QString text = tr("%1 × %2 px   |   %3 (%4 ch)   |   %5")
                       .arg(cur->width).arg(cur->height)
                       .arg(ImageConvert::channelLabel(cur->channels)).arg(cur->channels)
                       .arg(ImageConvert::formatBytes(ImageConvert::memoryBytes(*cur)));
    const QRect s = m_canvas->selection();
    if (!s.isEmpty())
        text += tr("   |   Selection %1 × %2").arg(s.width()).arg(s.height());
    m_infoLabel->setText(text);
}

void MainWindow::updateTitle()
{
    if (!m_history.current()) {
        setWindowTitle("QtPhoto");
        setWindowModified(false);
        return;
    }
    const QString name = m_path.isEmpty() ? tr("Untitled") : QFileInfo(m_path).fileName();
    setWindowTitle(name + "[*] — QtPhoto");
    setWindowModified(m_editor && m_editor->isModified());
}

void MainWindow::setBusy(bool busy, const QString &message)
{
    m_busy = busy;
    m_busyLabel->setText(busy ? message : QString());
    if (busy) m_spinner->start(); else m_spinner->stop();
    m_canvas->setPaintingEnabled(!busy);
    updateUi();
}

// ============================================================ operations

void MainWindow::runOp(const QString &name, ImageOp op, bool selectionAware)
{
    const auto base = m_history.current();
    if (!base || m_busy) return;
    const QRect sel = selectionAware ? m_canvas->selection() : QRect();
    m_opName = name;
    setBusy(true, tr("Applying %1…").arg(name));
    m_opWatcher.setFuture(QtConcurrent::run([base, op, sel]() -> OpResult {
        OpResult r;
        try {
            auto img = std::make_shared<Image>(*base);
            if (!sel.isEmpty())
                ImageOps::applyInRegion(*img, sel.x(), sel.y(), sel.width(), sel.height(), op);
            else
                op(*img);
            r.image = img;
        } catch (const std::exception &e) {
            r.error = QString::fromLocal8Bit(e.what());
        } catch (...) {
            r.error = QStringLiteral("Unknown error");
        }
        return r;
    }));
}

void MainWindow::onOpFinished()
{
    setBusy(false);
    const OpResult r = m_opWatcher.result();
    if (!r.image) {
        QMessageBox::warning(this, m_opName, tr("The operation failed:\n%1").arg(r.error));
        return;
    }
    m_history.push(m_opName, r.image);
    statusBar()->showMessage(tr("%1 applied").arg(m_opName), 2500);
}

void MainWindow::cropSelection(const QRect &r)
{
    if (r.isEmpty()) return;
    runOp(tr("Crop"), [r](Image &i) { ImageFilter::Crop(i, r.x(), r.y(), r.width(), r.height()); }, false);
}

// ============================================================ real-time preview

ImageOps::AdjustParams MainWindow::currentParams() const
{
    ImageOps::AdjustParams p;
    p.brightness = m_brightness->value();
    p.contrast = m_contrast->value();
    p.saturation = m_saturation->value();
    p.hue = m_hue->value();
    p.blur = m_blur->value();
    p.sharpen = m_sharpen->value();
    return p;
}

void MainWindow::resetSliders()
{
    for (SliderRow *r : {m_brightness, m_contrast, m_saturation, m_hue, m_blur, m_sharpen}) {
        QSignalBlocker b(r);
        r->reset();
    }
}

void MainWindow::schedulePreview()
{
    m_previewTimer.start();
}

void MainWindow::startPreview()
{
    const auto cur = m_history.current();
    if (!cur || m_busy) return;
    ImageOps::AdjustParams p = currentParams();
    if (p.isIdentity()) {
        ++m_previewGen;
        m_canvas->clearPreview();
        return;
    }
    if (m_previewRunning) {
        m_previewPending = true;
        return;
    }
    if (m_proxyFor != cur.get()) {
        m_proxyHold = cur;
        m_proxyFor = cur.get();
        const int maxSide = 900;
        if (std::max(cur->width, cur->height) <= maxSide) {
            m_proxy = cur;
        } else {
            const QImage small = ImageConvert::toQImage(*cur).scaled(maxSide, maxSide, Qt::KeepAspectRatio,
                                                                      Qt::SmoothTransformation);
            m_proxy = ImageConvert::fromQImage(small);
        }
    }
    const double s = static_cast<double>(m_proxy->width) / cur->width;
    if (p.blur)
        p.blur = std::max(1, static_cast<int>(std::lround(p.blur * s * s)));
    const QRect full = m_canvas->selection();
    QRect sel;
    if (!full.isEmpty())
        sel = QRect(int(std::floor(full.x() * s)), int(std::floor(full.y() * s)),
                    int(std::ceil(full.width() * s)), int(std::ceil(full.height() * s)));

    m_previewRunning = true;
    m_runningGen = m_previewGen;
    const auto proxy = m_proxy;
    m_previewWatcher.setFuture(QtConcurrent::run([proxy, p, sel]() -> QImage {
        try {
            Image img(*proxy);
            if (!sel.isEmpty())
                ImageOps::applyInRegion(img, sel.x(), sel.y(), sel.width(), sel.height(),
                                        [&p](Image &r) { ImageOps::applyAdjustments(r, p); });
            else
                ImageOps::applyAdjustments(img, p);
            return ImageConvert::toQImage(img);
        } catch (...) {
            return QImage();
        }
    }));
}

// ============================================================ dialogs

void MainWindow::resizeDialog()
{
    const auto cur = m_history.current();
    if (!cur) return;
    QDialog d(this);
    d.setWindowTitle(tr("Resize Image"));
    auto *form = new QFormLayout(&d);
    auto *w = new QSpinBox, *h = new QSpinBox;
    for (QSpinBox *s : {w, h}) { s->setRange(1, 30000); s->setSuffix(" px"); }
    w->setValue(cur->width);
    h->setValue(cur->height);
    auto *keep = new QCheckBox(tr("Keep aspect ratio"));
    keep->setChecked(true);
    const double ratio = static_cast<double>(cur->width) / cur->height;
    connect(w, &QSpinBox::valueChanged, &d, [=](int v) {
        if (keep->isChecked()) { QSignalBlocker b(h); h->setValue(std::max(1, int(std::lround(v / ratio)))); }
    });
    connect(h, &QSpinBox::valueChanged, &d, [=](int v) {
        if (keep->isChecked()) { QSignalBlocker b(w); w->setValue(std::max(1, int(std::lround(v * ratio)))); }
    });
    form->addRow(tr("Width"), w);
    form->addRow(tr("Height"), h);
    form->addRow(keep);
    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(box);
    connect(box, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &d, &QDialog::reject);
    if (d.exec() != QDialog::Accepted) return;
    const int nw = w->value(), nh = h->value();
    runOp(tr("Resize"), [nw, nh](Image &i) { ImageFilter::Resize(i, nw, nh); }, false);
}

void MainWindow::frameDialog()
{
    QDialog d(this);
    d.setWindowTitle(tr("Add Frame"));
    auto *form = new QFormLayout(&d);
    auto color = std::make_shared<QColor>(Qt::black);
    auto *colorBtn = new QPushButton(tr("Choose…"));
    auto paint = [colorBtn, color]() {
        colorBtn->setStyleSheet(QStringLiteral("QPushButton{background-color:%1;color:%2;}")
                                    .arg(color->name(), color->lightness() > 128 ? "#000" : "#fff"));
    };
    paint();
    connect(colorBtn, &QPushButton::clicked, &d, [&d, color, paint]() {
        const QColor c = QColorDialog::getColor(*color, &d, tr("Frame Color"));
        if (c.isValid()) { *color = c; paint(); }
    });
    auto *thickness = new QSpinBox;
    thickness->setRange(1, 1000);
    thickness->setValue(20);
    thickness->setSuffix(" px");
    auto *fancy = new QCheckBox(tr("Fancy (decorative lines)"));
    form->addRow(tr("Color"), colorBtn);
    form->addRow(tr("Thickness"), thickness);
    form->addRow(fancy);
    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(box);
    connect(box, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &d, &QDialog::reject);
    if (d.exec() != QDialog::Accepted) return;
    const Color c = toColor(*color);
    const int t = thickness->value();
    const bool f = fancy->isChecked();
    runOp(tr("Add Frame"), [c, t, f](Image &i) { ImageFilter::AddFrame(i, c, t, f); }, false);
}

void MainWindow::mergeDialog()
{
    if (!m_history.current()) return;
    QDialog d(this);
    d.setWindowTitle(tr("Merge with Image"));
    auto *form = new QFormLayout(&d);
    auto *path = new QLineEdit;
    auto *browse = new QPushButton(tr("Browse…"));
    auto *pathRow = new QHBoxLayout;
    pathRow->addWidget(path, 1);
    pathRow->addWidget(browse);
    auto *mode = new QComboBox;
    mode->addItems({tr("Scale to common size"), tr("Place on common area")});
    auto *alpha = new QSpinBox;
    alpha->setRange(0, 100);
    alpha->setValue(50);
    alpha->setSuffix(" %");
    auto *ox = new QSpinBox, *oy = new QSpinBox;
    for (QSpinBox *s : {ox, oy}) { s->setRange(-30000, 30000); s->setEnabled(false); }
    connect(mode, &QComboBox::currentIndexChanged, &d, [=](int i) { ox->setEnabled(i == 1); oy->setEnabled(i == 1); });
    connect(browse, &QPushButton::clicked, &d, [&]() {
        const QString p = QFileDialog::getOpenFileName(&d, tr("Overlay Image"), QString(), tr(kOpenFilter));
        if (!p.isEmpty()) path->setText(p);
    });
    form->addRow(tr("Overlay"), pathRow);
    form->addRow(tr("Mode"), mode);
    form->addRow(tr("Overlay opacity"), alpha);
    form->addRow(tr("Offset X"), ox);
    form->addRow(tr("Offset Y"), oy);
    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(box);
    connect(box, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &d, &QDialog::reject);
    if (d.exec() != QDialog::Accepted || path->text().isEmpty()) return;

    QString err;
    const auto loaded = ImageOps::loadEditor(path->text(), &err);
    if (!loaded) {
        QMessageBox::warning(this, tr("Merge"), tr("Could not open the overlay image:\n%1").arg(err));
        return;
    }
    const auto overlay = std::make_shared<Image>(loaded->getImage());
    const int a = alpha->value(), x = ox->value(), y = oy->value();
    if (mode->currentIndex() == 0)
        runOp(tr("Merge (Scale)"), [overlay, a](Image &i) { Image o(*overlay); ImageFilter::MergeByScaling(i, o, a); }, false);
    else
        runOp(tr("Merge (Common Area)"), [overlay, a, x, y](Image &i) {
            Image o(*overlay);
            ImageFilter::MergeByCommonArea(i, o, a, x, y);
        }, false);
}

void MainWindow::skewDialog()
{
    bool ok = false;
    const int angle = QInputDialog::getInt(this, tr("Skew"), tr("Angle (degrees):"), 20, -75, 75, 1, &ok);
    if (!ok) return;
    const Color fill = toColor(m_swatch->background());
    runOp(tr("Skew"), [angle, fill](Image &i) { ImageOps::skew(i, angle, fill); }, false);
}
