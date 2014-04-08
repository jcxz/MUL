#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <QGLWidget>
#include <QImage>
#include <QtOpenGL>
#include <QtGui>

/*!
 \brief Renderer class.

 Effectively displays video frames using OpenGL.
*/
class GLRenderer : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLRenderer(QObject *parent = 0);

    void renderPixmap(const QPixmap &_pixmap);

    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *e);
protected:

private:
    QPixmap pixmap;
    QTransform scaleMat, scaleMatI;

signals:
    //void update();

public slots:

};

#endif // GLRENDERER_H
