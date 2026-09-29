#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QWidget>
#include <QPoint>

class QLineEdit;
class QPushButton;

class KeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(QWidget *parent = nullptr);

    void setTarget(QLineEdit *lineEdit);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QLineEdit *targetEdit;
    bool shiftEnabled;

    bool dragging;
    QPoint dragOffset;

    QPushButton *createKey(const QString &text);
    void insertCharacter(const QString &text);
};

#endif