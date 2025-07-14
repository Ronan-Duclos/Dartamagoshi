#include "MyLabel.hpp"

#include <QEvent>
#include <QMouseEvent>

MyLabel::MyLabel(QWidget *parent) : QLabel(parent), m_equiped(false)
{

}

QString MyLabel::getImgName() const
{
    return m_imgName;
}

void MyLabel::setEquiped(const bool is)
{
    m_equiped = is;
}

bool MyLabel::isEquiped() const
{
    return m_equiped;
}

void MyLabel::setPos(const int pos)
{
    m_pos = pos;
}

int MyLabel::getPos() const
{
    return m_pos;
}

void MyLabel::setPixmap(const QString imgName)
{
    QPixmap img(imgName);

    m_imgName = imgName;
    QLabel::setPixmap(img);
}

void MyLabel::mouseDoubleClickEvent(const QMouseEvent *event)
{
    emit doubleClicked(this);
}

void MyLabel::mouseReleaseEvent(const QMouseEvent *event)
{

    emit clicked(this);
}
