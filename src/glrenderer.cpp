/*!
  \author Pavel Csoka (xcsoka00@stud.fit.vutbr.cz)
  \date   14.5.2013
  \file   glrenderer.h
  \brief  GLRenderer class implementation file
*/

#include "glrenderer.h"
#include <QResizeEvent>

GLRenderer::GLRenderer(QWidget *parent) :
    QGLWidget(parent)
{
    qDebug("openGL support: %i", this->isValid());
    //resizing = false;
    //connect(&resizeTimer, SIGNAL(timeout()), this, SLOT(enableUpdates()));
    //connect(&displayTimer, SIGNAL(timeout()), this, SLOT(update()));

    freeSlots = new QSemaphore(2);
    usedSlots = new QSemaphore(0);

    connect(&displayTimer, SIGNAL(timeout()), this, SLOT(render()));
    //displayTimer.start(33);
}

GLRenderer::~GLRenderer()
{
    delete freeSlots;
    delete usedSlots;
}

void GLRenderer::play(int ms)
{
    displayTimer.start(ms);
}

void GLRenderer::stop()
{
    if(displayTimer.isActive()) {
        displayTimer.stop();
        //frameBuffer = std::queue<QImage, std::list<QImage> >();
    }
}

void GLRenderer::resizeGL(int width, int height) {
    //mutex.lock();
    // Setup our viewport to be the entire size of the window
    glViewport(0, 0, width, height);

    // Change to the projection matrix and set orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //mutex.unlock();
}

//void GLRenderer::resizeEvent(QResizeEvent *e)
//{
//    //disable widget updates while resizing
//    //this->setEnabled(false);
//    //when resizing event occurs, restart timer
//    //when timer timeouts, enable widget updates
//    resizing = true;
//    resizeTimer.stop();
//    resizeTimer.start(300);
//    newSize = e->size();
//}

//void GLRenderer::enableUpdates()
//{
//    //this->setEnabled(true);
//    resizeTimer.stop();
//    this->resizeGL(newSize.width(), newSize.height());
//    resizing = false;
//}

void GLRenderer::paintGL()
{
   // mutex.lock();
    //Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor (0.0, 0.0, 0.0, 1.0);

    if(!GLFrame.isNull()) {
//        GLFrame = GLFrame.scaled(this->size(), Qt::KeepAspectRatio,
//                                 Qt::FastTransformation);

        //enable texturing
        glEnable(GL_TEXTURE_2D);
        //set parameters for current texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //upload texture to video memory
        //!! width and height must be power of two - IT HAS TO BE MULTIPLES OF 4
        //apparently only width has to be...
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLFrame.width(),
                     GLFrame.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, GLFrame.bits());
        //apply texture - openCV coordinates starts at top-left, openGL at bottom-left
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, this->height());
        glTexCoord2f(0, 1); glVertex2f(0, 0);
        glTexCoord2f(1, 1); glVertex2f(this->width(), 0);
        glTexCoord2f(1, 0); glVertex2f(this->width(), this->height());
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glFlush();
    }

    //mutex.unlock();
}

void GLRenderer::renderImage(const QImage &frame)
{
//mutex.lock();
//    if(!frame.isNull()) {
//        GLFrame = QGLWidget::convertToGLFormat(frame);
//        //this->updateGL();
//    }
//mutex.unlock();

    if(freeSlots->tryAcquire()) {
        bufferMutex.lock();
            frameBuffer.push(frame);
        bufferMutex.unlock();

        usedSlots->release();
    }
    //qDebug("frame buffer full");
}

void GLRenderer::renderSingleImage(const QImage &frame)
{
    GLFrame = QGLWidget::convertToGLFormat(frame);
    this->updateGL();
}

void GLRenderer::render()
{
    QImage frame;
    if(usedSlots->tryAcquire()) {
        bufferMutex.lock();
            frame = frameBuffer.front();
            frameBuffer.pop();
        bufferMutex.unlock();

        freeSlots->release();

        GLFrame = QGLWidget::convertToGLFormat(frame);
        this->updateGL();
    }
}
