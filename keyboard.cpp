#include "keyboard.h"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPalette>
#include <QColor>

KeyboardWidget::KeyboardWidget(QWidget *parent)
    : QWidget(parent),
    targetEdit(nullptr),
    shiftEnabled(false),
    dragging(false)
{
    setAutoFillBackground(true);
    setAttribute(Qt::WA_StyledBackground, true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(35, 35, 35));
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // ROW 1
    QHBoxLayout *row1 = new QHBoxLayout;

    const QString row1Keys = "QWERTYUIOPĞÜ";

    for (const QChar &c : row1Keys) {
        QPushButton *button = createKey(QString(c));
        row1->addWidget(button);
    }

    // ROW 2
    QHBoxLayout *row2 = new QHBoxLayout;

    const QString row2Keys = "ASDFGHJKLŞİ";

    for (const QChar &c : row2Keys) {
        QPushButton *button = createKey(QString(c));
        row2->addWidget(button);
    }

    // ROW 3
    QHBoxLayout *row3 = new QHBoxLayout;

    QPushButton *shiftButton = new QPushButton("SHIFT");

    connect(shiftButton, &QPushButton::clicked,
            this, [this]() {
                shiftEnabled = !shiftEnabled;
            });

    row3->addWidget(shiftButton);

    const QString row3Keys = "ZXCVBNMÖÇ";

    for (const QChar &c : row3Keys) {
        QPushButton *button = createKey(QString(c));
        row3->addWidget(button);
    }

    QPushButton *backspaceButton = new QPushButton("BACK");

    connect(backspaceButton, &QPushButton::clicked,
            this, [this]() {
                if (targetEdit)
                    targetEdit->backspace();
            });

    row3->addWidget(backspaceButton);

    // ROW 4
    QHBoxLayout *row4 = new QHBoxLayout;

    QPushButton *numbersButton = createKey("123");
    QPushButton *spaceButton = new QPushButton("SPACE");
    QPushButton *dotButton = createKey(".");
    QPushButton *commaButton = createKey(",");
    QPushButton *closeButton = new QPushButton("CLOSE");

    connect(spaceButton, &QPushButton::clicked,
            this, [this]() {
                if (targetEdit)
                    targetEdit->insert(" ");
            });

    connect(closeButton, &QPushButton::clicked,
            this, [this]() {
                hide();

                if (targetEdit)
                    targetEdit->clearFocus();
            });

    row4->addWidget(numbersButton);
    row4->addWidget(spaceButton, 4);
    row4->addWidget(dotButton);
    row4->addWidget(commaButton);
    row4->addWidget(closeButton);

    mainLayout->addLayout(row1);
    mainLayout->addLayout(row2);
    mainLayout->addLayout(row3);
    mainLayout->addLayout(row4);

    setLayout(mainLayout);


    setStyleSheet(
        "KeyboardWidget {"
        "   background-color: rgb(35, 35, 35);"
        "   border: 3px solid rgb(100, 100, 100);"
        "}"

        "QPushButton {"
        "   background-color: rgb(245, 245, 245);"
        "   color: rgb(20, 20, 20);"
        "   border: 2px solid rgb(120, 120, 120);"
        "   border-radius: 8px;"
        "   min-height: 55px;"
        "   font-size: 26px;"
        "   font-weight: bold;"
        "}"

        "QPushButton:pressed {"
        "   background-color: rgb(180, 180, 180);"
        "}"
        );

    hide();
}

QPushButton *KeyboardWidget::createKey(const QString &text)
{
    QPushButton *button = new QPushButton(text);

    connect(button, &QPushButton::clicked,
            this, [this, text]() {
                insertCharacter(text);
            });

    return button;
}

void KeyboardWidget::insertCharacter(const QString &text)
{
    if (!targetEdit)
        return;

    QString character = text;

    if (text.length() == 1 && text[0].isLetter()) {
        if (shiftEnabled)
            character = text.toUpper();
        else
            character = text.toLower();
    }

    targetEdit->insert(character);

    // Normal keyboard behaviour: Shift affects one character.
    if (shiftEnabled)
        shiftEnabled = false;
}

void KeyboardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragOffset = event->pos();
        raise();
    }

    QWidget::mousePressEvent(event);
}

void KeyboardWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPosition =
            mapToParent(event->pos() - dragOffset);

        move(newPosition);
    }

    QWidget::mouseMoveEvent(event);
}

void KeyboardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragging = false;

    QWidget::mouseReleaseEvent(event);
}

void KeyboardWidget::setTarget(QLineEdit *lineEdit)
{
    targetEdit = lineEdit;
}