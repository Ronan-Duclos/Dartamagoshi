#pragma once

#include <QWidget>
#include <QOpenGLWidget>

class MyOpenGlWidget : public QOpenGLWidget
{
public:
    MyOpenGlWidget(QWidget *parent);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
