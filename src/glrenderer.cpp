#include "glrenderer.h"

GLRenderer::GLRenderer(QObject *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers))
{
    setMinimumSize(320,240);
}

void GLRenderer::renderImage(const QImage &_frame)
{
    //frame = _frame;
    frame = QGLWidget::convertToGLFormat(_frame);
    this->updateGL();
}

void GLRenderer::initializeGL()
{
    glClearColor(0.0f,0.0f,0.0f,1.0f);
}

void GLRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    if(!frame.isNull()) {
        frame = frame.scaledToWidth(this->width());
        glDrawPixels(frame.width(), frame.height(),
                     GL_RGBA, GL_UNSIGNED_BYTE, frame.bits());
    }
}

void GLRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, 0, 1);
    //gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
