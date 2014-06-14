/*
 * Copyright (C) 2014 Pavel Csoka <xcsoka00@stud.fit.vutbr.cz>
 *
 * This file is part of MUL.
 *
 * MUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MUL. If not, see <http://www.gnu.org/licenses/>.
 */

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
