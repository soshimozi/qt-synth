#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <QObject>
#include <QPixmap>
#include <QVector>

class SpriteSheet : public QObject {
    Q_OBJECT

public:
    explicit SpriteSheet(QObject *parent = nullptr);

    // Load the sprite sheet image
    void setSource(const QString &file_path);
    const QPixmap& source() const { return source_; }

    // Setters and getters for orientation and cells
    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const { return orientation_; }

    void setCells(int cells);
    int cells() const { return cells_; }

    // Get the list of cropped images
    const QVector<QPixmap>& croppedImages();

private:
    QPixmap source_;
    Qt::Orientation orientation_;
    int cells_;
    QVector<QPixmap> cropped_images_;

private:
    void generateCroppedImages();    
};

#endif
