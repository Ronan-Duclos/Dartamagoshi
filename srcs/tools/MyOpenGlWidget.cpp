#include "MyOpenGlWidget.hpp"

#include <QOpenGLFunctions>

MyOpenGlWidget::MyOpenGlWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

void MyOpenGlWidget::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void MyOpenGlWidget::resizeGL(int w, int h)
{

}

void MyOpenGlWidget::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
}
