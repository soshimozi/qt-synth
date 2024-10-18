#include "spritesheet.h"

SpriteSheet::SpriteSheet(QObject *parent)
    : QObject(parent), orientation_(Qt::Horizontal), cells_(1) {}


void SpriteSheet::setSource(const QString &file_path) {
    source_ = QPixmap(file_path);
    generateCroppedImages();
}

void SpriteSheet::setOrientation(Qt::Orientation orientation) {
    orientation_ = orientation;
    generateCroppedImages();
}

void SpriteSheet::setCells(int cells) {
    cells_ = cells;
    generateCroppedImages();
}

const QVector<QPixmap>& SpriteSheet::croppedImages() {
    return cropped_images_;
}

void SpriteSheet::generateCroppedImages() {
    cropped_images_.clear();

    if(cells_ <= 0 || source_.isNull()) return;

    int spriteWidth = source_.width() / (orientation_ == Qt::Horizontal ? cells_ : 1);
    int spriteHeight = source_.height() / (orientation_ == Qt::Vertical ? cells_ : 1);;

    for(int i = 0; i < cells_; ++i) {
        int x = (orientation_ == Qt::Horizontal) ? i * spriteWidth : 0;
        int y = (orientation_ == Qt::Vertical) ? i * spriteHeight : 0;

        QPixmap croppedImage = source_.copy(x, y, spriteWidth, spriteHeight);
        cropped_images_.append(croppedImage);
    }
}
