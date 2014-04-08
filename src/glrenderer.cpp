#include "glrenderer.h"

GLRenderer::GLRenderer(QObject *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers))
{
    setMinimumSize(320,240);
    setAttribute(Qt::WA_DeleteOnClose);
}

void GLRenderer::renderPixmap(const QPixmap &_pixmap)
{
    pixmap = _pixmap;
    resizeEvent(NULL);
    this->update();
}

void GLRenderer::resizeEvent(QResizeEvent *)
{
    //retain aspect ratio when changing size
    float srcAspectRt = pixmap.width() / (float)pixmap.height();
    float dstAspectRt = this->width() / (float)this->height();
    float width;

    if (srcAspectRt > dstAspectRt)
        width = this->width();
    else
        width = this->height() * srcAspectRt;

    double scale = width / pixmap.width();

    scaleMat = QTransform().scale(scale, scale);
    scaleMatI = scaleMat.inverted();
}

void GLRenderer::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldTransform(scaleMat);
    QRect scaled = scaleMatI.mapRect(e->rect());
    p.drawPixmap(scaled, pixmap, scaled);
}
