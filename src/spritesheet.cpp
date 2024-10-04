#include "spritesheet.h"

SpriteSheet::SpriteSheet(QObject *parent)
    : QObject(parent), m_orientation(Qt::Horizontal), m_cells(1) {}


void SpriteSheet::setSource(const QString &file_path) {
    m_source = QPixmap(file_path);
    generateCroppedImages();
}

void SpriteSheet::setOrientation(Qt::Orientation orientation) {
    m_orientation = orientation;
    generateCroppedImages();
}

void SpriteSheet::setCells(int cells) {
    m_cells = cells;
    generateCroppedImages();
}

const QVector<QPixmap>& SpriteSheet::croppedImages() {
    return m_croppedImages;
}

void SpriteSheet::generateCroppedImages() {
    m_croppedImages.clear();

    if(m_cells <= 0 || m_source.isNull()) return;

    int spriteWidth = m_source.width() / (m_orientation == Qt::Horizontal ? m_cells : 1);
    int spriteHeight = m_source.height() / (m_orientation == Qt::Vertical ? m_cells : 1);;

    for(int i = 0; i < m_cells; ++i) {
        int x = (m_orientation == Qt::Horizontal) ? i * spriteWidth : 0;
        int y = (m_orientation == Qt::Vertical) ? i * spriteHeight : 0;

        QPixmap croppedImage = m_source.copy(x, y, spriteWidth, spriteHeight);
        m_croppedImages.append(croppedImage);
    }
}
