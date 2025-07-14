#pragma once

#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    MyLabel(QWidget *parent = 0);

    QString getImgName() const;
    void setEquiped(const bool is);
    bool isEquiped() const;
    void setPos(const int pos);
    int getPos() const;

public slots:
    void setPixmap(const QString imgName);

signals:
    void doubleClicked(const MyLabel *clicked);
    void clicked(const MyLabel *clicked);

protected:
    void mouseDoubleClickEvent(const QMouseEvent *event);
    void mouseReleaseEvent(const QMouseEvent *event);

private:
    QString m_imgName;
    bool    m_equiped;
    int     m_pos;
};

