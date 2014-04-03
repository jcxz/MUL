#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <QGLWidget>
#include <QImage>

class GLRenderer : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLRenderer(QObject *parent = 0);

    void renderImage(const QImage &_frame);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
    QImage frame;

signals:

public slots:

};

#endif // GLRENDERER_H
