#include "navbar.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QIcon>
#include <QFrame>

Navbar::Navbar(QWidget *parent)
    : QWidget(parent)
    , undoAction(nullptr)
    , redoAction(nullptr)
    , resetAction(nullptr)
{
    setFixedHeight(52);

    // =========================================================
    // Main layout
    // =========================================================

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(4);


    // =========================================================
    // FILE BUTTON
    // =========================================================

    QToolButton *fileButton = createNavButton("File");

    QMenu *fileMenu = createFileMenu();

    fileButton->setMenu(fileMenu);
    fileButton->setPopupMode(QToolButton::InstantPopup);

    layout->addWidget(fileButton);

    // =========================================================
    // EDIT BUTTON
    // =========================================================

    QToolButton *editButton = createNavButton("Edit");
    editButton->setMenu(createEditMenu());
    editButton->setPopupMode(QToolButton::InstantPopup);
    layout->addWidget(editButton);

    layout->addStretch();

    // =========================================================
    // STYLE
    // =========================================================

    setStyleSheet(R"(
        Navbar {
            background-color: #252526;
            border-bottom: 1px solid #333333;
        }

        QToolButton {
            background-color: transparent;
            color: #dddddd;

            border: none;
            border-radius: 6px;

            padding: 7px 12px;

            font-size: 13px;
            font-weight: 500;
        }

        QToolButton:hover {
            background-color: #333333;
        }

        QToolButton:pressed {
            background-color: #3d3d3d;
        }

        QMenu {
            background-color: #252526;

            color: #dddddd;

            border: 1px solid #3f3f46;
            border-radius: 7px;

            padding: 5px;
        }

        QMenu::item {
            padding: 8px 35px 8px 30px;

            border-radius: 4px;
        }

        QMenu::item:selected {
            background-color: #3a3a3a;
        }

        QMenu::separator {
            height: 1px;
            background: #3a3a3a;
            margin: 5px 8px;
        }
    )");
}

QMenu *Navbar::createFileMenu()
{
    QMenu *menu = new QMenu(this);

    // ---------------------------------------------------------
    // Open
    // ---------------------------------------------------------

    QAction *openAction = new QAction(
        style()->standardIcon(QStyle::SP_DialogOpenButton),
        "Open File",
        this
        );

    openAction->setShortcut(QKeySequence("Ctrl+O"));

    connect(openAction, &QAction::triggered,
            this, &Navbar::openFileRequested);


    // ---------------------------------------------------------
    // Save
    // ---------------------------------------------------------

    QAction *saveAction = new QAction(
        style()->standardIcon(QStyle::SP_DialogSaveButton),
        "Save File",
        this
        );

    saveAction->setShortcut(QKeySequence("Ctrl+S"));

    connect(saveAction, &QAction::triggered,
            this, &Navbar::saveFileRequested);


    // ---------------------------------------------------------
    // Close
    // ---------------------------------------------------------

    QAction *closeAction = new QAction(
        style()->standardIcon(QStyle::SP_DialogCloseButton),
        "Close File",
        this
        );

    connect(closeAction, &QAction::triggered,
            this, &Navbar::closeFileRequested);


    // ---------------------------------------------------------
    // Exit
    // ---------------------------------------------------------

    QAction *exitAction = new QAction(
        style()->standardIcon(QStyle::SP_DialogCancelButton),
        "Exit",
        this
        );

    connect(exitAction, &QAction::triggered,
            this, &Navbar::exitRequested);


    // ---------------------------------------------------------
    // Add everything
    // ---------------------------------------------------------

    menu->addAction(openAction);
    menu->addAction(saveAction);

    menu->addSeparator();

    menu->addAction(closeAction);

    menu->addSeparator();

    menu->addAction(exitAction);

    return menu;
}

QMenu *Navbar::createEditMenu()
{
    QMenu *menu = new QMenu(this);

    undoAction = new QAction(
        style()->standardIcon(QStyle::SP_ArrowBack),
        "Undo",
        this
        );
    undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    connect(undoAction, &QAction::triggered,
            this, &Navbar::undoRequested);

    redoAction = new QAction(
        style()->standardIcon(QStyle::SP_ArrowForward),
        "Redo",
        this
        );
    redoAction->setShortcut(QKeySequence("Ctrl+Y"));
    connect(redoAction, &QAction::triggered,
            this, &Navbar::redoRequested);

    resetAction = new QAction(
        style()->standardIcon(QStyle::SP_BrowserReload),
        "Reset",
        this
        );
    resetAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(resetAction, &QAction::triggered,
            this, &Navbar::resetRequested);

    menu->addAction(undoAction);
    menu->addAction(redoAction);
    menu->addSeparator();
    menu->addAction(resetAction);

    return menu;
}

void Navbar::setButtonEnabled(bool canUndo, bool canRedo, bool hasImage){
    undoAction->setEnabled(canUndo);
    redoAction->setEnabled(canRedo);
    resetAction->setEnabled(hasImage);
}

QToolButton *Navbar::createNavButton(const QString &text)
{
    QToolButton *button = new QToolButton(this);

    button->setText(text);

    button->setCursor(Qt::PointingHandCursor);

    return button;
}



